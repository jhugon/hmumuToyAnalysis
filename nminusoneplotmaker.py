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

def makeDataMCStack(fileDicts):

  dataSamples = [x for x in fileDicts if x['isData']]
  signalSamples = [x for x in fileDicts if x['isSignal']]
  backgroundSamples = [x for x in fileDicts if not (x['isSignal'] or x['isData'])]

if __name__ == "__main__":

  fileDicts = loadFiles()
  energies = set()
  for fd in fileDicts:
    if not fd['energy'] in energies:
      energies.add(fd['energy'])
  for energy in energies:
    files = [x for x in fileDicts if x['energy']==energy]
    makeDataMCStack(fileDicts)


