#!/usr/bin/env python

import glob
import re
import sys
import os.path
import ROOT as root
from src.helpers import *

root.gROOT.SetBatch(True)

#################################################3

nEventsMap = {}
nEventsMap["ggHmumu_8TeV"] = 49994
nEventsMap["vbfHmumu_8TeV"] = 49894

nEventsMap["zHmumu_8TeV"] = 10000
nEventsMap["wHmumu_8TeV"] = 10000
nEventsMap["DY_8TeV"] = 30086987
nEventsMap["TT_8TeV"] = 6921652 
nEventsMap["WW_8TeV"] =  5218045
nEventsMap["WZ_8TeV"] = 10000283
nEventsMap["ZZ_8TeV"] = 10320000
nEventsMap["QCD_8TeV"] =  20764602

#################################################3

def loadFiles(globstr):

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
    if "GluGlu" in fn or "gg" in fn:
      isSignal = True
      sampleName = "gg"
    if "VBF" in fn or "vbf" in fn:
      isSignal = True
      sampleName = "vbf"
    if "WH" in fn or "wh" in fn or "wH" in fn:
      isSignal = True
      sampleName = "wh"
    if "ZH" in fn or "zh" in fn or "zH" in fn:
      isSignal = True
      sampleName = "zh"
    if "VH" in fn or "vh" in fn or "vH" in fn:
      isSignal = True
      sampleName = "vh"
    if "DYToMuMu" in fn:
      sampleName = "DYToMuMu"
    elif "DYToTauTau" in fn:
      sampleName = "DYToTauTau"
    elif "DY" in fn:
      sampleName = "DY"
    if "QCD" in fn:
      sampleName = "QCD"
    if "TT" in fn or "ttbar" in fn:
      sampleName = "ttbar"
    if "WJets" in fn:
      sampleName = "WJets"
    if "WW" in fn:
      sampleName = "WW"
    if "WZ" in fn:
      sampleName = "WZ"
    if "ZZ" in fn:
      sampleName = "ZZ"

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
    }
    if isSignal:
      thisResult['mh'] = mh

    results.append(thisResult)
        
  return results

def nminusoneCutMaker(variableStr,cutStr):
  result = ""
  cutList = cutStr.split("&&")
  for i, cut in enumerate(cutList):
    if variableStr in cut:
      cutList.pop(i)
  result = " && ".join(cutList)
  return result

def makeHists(fileDict):
  print "Running on {0}".format(fileDict['fn'])
  tfile = root.TFile(fileDict['fn'])

  # get ngenerated events
  originalNumEvents = 0
  if not fileDict['isData']:
    metadata = tfile.Get("metadata")
    if metadata:
      for iEntry in range(metadata.GetEntries()):
        metadata.GetEntry(iEntry)
        originalNumEvents += metadata.originalNumEvents
  for key in nEventsMap:
    if key in fileDict['fn']:
      originalNumEvents = nEventsMap[key]

  scaleFactor = 1.
  if originalNumEvents > 0:
    scaleFactor /= float(originalNumEvents)
  print "Original NumEvents: {0} Scale factor: {1}".format(originalNumEvents,scaleFactor)

  tree = tfile.Get("treefriend")
  tree.AddBranchToCache("*")
  
  #tree.Print()

  outfile = root.TFile(fileDict['fnOut'],"RECREATE")
  outfile.cd()

  genericMuonCuts = "muon.isGlobal && muon.isPFMuon && muon.pt > 15 && abs(muon.eta) < 2.1 & muon.numTrackerLayers >= 6 && abs(muon.d0_PV) <= 0.2 && abs(muon.dz_PV) <= 0.5 && muon.numValidMuonHits >= 1 && muon.numValidPixelHits >= 1 && muon.numOfMatchedStations >= 2 && muon.normChiSquare <= 10 && (muon.sumChargedHadronPtR04 + max(0.0,muon.sumNeutralHadronEtR04+muon.sumPhotonEtR04 - 0.5*muon.sumPUPtR04))/muon.pt <= 0.12"
  triggerMatchedCut = "(( muLead.isHltMatched[0] && muLead.pt > 25.0 ) || ( muSubLead.isHltMatched[0] && muSubLead.pt > 25.0 ))"
  muonCuts = genericMuonCuts.replace("muon","muLead") + " && " + genericMuonCuts.replace("muon","muSubLead")

  histDicts = [
    {'name':"muLeadPt",'var':"muLead.pt",'hist':"300,0,300"},
    {'name':"muSubLeadPt",'var':"muSubLead.pt",'hist':"300,0,300"},
    {'name':"muLeadIsGlobal",'var':"muLead.isGlobal",'hist':"2,-0.5,1.5"},
    {'name':"muSubLeadIsGlobal",'var':"muSubLead.isGlobal",'hist':"2,-0.5,1.5"},
    {'name':"muLeadIsPFMuon",'var':"muLead.isPFMuon",'hist':"2,-0.5,1.5"},
    {'name':"muSubLeadIsPFMuon",'var':"muSubLead.isPFMuon",'hist':"2,-0.5,1.5"},
    {'name':"muLeadEta",'var':"muLead.eta",'hist':"100,-2.5,2.5"},
    {'name':"muSubLeadEta",'var':"muSubLead.eta",'hist':"100,-2.5,2.5"},
    {'name':"muLeadNumTrackerLayers",'var':"muLead.numTrackerLayers",'hist':"21,-0.5,20.5"},
    {'name':"muSubLeadNumTrackerLayers",'var':"muLead.numTrackerLayers",'hist':"21,-0.5,20.5"},
    {'name':"muLeadD0",'var':"muLead.d0_PV",'hist':"100,-0.2,0.2"},
    {'name':"muSubLeadD0",'var':"muLead.d0_PV",'hist':"100,-0.2,0.2"},
    {'name':"muLeadDz",'var':"muLead.dz_PV",'hist':"100,-0.2,0.2"},
    {'name':"muSubLeadDz",'var':"muLead.dz_PV",'hist':"100,-0.2,0.2"},
    {'name':"muLeadNumValidMuonHits",'var':"muLead.numValidMuonHits",'hist':"21,-0.5,20.5"},
    {'name':"muSubLeadNumValidMuonHits",'var':"muLead.numValidMuonHits",'hist':"21,-0.5,20.5"},
    {'name':"muLeadNumValidPixelHits",'var':"muLead.numValidPixelHits",'hist':"21,-0.5,20.5"},
    {'name':"muSubLeadNumValidPixelHits",'var':"muLead.numValidPixelHits",'hist':"21,-0.5,20.5"},
    {'name':"muLeadNumOfMatchedStations",'var':"muLead.numOfMatchedStations",'hist':"5,-0.5,4.5"},
    {'name':"muSubLeadNumOfMatchedStations",'var':"muLead.numOfMatchedStations",'hist':"5,-0.5,4.5"},
    {'name':"muLeadNormChiSquare",'var':"muLead.normChiSquare",'hist':"250,0,5.0"},
    {'name':"muSubLeadNormChiSquare",'var':"muLead.normChiSquare",'hist':"250,0,5.0"},
    {'name':"muLeadIsHltMatched",'var':"muLead.isHltMatched[0]",'hist':"2,-0.5,1.5"},
    {'name':"muSubLeadIsHltMatched",'var':"muSubLead.isHltMatched[0]",'hist':"2,-0.5,1.5"},
  ]

  nminusoneHistDicts = []
  for ihist, histDict in enumerate(histDicts):
    histDict['cuts'] = nminusoneCutMaker(histDict['var'],muonCuts) +" && "+ triggerMatchedCut
    nminusoneHistDicts.append(histDict)

  nminusoneHistDicts.extend([
    {'name':"muLeadPFRelIso",'var':"(muLead.sumChargedHadronPtR04 + max(0.0,muLead.sumNeutralHadronEtR04+muLead.sumPhotonEtR04 - 0.5*muLead.sumPUPtR04))/muLead.pt",'hist':"200,0,2",'cuts':nminusoneCutMaker("muLead.sumNeutralHadronEtR04",muonCuts)+" && "+ triggerMatchedCut},
    {'name':"muSubLeadPFRelIso",'var':"(muSubLead.sumChargedHadronPtR04 + max(0.0,muSubLead.sumNeutralHadronEtR04+muSubLead.sumPhotonEtR04 - 0.5*muSubLead.sumPUPtR04))/muSubLead.pt",'hist':"200,0,2",'cuts':nminusoneCutMaker("muSubLead.sumNeutralHadronEtR04",muonCuts)+" && "+ triggerMatchedCut},
    {'name':"muLeadIsHltMatched",'var':"muLead.isHltMatched[0]",'hist':"2,-0.5,1.5",'cuts':muonCuts + "&& muSubLead.isHltMatched[0] && muSubLead.pt>25.0"},
    {'name':"muSubLeadIsHltMatched",'var':"muSubLead.isHltMatched[0]",'hist':"2,-0.5,1.5",'cuts':muonCuts + "&& muLead.isHltMatched[0] && muLead.pt>25.0"},
  ])

  for ihist, histDict in enumerate(nminusoneHistDicts):
    histTmpName = "hist{0}".format(ihist)

    histName = histDict['name']
    print "  Making Histogram of '{0}'".format(histName)
    histVariableString = histDict['var']
    histDrawString = " >> {0}({1})".format(histTmpName,histDict['hist'])
    histCutString = histDict['cuts']

    tree.Draw(histVariableString+histDrawString,histCutString)
    hist = root.gDirectory.Get(histTmpName)
    hist.SetName(histName)
    hist.SetTitle("")
    hist.Sumw2()
    hist.Scale(scaleFactor)
    hist.Write()

  outfile.Close()

if __name__ == "__main__":


  globstr = "*_ptOrdered.root"
  fileDicts = loadFiles(globstr)
  #for fd in fileDicts:
  #  print fd
  for fd in fileDicts:
    makeHists(fd)

