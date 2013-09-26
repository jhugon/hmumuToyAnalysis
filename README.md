hmumuToyAnalysis
================

H->mumu Ntuple example analyzer code

Data samples are available on the UF analysis cluster in

/raid/raid8/jhugon/higgsSamples/stage1/

which physically resides on the melrose node.

The current analyzer.C includes:
  Pileup reweighting
  Higgs signal muon resolution smearing to match the MuSclefit data resolution
  Muon ID, Iso, and selection cuts mirroring the full analysis
It does not include:  
  Muon efficiency scale factors
  HLT trigger matching or checks
  MuScleFit muon corrections

Instructions
============

Log on to melrose.  Create your own scratch directory: /raid/raid8/<username>
Move to that directory, and clone this git repository:

git clone https://github.com/jhugon/hmumuToyAnalysis.git

move into hmumuToyAnalysis.  analyzer.C is an example script for analyzing our Ntuples.  To run it, and load the needed libraries, execute:

root -b -q -x run.C

If you would like to analyze a different set of input files, then you can change run.C
