hmumuToyAnalysis
================

H->mumu Ntuple example analyzer code

Data samples are available on the UF analysis cluster in

    /raid/raid8/jhugon/higgsSamples/stage1/

which physically resides on the melrose node.

Even better, you can access it on the /cms filesystem here:

    /cms/data/store/user/jhugon/hmumu/stage1/

The current analyzer.C includes:
*  Pileup reweighting
*  Higgs signal muon resolution smearing to match the MuSclefit data resolution
*  Muon ID, Iso, and selection cuts mirroring the full analysis

It does not include:  
*  Muon efficiency scale factors
*  HLT trigger matching or checks
*  MuScleFit muon corrections

Instructions
============

Log on to melrose.  Create your own scratch directory: /raid/raid8/<username>,
and move into it.

You will need a working copy of ROOT to analyze Ntuples.  The easiest way to
get one is to create a CMSSW release, move into it and setup your environment.

    cmsrel CMSSW_5_3_9
    cd CMSSW_5_3_9
    cmsenv

Now you should be able to type the root command and get a working prompt, if so
you can continue to setup the anlaysis package.

First, clone this git repository:

    git clone https://github.com/jhugon/hmumuToyAnalysis.git

move into hmumuToyAnalysis.  analyzer.C is an example script for analyzing our
Ntuples.  To run it, and load the needed libraries, execute:

    root -b -q -x run.C

It spits out .root files with histograms inside.  If you would like to analyze
a different set of input files, then you can change run.C

To make images from the histograms, just run:

    ./plot.py
    
Currently, plot.py stacks each MC histogram, after rescaling appropriately, and
displays the data on top.

ptOrderedTreeMaker
==================

ptOrderedTreeMaker makes a tree with the reconstructed muons and generator
muons ordered by pt.  It should be used as a friend tree of the normal stage1
trees. It also stores the appropriate weight for the pileup reweighting.
