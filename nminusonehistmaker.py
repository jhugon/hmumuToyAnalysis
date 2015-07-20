#!/usr/bin/env python

import glob
import re
import sys
import os.path
import ROOT as root
from src.helpers import *

root.gROOT.SetBatch(True)

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
    }
    if isSignal:
      thisResult['mh'] = mh

    results.append(thisResult)
        
  return results

def makeHists(fileDict):
  tfile = root.TFile(fileDict['fn'])
  tree = tfile.Get("treefriend")
  #tree.Print()

  outfile = root.TFile(fileDict['fnOut'],"RECREATE")
  outfile.cd()

  histDicts = [
    {'name':"muLeadPt",'var':"muLead.pt",'hist':"300,0,300",'cuts':'muSubLead.pt>15.'},
    {'name':"muSubLeadPt",'var':"muSubLead.pt",'hist':"300,0,300",'cuts':'muLead.pt>25.'},
  ]

  for ihist, histDict in enumerate(histDicts):

    histTmpName = "hist{0}".format(ihist)

    histName = histDict['name']
    histVariableString = histDict['var']
    histDrawString = " >> {0}({1})".format(histTmpName,histDict['hist'])
    histCutString = histDict['cuts']

    tree.Draw(histVariableString+histDrawString,histCutString)
    hist = root.gDirectory.Get(histTmpName)
    hist.SetName(histName)
    hist.Write()

  outfile.Close()
  sys.exit(0)

if __name__ == "__main__":

  globstr = "*_ptOrdered.root"
  fileDicts = loadFiles(globstr)
  #for fd in fileDicts:
  #  print fd
  for fd in fileDicts:
    makeHists(fd)

