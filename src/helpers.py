
import ROOT as root
from ROOT import gStyle as gStyle
import array

def setNormalColorTable():
  rArray = array.array('d',[0.0,1.0,1.0])
  gArray = array.array('d',[1.0,1.0,0.0])
  bArray = array.array('d',[0.0,0.0,0.0])
  stopArray = array.array('d',[0.,0.5,1.])
  nTabColors = 500
  root.TColor.CreateGradientColorTable(len(stopArray),
            stopArray,rArray,gArray,bArray,nTabColors
         )
def setInvertColorTable():
  rArray = array.array('d',[1.0,1.0,0.0])
  gArray = array.array('d',[0.0,1.0,1.0])
  bArray = array.array('d',[0.0,0.0,0.0])
  stopArray = array.array('d',[0.,0.5,1.])
  nTabColors = 500
  root.TColor.CreateGradientColorTable(len(stopArray),
            stopArray,rArray,gArray,bArray,nTabColors
         )

def setStyle():
  gStyle.SetCanvasColor(0)
  gStyle.SetCanvasBorderSize(10)
  gStyle.SetCanvasBorderMode(0)
  gStyle.SetCanvasDefH(700)
  gStyle.SetCanvasDefW(700)

  gStyle.SetPadColor       (0)
  gStyle.SetPadBorderSize  (10)
  gStyle.SetPadBorderMode  (0)
  gStyle.SetPadBottomMargin(0.13)
  gStyle.SetPadTopMargin   (0.08)
  gStyle.SetPadLeftMargin  (0.15)
  gStyle.SetPadRightMargin (0.05)
  gStyle.SetPadGridX       (0)
  gStyle.SetPadGridY       (0)
  gStyle.SetPadTickX       (1)
  gStyle.SetPadTickY       (1)

  gStyle.SetFrameFillStyle ( 0)
  gStyle.SetFrameFillColor ( 0)
  gStyle.SetFrameLineColor ( 1)
  gStyle.SetFrameLineStyle ( 0)
  gStyle.SetFrameLineWidth ( 1)
  gStyle.SetFrameBorderSize(10)
  gStyle.SetFrameBorderMode( 0)

  gStyle.SetNdivisions(505)

  gStyle.SetLineWidth(2)
  gStyle.SetHistLineWidth(2)
  gStyle.SetFrameLineWidth(2)
  gStyle.SetLegendFillColor(root.kWhite)
  gStyle.SetLegendFont(42)
  gStyle.SetMarkerSize(1.2)
  gStyle.SetMarkerStyle(20)
 
  gStyle.SetLabelSize(0.040,"X")
  gStyle.SetLabelSize(0.040,"Y")

  gStyle.SetLabelOffset(0.010,"X")
  gStyle.SetLabelOffset(0.010,"Y")
 
  gStyle.SetLabelFont(42,"X")
  gStyle.SetLabelFont(42,"Y")
 
  gStyle.SetTitleBorderSize(0)
  gStyle.SetTitleFont(42)
  gStyle.SetTitleFont(42,"X")
  gStyle.SetTitleFont(42,"Y")

  gStyle.SetTitleSize(0.045,"X")
  gStyle.SetTitleSize(0.045,"Y")
 
  gStyle.SetTitleOffset(1.4,"X")
  gStyle.SetTitleOffset(1.4,"Y")
 
  gStyle.SetTextSize(0.055)
  gStyle.SetTextFont(42)
 
  gStyle.SetOptStat(0)
  setNormalColorTable()
  
setStyle()

def setHistTitles(hist,xlabel,ylabel):
    hist.GetXaxis().SetTitle(xlabel)
    hist.GetYaxis().SetTitle(ylabel)

class DataMCStack:
  def __init__(self, mcHistList, dataHist, canvas, xtitle, ytitle="", drawStack=True,nDivX=7,xlimits=[],showOverflow=False,lumi=5.0,logy=False,signalsNoStack=[],showCompatabilityTests=True,integralPlot=False,energyStr="8TeV",ylimits=[],ylimitsRatio=[],pullType="",doMCErrors=False,showPullStats=False,yMaxVals=[],yMaxXRanges=[],mcVariations=None,scaleMC2Data=False):
    nBinsX = dataHist.GetNbinsX()
    self.xlimits = xlimits
    self.ylimits = ylimits
    self.logy = logy
    self.nBinsX = nBinsX
    self.dataHist = dataHist
    self.canvas = canvas
    self.tlatex = root.TLatex()
    self.tlatex.SetNDC()
    self.tlatex.SetTextFont(root.gStyle.GetLabelFont())
    self.tlatex.SetTextSize(0.05)
    self.tlatex.SetTextAlign(22)
    self.mcVarHist = None
    setYLimitsAuto = getattr(self,"setYLimitsAuto")
    if ytitle=="":
      ytitle="Events/%s" % (getBinWidthStr(dataHist))
    for mcHist in mcHistList:
      #print("nBinsX data: %i, mc: %i" % (nBinsX,mcHist.GetNbinsX()))
      assert(nBinsX == mcHist.GetNbinsX())
    for sigHist in signalsNoStack:
      assert(nBinsX == sigHist.GetNbinsX())

    if integralPlot:
      dataHist = getIntegralHist(dataHist,True)
      self.dataHist = dataHist
      newMcHistList = []
      for i in mcHistList:
        newMcHistList.append(getIntegralHist(i))
      mcHistList = newMcHistList
      newSigHistList = []
      for i in signalsNoStack:
        newSigHistList.append(getIntegralHist(i))
      signalsNoStack = newSigHistList
      ytitle = "Integral of "+ytitle+" #geq X"
    self.signalsNoStack = signalsNoStack
    self.mcHistList = mcHistList
    self.dataHist = dataHist

    self.nDataEvents = dataHist.Integral(0,dataHist.GetNbinsX()+1)
    self.mc2DataSF = 1.
    if scaleMC2Data:
      tmpMCSum = 0.
      for mcHist in mcHistList:
        tmpMCSum += mcHist.Integral(0,mcHist.GetNbinsX()+1)
      self.mc2DataSF = float(self.nDataEvents)/tmpMCSum
      print("DataMC SF: %.2f" % self.mc2DataSF)

    # Make MC Stack/sumHist
    self.stack = root.THStack()
    self.mcSumHist = dataHist.Clone("mcSumHist"+dataHist.GetName())
    self.mcSumHist.Reset()
    for mcHist in mcHistList:
      mcHist.SetMaximum(1e12)
      mcHist.SetMinimum(1e-12)
      mcHist.SetLineColor(mcHist.GetFillColor())
      if showOverflow:
        showHistOverflow(mcHist)
      mcHist.Scale(self.mc2DataSF)
      self.mcSumHist.Add(mcHist)
      self.stack.Add(mcHist)

    if showOverflow:
        showHistOverflow(dataHist)

    self.doMCVariations(mcVariations)

    self.mcSumHist.SetFillColor(root.kGray+3)
    self.mcSumHist.SetFillStyle(3254)
    self.mcSumHist.SetMarkerSize(0)
    if doMCErrors and drawStack:
        self.mcSumHist.SetLineStyle(0)

    self.nMCEvents = self.mcSumHist.Integral(0,self.mcSumHist.GetNbinsX()+1)

    # Get chi^2 Prob Data/MC
    self.normchi2 = dataHist.Chi2Test(self.mcSumHist,"UW CHI2/NDF")
    self.chi2Prob = dataHist.Chi2Test(self.mcSumHist,"UW")
    self.KSProb = dataHist.KolmogorovTest(self.mcSumHist)
    if self.mcVarHist != None:
      self.normchi2 = dataHist.Chi2Test(self.mcVarHist,"UW CHI2/NDF")
      self.chi2Prob = dataHist.Chi2Test(self.mcVarHist,"UW")
      self.KSProb = dataHist.KolmogorovTest(self.mcVarHist)
    if self.chi2Prob < 1e-20:
        self.chi2Prob = 0.0
    if self.KSProb < 1e-20:
        self.KSProb = 0.0

    # Make Pull Hist
    self.pullList = []
    self.pullHist = dataHist.Clone("pullHist"+dataHist.GetName())
    self.pullHist.Reset()
    self.oneGraph = root.TGraph()
    self.oneGraph.SetLineWidth(2)
    self.oneGraph.SetLineStyle(2)
    iGraph = 0
    for i in range(0,self.pullHist.GetNbinsX()+2):
      nData = dataHist.GetBinContent(i)
      nMC = self.mcSumHist.GetBinContent(i)
      error = dataHist.GetBinError(i)
      errorMC = self.mcSumHist.GetBinError(i)
      if self.mcVarHist != None:
        errorMC = self.mcVarHist.GetBinError(i)
      pull = 0.0
      ratio = 0.0
      ratioErr = 0.0
      self.oneGraph.SetPoint(iGraph,dataHist.GetXaxis().GetBinCenter(i),1.0)
      iGraph += 1
      if error != 0.0:
        if pullType=="adrian1":
          pull = (nData -nMC)/nData
        else:
          pull = (nData -nMC)/error
      if pullType=="pullMC":
        if errorMC != 0.0:
          pull = (nData -nMC)/errorMC
        else:
          pull = 0.0
      if nMC != 0.0:
        ratio = nData/nMC
        ratioErr = error/nMC
      if pullType=="ratio":
        self.pullHist.SetBinContent(i,ratio)
        self.pullHist.SetBinError(i,ratioErr)
        #print("nData: {0:.2f} +/- {1:.2f}, nMC: {2:.2f}, ratio: {3:.2f} +/- {4:.2f}".format(nData,error,nMC,ratio,ratioErr))
      else:
        self.pullHist.SetBinContent(i,pull)
        #print("nData: %f, nMC: %f, error: %f, pull: %f" % (nData,nMC,error,pull))
      #pullDistribution
      if pullType == "pullMC":
        if errorMC != 0.0:
          self.pullList.append((nData -nMC)/errorMC)
      else:
        if error != 0.0:
          self.pullList.append((nData -nMC)/error)
    #print getattr(self,"getPullDistributionParams")(self.pullList)

    #Find Maximum y-value
    if xlimits != []:
      self.mcSumHist.GetXaxis().SetRangeUser(*xlimits)
      self.dataHist.GetXaxis().SetRangeUser(*xlimits)
    mcMax = self.mcSumHist.GetMaximum()
    if self.mcVarHist != None:
      mcMax = self.mcSumHist.GetMaximum()
    dataMaxBin = self.dataHist.GetMaximumBin()
    dataMax = dataHist.GetBinContent(dataMaxBin)+dataHist.GetBinError(dataMaxBin)
    ymax = 0.0
    if mcMax > dataMax:
       ymax = mcMax
    else:
       ymax = dataMax
    self.ymax = ymax
  
    #Setup Canvas
    canvas.cd()
    self.pad1Top = 0.98
    self.pad1Bot = 0.30
    self.pad1Right = 0.98
    self.pad1Left = 0.02
    pad1 = root.TPad("pad1"+dataHist.GetName(),"",0.02,0.30,0.98,0.98,0)
    pad2 = root.TPad("pad2"+dataHist.GetName(),"",0.02,0.01,0.98,0.29,0)
    self.pad1 = pad1
    self.pad2 = pad2
  
    pad1.SetBottomMargin(0.005);
    pad2.SetTopMargin   (0.005);
    pad2.SetBottomMargin(0.33);
    """
    pad1.SetBottomMargin(0.01);
    pad2.SetTopMargin   (0.3);
    pad2.SetBottomMargin(0.33);
    """
    canvas.SetLogy(0)
    pad2.SetLogy(0)
    if logy:
        pad1.SetLogy(1)
    else:
        pad1.SetLogy(0)
  
    pad1.Draw() # Projections pad
    pad2.Draw() # Residuals   pad

    pad1Width = pad1.XtoPixel(pad1.GetX2())
    pad1Height = pad1.YtoPixel(pad1.GetY1())
    pad2Height = pad2.YtoPixel(pad2.GetY1())
    #pad1ToPad2FontScalingFactor = float(pad1Width)/pad2Height
    pad1ToPad2FontScalingFactor = float(pad1Height)/pad2Height
  
    # Main Pad
    pad1.cd();
    xAxis = None
    yAxis = None
    histForAxis = None
    if len(self.ylimits)==2:
      ylimits[0] += 1e-3
      histForAxis = root.TH2F(dataHist.GetName()+"ForAxis","",1,xlimits[0],xlimits[1],1,self.ylimits[0],self.ylimits[1])
    elif self.logy:
      histForAxis = root.TH2F(dataHist.GetName()+"ForAxis","",1,xlimits[0],xlimits[1],1,0.1,ymax*2.0)
    else:
      histForAxis = root.TH2F(dataHist.GetName()+"ForAxis","",1,xlimits[0],xlimits[1],1,1e-3,ymax*1.05)
    self.histForAxis = histForAxis
    self.histForAxis.Draw()
    self.mcSumHist.Draw("e1same")
    #self.canvas.SaveAs("debug.png")
    if len(self.ylimits)!=2:
      setYLimitsAuto(yMaxXRanges,yMaxVals,self.ymax)
    self.histForAxis.Draw()
    self.histForAxis.GetXaxis().SetTitle("")
    self.histForAxis.GetXaxis().SetLabelSize(0)
    self.histForAxis.GetYaxis().SetTitle(ytitle)
    self.histForAxis.GetYaxis().SetLabelSize(0.050)
    self.histForAxis.GetYaxis().SetTitleSize(0.055)
    self.histForAxis.GetXaxis().SetNdivisions(nDivX)
    self.histForAxis.GetXaxis().SetTitleColor(0)
    self.histForAxis.GetXaxis().SetLabelColor(0)
    if drawStack:
      self.stack.Draw("hist same")
      if doMCErrors:
        if self.mcVarHist != None:
          self.mcVarHist.Draw("e2same")
        self.mcSumHist.Draw("e2same")
      pad1.Update()
    else:
      self.mcSumHist.SetFillColor(856)
      self.mcSumHist.SetLineColor(856)
      self.mcSumHist.SetMarkerColor(856)
      self.mcSumHist.SetFillStyle(1001)
      self.mcSumHist.Draw("histo b")
    for sigHist in signalsNoStack:
      sigHist.Draw("histo same")
    dataHist.Draw("pe same")

    pad1.RedrawAxis() # Updates Axis Lines
  
    # Pulls Pad
    pad2.cd()
    self.pullHist.SetTitle("")
    if xlimits != []:
      self.pullHist.GetXaxis().SetRangeUser(*xlimits)
    self.pullHist.GetXaxis().SetTitle(xtitle)
    self.pullHist.GetXaxis().CenterTitle(1)
    self.pullHist.GetXaxis().SetNdivisions(nDivX)
    self.pullHist.GetXaxis().SetTitleSize(0.055*pad1ToPad2FontScalingFactor)
    self.pullHist.GetXaxis().SetLabelSize(0.050*pad1ToPad2FontScalingFactor)
    self.pullHist.SetLineColor(root.kBlue)
    self.pullHist.SetLineStyle(1)
    self.pullHist.SetLineWidth(2)
    if pullType=="adrian1":
      self.pullHist.GetYaxis().SetTitle("#frac{Data-MC}{Data}")
    elif pullType=="pullMC":
      self.pullHist.GetYaxis().SetTitle("#frac{Data-MC}{\sigma_{MC}}")
    else:
      self.pullHist.GetYaxis().SetTitle("#frac{Data-MC}{\sigma_{Data}}")
    self.pullHist.GetYaxis().SetTitleSize(0.040*pad1ToPad2FontScalingFactor)
    self.pullHist.GetYaxis().SetLabelSize(0.040*pad1ToPad2FontScalingFactor)
    self.pullHist.GetYaxis().CenterTitle(1)
    self.pullHist.GetXaxis().SetTitleOffset(0.75*self.pullHist.GetXaxis().GetTitleOffset())
    self.pullHist.GetYaxis().SetTitleOffset(0.70)
    self.pullHist.SetFillColor(856)
    self.pullHist.SetFillStyle(1001)
    if len(ylimitsRatio) == 2:
      ylimitsRatio[0] += 1e-3
      ylimitsRatio[1] -= 1e-3
      self.pullHist.GetYaxis().SetRangeUser(*ylimitsRatio)

    if pullType=="ratio":
      #pad2.SetGridy(1)
      self.pullHist.GetYaxis().SetTitle("#frac{Data}{MC}")
      self.pullHist.Draw("")
      self.oneGraph.Draw()
      self.pullHist.Draw("same")
    else:
      self.pullHist.Draw("histo")

    if showCompatabilityTests:
      self.problatex = root.TLatex()
      self.problatex.SetNDC()
      self.problatex.SetTextFont(root.gStyle.GetLabelFont())
      self.problatex.SetTextSize(self.pullHist.GetYaxis().GetLabelSize())
      self.problatex.SetTextAlign(12)
      yToDraw = 0.41 #bottom
      yToDraw = 0.92 #top
      #self.problatex.DrawLatex(0.18,yToDraw,"KS Prob: {0:.3g}".format(self.KSProb))
      self.problatex.DrawLatex(0.18,yToDraw,"#chi^{2}/NDF: %.3g" % (self.normchi2))
      self.problatex.DrawLatex(0.18,yToDraw-0.08,"#chi^{2}  Prob: %.3g" % (self.chi2Prob))

    pad2.Update()
    pad2.GetFrame().DrawClone()
    pad2.RedrawAxis() # Updates Axis Lines
  
    canvas.cd()
    self.tlatex.DrawLatex(0.33,0.96,PRELIMINARYSTRING)
    self.tlatex.DrawLatex(0.75,0.96,"#sqrt{s}=%s, L=%.1f fb^{-1}" % (energyStr,lumi))

  def getPullDistributionParams(self,pullList):
    pull = root.RooRealVar("pull","pull",-20,20)
    mean = root.RooRealVar("mean","pull Mean",0.0,-20,20)
    sigma = root.RooRealVar("sigma","pull sigma",1.0,0.01,20)
    self.pullGaus = root.RooGaussian("pullGaus","pullGaus",pull,mean,sigma)
    self.pullDS = root.RooDataSet("pullDS","pullDS",root.RooArgSet(pull))
    for i in pullList:
      pull.setVal(i)
      self.pullDS.add(root.RooArgSet(pull))
    self.pullFR = self.pullGaus.fitTo(self.pullDS,PRINTLEVEL)
    self.pullMean = mean
    self.pullSigma = sigma
    meanStr = "<Pull> = %.2f #pm %.2f" % (mean.getVal(), mean.getError())
    sigmaStr = "#sigma(Pull) = %.2f #pm %.2f" % (sigma.getVal(), sigma.getError())

    frame = pull.frame(root.RooFit.Bins(20))
    self.pullDS.plotOn(frame)
    self.pullGaus.plotOn(frame)
    frame.Draw()
    self.canvas.SaveAs("pullDist"+self.dataHist.GetName()+".png")
    return meanStr, sigmaStr

  def getXNDC(self,x):
    minX = self.pad1.GetX1()
    maxX = self.pad1.GetX2()
    result=(x-minX)/(maxX-minX)
    return result
  def getYNDC(self,y):
    minY = self.pad1.GetY1()
    maxY = self.pad1.GetY2()
    result=(y-minY)/(maxY-minY)
    return result
  def getXUser(self,x):
    minX = self.pad1.GetX1()
    maxX = self.pad1.GetX2()
    result=x*(maxX-minX)+minX
    return result
  def getYUser(self,y):
    minY = self.pad1.GetY1()
    maxY = self.pad1.GetY2()
    result=y*(maxY-minY)+minY
    #print "running getYUser with: %.2f" % y
    #print "  minY: %.2f" % minY
    #print "  maxY: %.2f" % maxY
    #print "  result: %.2f" % result
    return result
  def setYLimitsAuto(self,rangesNDC,yNDCLimits,yMaxCurrent):
    #self.canvas.SaveAs("before_"+str(int(time.time()*100))+".png")
    #print("Running setYLimitsAuto...")
    self.pad1.Update()
    self.canvas.Update()
    getXUser = getattr(self,"getXUser")
    getYUser = getattr(self,"getYUser")
    setYLimitsAuto = getattr(self,"setYLimitsAuto")
    self.pad1.cd()
    ranges = [[getXUser(i[0]),getXUser(i[1])] for i in rangesNDC]
    yLimitsScaleFactor = 1.0
    if self.logy:
      yLimitsScaleFactor = 0.75
    yLimits = [getYUser(i)*yLimitsScaleFactor for i in yNDCLimits]
    maxPoints = []
    xAxis = self.mcSumHist.GetXaxis()
    #print("yMaxCurrent: %.2f " % (yMaxCurrent))
    for r,yLim in zip(ranges,yLimits):
      maxY = 0.0
      for i in range(1,xAxis.GetNbins()+1):
        if xAxis.GetBinUpEdge(i) >= r[0] and xAxis.GetBinLowEdge(i) <= r[1]:
          y = self.mcSumHist.GetBinContent(i)
          yErrTmp = self.mcSumHist.GetBinError(i)
          yErr2Tmp = 0.
          if self.mcVarHist != None:
            yErr2Tmp = self.mcVarHist.GetBinError(i)
          y += max(yErrTmp,yErr2Tmp)
          maxY = max(y,maxY)
      maxPoints += [maxY]
    rescale = 0.0
    if self.logy:
      newMaxPoints = []
      for x in maxPoints:
        if x>0.:
          newMaxPoints += [log10(x)]
        else:
          newMaxPoints += [0.]
      maxPoints = newMaxPoints
    for yLim,maxY in zip(yLimits,maxPoints):
      #print("yLim: %.2f maxY: %.2f" % (yLim, maxY))
      if maxY > yLim:
        rescaleTmp = (maxY/yLim)
        if rescaleTmp > rescale:
          rescale = rescaleTmp
    if rescale == 0.0:
        self.ymax = yMaxCurrent*1.1
        return
    if self.logy:
      rescale = 10**rescale*5.
    #print(rescale)
    newYMax = yMaxCurrent*rescale*1.5
    newYMin = 1e-3
    if self.logy:
      newYMin = 0.1
    self.histForAxis = root.TH2F(self.histForAxis.GetName()+"ForAxis","",1,self.xlimits[0],self.xlimits[1],1,newYMin,newYMax)
    self.histForAxis.Draw("")
    self.mcSumHist.Draw("e1 same")
    #self.canvas.SaveAs("after_"+str(int(time.time()*100))+".png")
    setYLimitsAuto(rangesNDC,yNDCLimits,newYMax)

  def doMCVariations(self,mcVariations):
    self.mcVarHist = None
    if mcVariations==None:
      return
    for key in mcVariations:
      for hist in mcVariations[key]:
        hist.Scale(self.mc2DataSF)
    errorTypes = set()
    for key in mcVariations:
      key = re.sub("Up$","",key)
      key = re.sub("Down$","",key)
      if not key in errorTypes:
        errorTypes.add(key)
    mcSumVariations = {}
    for key in mcVariations:
      if len(mcVariations[key])==0:
        continue
      sumHist = mcVariations[key][0].Clone()
      sumHist.Reset()
      for h in mcVariations[key]:
        sumHist.Add(h)
      mcSumVariations[key] = sumHist
    self.mcVarHist = self.mcSumHist.Clone(self.mcSumHist.GetName()+"_mcVariations")
    for iBin in range(1,self.mcVarHist.GetNbinsX()+1):
      nom = self.mcVarHist.GetBinContent(iBin)
      err2 = self.mcVarHist.GetBinError(iBin)**2
      for eBase in errorTypes:
        errUp = mcSumVariations[eBase+"Up"].GetBinContent(iBin)
        errDown = mcSumVariations[eBase+"Down"].GetBinContent(iBin)
        errUp = abs(nom-errUp)
        errDown = abs(nom-errDown)
        if errUp > errDown:
            err2 += errUp**2
        else:
            err2 += errDown**2
      err = sqrt(err2)
      self.mcVarHist.SetBinError(iBin,err)
    self.mcVarHist.SetFillColor(root.kRed)
    self.mcVarHist.SetFillStyle(3245)
    self.mcVarHist.SetMarkerSize(0)
    self.mcVarHist.SetLineStyle(0)
