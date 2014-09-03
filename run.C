{
  gSystem->cd("src");
  gROOT->ProcessLine(".L FuncSmearingZmumu2011ChargePtCorr2.C+");
  gROOT->ProcessLine(".L FuncSmearingZmumu2012PtCorr2.C+");
  gSystem->cd("../");
  gROOT->ProcessLine(".L analyzer.C++");
  
  unsigned maxEvents = 100000000000;
  //unsigned maxEvents = 10000;
  TString runPeriod("8TeV");
  analyzer("/cms/data/store/user/jhugon/hmumu/stage1/GluGlu_HToMM_M-125.root","ggHmumu.root",runPeriod,false,true,maxEvents);
  analyzer("/cms/data/store/user/jhugon/hmumu/stage1/VBF_HToMM_M-125.root","vbfHmumu.root",runPeriod,false,true,maxEvents);
  analyzer("/cms/data/store/user/jhugon/hmumu/stage1/WH_HToMuMu_M-125.root","wHmumu.root",runPeriod,false,true,maxEvents);
  analyzer("/cms/data/store/user/jhugon/hmumu/stage1/ZH_HToMuMu_M-125.root","zHmumu.root",runPeriod,false,true,maxEvents);

  analyzer("/cms/data/store/user/jhugon/hmumu/stage1/DYJetsToLL_minimal.root","DY.root",runPeriod,false,false,maxEvents);
  analyzer("/cms/data/store/user/jhugon/hmumu/stage1/TTJets_minimal.root","TT.root",runPeriod,false,false,maxEvents);
  analyzer("/cms/data/store/user/jhugon/hmumu/stage1/WW_minimal.root","WW.root",runPeriod,false,false,maxEvents);
  analyzer("/cms/data/store/user/jhugon/hmumu/stage1/WZ_minimal.root","WZ.root",runPeriod,false,false,maxEvents);
  analyzer("/cms/data/store/user/jhugon/hmumu/stage1/ZZ_minimal.root","ZZ.root",runPeriod,false,false,maxEvents);

  analyzer("/cms/data/store/user/jhugon/hmumu/stage1/SingleMuRun2012A-13Jan2013-v1_minimal.root","SingleMuRun2012A.root",runPeriod,true,false,maxEvents);
  analyzer("/cms/data/store/user/jhugon/hmumu/stage1/SingleMuRun2012B-13Jan2013-v1_minimal.root","SingleMuRun2012B.root",runPeriod,true,false,maxEvents);
  analyzer("/cms/data/store/user/jhugon/hmumu/stage1/SingleMuRun2012C-13Jan2013-v1_minimal.root","SingleMuRun2012C.root",runPeriod,true,false,maxEvents);
  analyzer("/cms/data/store/user/jhugon/hmumu/stage1/SingleMuRun2012D-13Jan2013-v1_minimal.root","SingleMuRun2012D.root",runPeriod,true,false,maxEvents);
  
}
