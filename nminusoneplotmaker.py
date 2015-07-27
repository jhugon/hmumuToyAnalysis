#!/usr/bin/env python

import glob
import re
import sys
import os.path
import ROOT as root
from src.helpers import *

root.gROOT.SetBatch(True)

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
      color = root.kYellow
    elif "DYToTauTau" in fn:
      sampleName = "DYToTauTau"
      color = root.kPink
    elif "DY" in fn:
      sampleName = "DY"
      color = root.kYellow
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
    }
    if isSignal:
      thisResult['mh'] = mh

    results.append(thisResult)
        
  return results

def makeDataMCStack(fileDicts,energy,canvas):

  dataSampleDicts = [x for x in fileDicts if x['isData']]
  signalSampleDicts = [x for x in fileDicts if x['isSignal']]
  backgroundSampleDicts = [x for x in fileDicts if not (x['isSignal'] or x['isData'])]

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

  histDicts = [
    {'name':"muLeadPt",'xtitle':"Leading Muon p_{T} [GeV]"},
    {'name':"muSubLeadPt",'xtitle':"Sub-leading Muon p_{T} [GeV]"},
    {'name':"muLeadIsGlobal",'xtitle':"Leading Muon is Global"},
    {'name':"muSubLeadIsGlobal",'xtitle':"Sub-leading Muon is Global"},
    {'name':"muLeadIsPFMuon",'xtitle':"Leading Muon is PF"},
    {'name':"muSubLeadIsPFMuon",'xtitle':"Sub-leading Muon is PF"},
    {'name':"muLeadEta",'xtitle':"Leading muon |#eta|"},
    {'name':"muSubLeadEta",'xtitle':"Sub-leading muon |#eta|"},
    {'name':"muLeadNumTrackerLayers",'xtitle':"Leading Muon N Tracker Layers"},
    {'name':"muSubLeadNumTrackerLayers",'xtitle':"Sub-leading Muon N Tracker Layers"},
    {'name':"muLeadD0",'xtitle':"Leading Muon D_{0}"},
    {'name':"muSubLeadD0",'xtitle':"Sub-leading Muon D_{0}"},
    {'name':"muLeadDz",'xtitle':"Leading Muon D_{z}"},
    {'name':"muSubLeadDz",'xtitle':"Sub-leading Muon D_{z}"},
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
    if not h:
     raise Exception("Histogram '{0}' not found in file: '{1}'".format(histName,sample['fn']))
    if not sample['isData']:
      print sample 
      print "color: ",sample['color']
      h.SetLineColor(sample['color'])
      if not sample['isSignal']:
        h.SetFillColor(sample['color'])
    else:
      h.SetLineColor(root.kBlack)
      h.SetMarkerColor(root.kBlack)
    return h
    

  for histDict in histDicts:
    signalHists = []
    for sample in signalSamples:
      signalHists.append(getHist(histDict,sample))
    backgroundHists = []
    for sample in backgroundSamples:
      backgroundHists.append(getHist(histDict,sample))
    dataHists = []
    for sample in dataSamples:
      dataHists.append(getHist(histDict,sample))

    # Setup dataHist
    assert(len(backgroundHists)>0)
    dataHist = backgroundHists[0].Clone("dataHist_"+histDict['name']+energy)
    dataHist.Clear()
    for h in dataHists:
      dataHist.Add(h)

    dmcstack = DataMCStack(backgroundHists,dataHist,canvas,histDict['xtitle'],energyStr=energy)
    canvas.SaveAs("{0}_{1}.png".format(histDict['name'],energy))

if __name__ == "__main__":

  canvas = root.TCanvas("c")
  fileDicts = loadFiles()
  energies = set()
  for fd in fileDicts:
    if not fd['energy'] in energies:
      energies.add(fd['energy'])
  for energy in energies:
    files = [x for x in fileDicts if x['energy']==energy]
    makeDataMCStack(fileDicts,energy,canvas)


