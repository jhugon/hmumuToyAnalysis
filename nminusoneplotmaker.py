#!/usr/bin/env python

import glob
import re
import sys
import os.path
import ROOT as root
from src.helpers import *

root.gROOT.SetBatch(True)

#################################################3

SIGSF = 10000.

# in fb^-1
lumiDict = {}
lumiDict["8TeV"] = 19.712 #2012ABCD 22Jan2013 Pixel-Lumi
lumiDict["7TeV"] = 5.05 #2011AB
# From https://hypernews.cern.ch/HyperNews/CMS/get/physics-validation/2446.html
lumiDict["13TeV-Golden"] = 0.04024
lumiDict["13TeV-MuonPhys"] = 0.04749

hmm_br = {'125': 2.19e-04 }

# in pb
xsec = {}
xsec["DY_8TeV"] = 3503.71   ## madgraph
xsec["ttbar_8TeV"] = 225.197   ## madgraph

xsec["WW_8TeV"] =   54.838
xsec["WZ_8TeV"] =   33.21 
xsec["ZZ_8TeV"] =  17.654 

xsec["gg_M125_8TeV"] =   19.27  * hmm_br['125'] * SIGSF
xsec["vbf_M125_8TeV"] =    1.578  * hmm_br['125'] * SIGSF
xsec["wh_M125_8TeV"] =     0.7046  * hmm_br['125'] * SIGSF
xsec["zh_M125_8TeV"] =     0.4153  * hmm_br['125'] * SIGSF
xsec["vh_M125_8TeV"] =    xsec["wh_M125_8TeV"]+xsec["zh_M125_8TeV"]
xsec["tth_M125_8TeV"] =       0.1293  * hmm_br['125'] * SIGSF

#LHC Higgs XS WG: European Strat Group
xsec["DY_14TeV"] = 6131.
xsec["ttbar_14TeV"] = 964.6 #LHC Higgs XS WG: European Strat Group

# Extrapolate Backgrounds
#xsec["DY_13TeV"] = 5693.53
xsec["DY_13TeV"] = 9000.00 # just adjusted untill approx correct
xsec["DY_13TeV"] = 11500.00 # just adjusted untill approx correct
xsec["ttbar_13TeV"] = 814.4 #https://indico.ifca.es/indico/event/69/contribution/1/material/slides/1.pdf
# Signal from https://twiki.cern.ch/twiki/bin/view/LHCPhysics/CERNYellowReportPageAt1314TeV
xsec["gg_M125_13TeV"] =  43.92 * hmm_br['125'] * SIGSF
xsec["vbf_M125_13TeV"] =   3.748 * hmm_br['125']  * SIGSF
xsec["wh_M125_13TeV"] =    1.380  * hmm_br['125'] * SIGSF
xsec["zh_M125_13TeV"] =     0.8696  * hmm_br['125'] * SIGSF
xsec["vh_M125_13TeV"] =    xsec["wh_M125_13TeV"]+xsec["zh_M125_13TeV"]
xsec["tth_M125_13TeV"] =      0.5085  * hmm_br['125'] * SIGSF


#################################################3

def loadFiles(globstr="*_hists.root"):

  # Load files
  inputfileNames = glob.glob(globstr)
  results = []
  for fn in inputfileNames:
    if fn[-5:] != ".root":
      raise Exception("{0} doesn't have .root suffix".format(fn))
    energy = "8TeV"
    if "13TeV" in fn:
      energy = "13TeV"
    elif "7TeV" in fn:
      energy = "7TeV"
    isSignal = False
    isData = False
    mh = "125"
    sampleName = ""
    color = ""
    if "GluGlu" in fn or "gg" in fn:
      isSignal = True
      sampleName = "gg"
      color = root.kRed
    if "VBF" in fn or "vbf" in fn:
      isSignal = True
      sampleName = "vbf"
      color = root.kBlue
    if "WH" in fn or "wh" in fn or "wH" in fn:
      isSignal = True
      sampleName = "wh"
      color = root.kGreen
    if "ZH" in fn or "zh" in fn or "zH" in fn:
      isSignal = True
      sampleName = "zh"
      color = root.kGreen
    if "VH" in fn or "vh" in fn or "vH" in fn:
      isSignal = True
      sampleName = "vh"
      color = root.kGreen
    if "DYToMuMu" in fn:
      sampleName = "DYToMuMu"
      color = root.kOrange
    elif "DYToTauTau" in fn:
      sampleName = "DYToTauTau"
      color = root.kPink
    elif "DY" in fn:
      sampleName = "DY"
      color = root.kOrange
    if "QCD" in fn:
      sampleName = "QCD"
    if "TT" in fn or "ttbar" in fn:
      sampleName = "ttbar"
      color = root.kGreen-1
    if "WJets" in fn:
      sampleName = "WJets"
      color = root.kGreen
    if "WW" in fn:
      sampleName = "WW"
      color = root.kPink+9
    if "WZ" in fn:
      sampleName = "WZ"
      color = root.kPink+9
    if "ZZ" in fn:
      sampleName = "ZZ"
      color = root.kPink+9

    match = re.search(r"[a-zA-Z0-9]+Run[a-zA-Z0-9]+",fn)
    if match:
      sampleName = match.group(0)
      isData = True

    ## Cross Section
    xs = -1.
    if not isData:
      matchStr = r".+_hists.root"
      match = re.search(r"(.+)_hists.root",fn)
      if match:
        key = match.group(1)
        if xsec.has_key(key):
          xs = xsec[key]
        else:
          raise Exception("Datasample '{0}' not found in xsec dict.".format(key))
      else:
        raise Exception("Could not match filename '{0}' to '{1}'".format(fn,matchStr))

    fnOut = sampleName

    if isSignal:
      match = re.match(r"[-a-zA-Z_/]+([0-9.p]+)\.root",os.path.basename(fn))
      #match = re.search(r"([0-9.]+)\.root",fn)
      if match:
        mh = match.group(1)
      fnOut += "_M" + mh

    fnOut += "_" + energy
    fnOut += "_hists.root"

    thisResult = {
      'fn':fn,
      'fnOut':fnOut,
      'energy':energy,
      'sampleName':sampleName,
      'isSignal':isSignal,
      'isData':isData,
      'color':color,
      'xs': xs,
    }
    if isSignal:
      thisResult['mh'] = mh

    results.append(thisResult)
        
  return results

def makeDataMCStack(fileDicts,energy,canvas):

  energyWithStuff = energy
  energy = re.sub(r"-.*","",energy)

  lumi = lumiDict[energyWithStuff]

  #print "********** ",energyWithStuff,"****************"
  #print fileDicts
  #print "********** ******** ****************"

  dataSampleDicts = [x for x in fileDicts if x['isData']]
  signalSampleDicts = [x for x in fileDicts if x['isSignal']]
  backgroundSampleDicts = [x for x in fileDicts if not (x['isSignal'] or x['isData'])]

  orderDef = [
      "WW",
      "WZ",
      "ZZ",
      "DYToTauTau",
      "WJets",
      "QCD",
      "ttbar",
      "DYToMuMu",
      "DY",
  ]

  backgroundSampleDicts = sorted(backgroundSampleDicts,key=lambda x:orderDef.index(x['sampleName']))

  dataSamples = []
  for d in dataSampleDicts:
    f = root.TFile(d['fn'])
    d['rootfile'] = f
    dataSamples.append(d)
  signalSamples = []
  for d in signalSampleDicts:
    f = root.TFile(d['fn'])
    d['rootfile'] = f
    signalSamples.append(d)
  backgroundSamples = []
  for d in backgroundSampleDicts:
    f = root.TFile(d['fn'])
    d['rootfile'] = f
    backgroundSamples.append(d)

  print "********** ",energyWithStuff,"****************"
  print dataSamples
  print signalSamples
  print backgroundSamples
  print "********** ******** ****************"

  histDicts = [
    {'name':"muLeadPt",'xtitle':"Leading Muon p_{T} [GeV]",'xlimits':[0.,150.]},
    {'name':"muSubLeadPt",'xtitle':"Sub-leading Muon p_{T} [GeV]",'xlimits':[0.,100.]},
    {'name':"muLeadIsGlobal",'xtitle':"Leading Muon is Global"},
    {'name':"muSubLeadIsGlobal",'xtitle':"Sub-leading Muon is Global"},
    {'name':"muLeadIsPFMuon",'xtitle':"Leading Muon is PF"},
    {'name':"muSubLeadIsPFMuon",'xtitle':"Sub-leading Muon is PF"},
    {'name':"muLeadEta",'xtitle':"Leading muon |#eta|"},
    {'name':"muSubLeadEta",'xtitle':"Sub-leading muon |#eta|"},
    {'name':"muLeadNumTrackerLayers",'xtitle':"Leading Muon N Tracker Layers"},
    {'name':"muSubLeadNumTrackerLayers",'xtitle':"Sub-leading Muon N Tracker Layers"},
    {'name':"muLeadD0",'xtitle':"Leading Muon D_{0}",'xlimits':[-0.05,0.05]},
    {'name':"muSubLeadD0",'xtitle':"Sub-leading Muon D_{0}",'xlimits':[-0.05,0.05]},
    {'name':"muLeadDz",'xtitle':"Leading Muon D_{z}",'xlimits':[-0.05,0.05]},
    {'name':"muSubLeadDz",'xtitle':"Sub-leading Muon D_{z}",'xlimits':[-0.05,0.05]},
    {'name':"muLeadNumValidMuonHits",'xtitle':"Leading Muon N Valid Muon Hits"},
    {'name':"muSubLeadNumValidMuonHits",'xtitle':"Sub-leading Muon N Valid Muon Hits"},
    {'name':"muLeadNumValidPixelHits",'xtitle':"Leading Muon N Valid Pixel Hits"},
    {'name':"muSubLeadNumValidPixelHits",'xtitle':"Sub-leading Muon N Valid Pixel Hits"},
    {'name':"muLeadNumOfMatchedStations",'xtitle':"Leading Muon N Matched Station Hits"},
    {'name':"muSubLeadNumOfMatchedStations",'xtitle':"Sub-leading Muon N Matched Station Hits"},
    {'name':"muLeadNormChiSquare",'xtitle':"Leading Muon #chi^{2}/NDF"},
    {'name':"muSubLeadNormChiSquare",'xtitle':"Sub-leading Muon #chi^{2}/NDF"},
    {'name':"muLeadIsHltMatched",'xtitle':"Leading Muon is HLT Matched"},
    {'name':"muSubLeadIsHltMatched",'xtitle':"Sub-leading Muon is HLT Matched"},
  ]

  def getHist(histDict,sample):
    histName = histDict['name']
    h = sample['rootfile'].Get(histName)
    h.UseCurrentStyle()
    if not h:
     raise Exception("Histogram '{0}' not found in file: '{1}'".format(histName,sample['fn']))
    if sample['isData']:
      #print "Justin isData: ",sample
      h.SetFillColor(root.kBlack)
      h.SetLineColor(root.kBlack)
      h.SetMarkerColor(root.kBlack)
    else:
      #print "Justin isNotData: ",sample
      #print sample 
      #print "color: ",sample['color']
      h.SetLineColor(sample['color'])
      if not sample['isSignal']:
        h.SetFillColor(sample['color'])
      sf = sample['xs']*lumiDict[energyWithStuff]*1000.
      h.Scale(sf)
    if histDict.has_key('rebin'):
      h.Rebin(histDict['rebin'])
    return h
    

  for histDict in histDicts:
    outfn = "{0}_{1}.png".format(histDict['name'],energyWithStuff)
    signalHists = []
    for sample in signalSamples:
      tmpHist = getHist(histDict,sample)
      signalHists.append(tmpHist)
      #tmpHist.Draw()
      #canvas.SaveAs("test/"+sample['sampleName']+"_"+outfn)
    backgroundHists = []
    for sample in backgroundSamples:
      tmpHist = getHist(histDict,sample)
      backgroundHists.append(tmpHist)
      #tmpHist.Draw()
      #canvas.SaveAs("test/"+sample['sampleName']+"_"+outfn)
    dataHists = []
    for sample in dataSamples:
      dataHists.append(getHist(histDict,sample))

    # Setup dataHist
    assert(len(backgroundHists)>0)
    dataHist = backgroundHists[0].Clone("dataHist_"+histDict['name']+energyWithStuff)
    dataHist.Reset()
    dataHist.SetLineColor(root.kBlack)
    dataHist.SetMarkerColor(root.kBlack)
    for h in dataHists:
      dataHist.Add(h)

    #dataHist.Draw()
    #canvas.SaveAs("test/data_"+outfn)

    xlimits = []
    if histDict.has_key('xlimits'):
      xlimits = histDict['xlimits']
    ylimits = []
    if histDict.has_key('ylimits'):
      ylimits = histDict['ylimits']
    logy = False
    if histDict.has_key('logy'):
      logy = histDict['logy']

    canvas.Clear()
    dmcstack = DataMCStack(backgroundHists,dataHist,canvas,histDict['xtitle'],energyStr=energy,xlimits=xlimits,logy=logy,ylimits=ylimits,lumi=lumi)
    canvas.SaveAs(outfn)

if __name__ == "__main__":

  canvas = root.TCanvas("c")
  fileDicts = loadFiles()
  #for fd in fileDicts:
  #  print fd["sampleName"],fd['fn']
  energies = set()
  for fd in fileDicts:
    if not fd['energy'] in energies:
      energies.add(fd['energy'])
  for energy in energies:
    if energy == "13TeV":
      for validation in ["Golden","MuonPhys"]:
        files = [x for x in fileDicts if x['energy']==energy]
        for i in reversed(range(len(files))):
          fn = files[i]['fn']
          if "Run2015" in fn and not validation in fn:
            files.pop(i)
        makeDataMCStack(files,energy+"-"+validation,canvas)
    else:
      files = [x for x in fileDicts if x['energy']==energy]
      makeDataMCStack(files,energy,canvas)


