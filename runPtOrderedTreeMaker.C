{
  gROOT->ProcessLine(".L ptOrderedTreeMaker.C++");
  
  unsigned maxEvents = 100;
  TString runPeriod("8TeV");
  analyzer("/raid/raid8/jhugon/higgsSamples/stage1/8TeV/GluGlu_HToMM_M-125.root","ggHmumu_8TeV.root",runPeriod,false,true,maxEvents);
  analyzer("/raid/raid8/jhugon/higgsSamples/stage1/8TeV/VBF_HToMM_M-125.root","vbfHmumu_8TeV.root",runPeriod,false,true,maxEvents);
  analyzer("/raid/raid8/jhugon/higgsSamples/stage1/8TeV/WH_HToMuMu_M-125.root","wHmumu_8TeV.root",runPeriod,false,true,maxEvents);
  analyzer("/raid/raid8/jhugon/higgsSamples/stage1/8TeV/ZH_HToMuMu_M-125.root","zHmumu_8TeV.root",runPeriod,false,true,maxEvents);

  analyzer("/raid/raid8/jhugon/higgsSamples/stage1/8TeV/DYJetsToLL_minimal.root","DY_8TeV.root",runPeriod,false,false,maxEvents);
  analyzer("/raid/raid8/jhugon/higgsSamples/stage1/8TeV/TTJets_minimal.root","TT_8TeV.root",runPeriod,false,false,maxEvents);
  analyzer("/raid/raid8/jhugon/higgsSamples/stage1/8TeV/WW_minimal.root","WW_8TeV.root",runPeriod,false,false,maxEvents);
  analyzer("/raid/raid8/jhugon/higgsSamples/stage1/8TeV/WZ_minimal.root","WZ_8TeV.root",runPeriod,false,false,maxEvents);
  analyzer("/raid/raid8/jhugon/higgsSamples/stage1/8TeV/ZZ_minimal.root","ZZ_8TeV.root",runPeriod,false,false,maxEvents);

  analyzer("/raid/raid8/jhugon/higgsSamples/stage1/8TeV/SingleMuRun2012A-13Jan2013-v1_minimal.root","SingleMuRun2012A.root",runPeriod,true,false,maxEvents);
  analyzer("/raid/raid8/jhugon/higgsSamples/stage1/8TeV/SingleMuRun2012B-13Jan2013-v1_minimal.root","SingleMuRun2012B.root",runPeriod,true,false,maxEvents);
  analyzer("/raid/raid8/jhugon/higgsSamples/stage1/8TeV/SingleMuRun2012C-13Jan2013-v1_minimal.root","SingleMuRun2012C.root",runPeriod,true,false,maxEvents);
  analyzer("/raid/raid8/jhugon/higgsSamples/stage1/8TeV/SingleMuRun2012D-13Jan2013-v1_minimal.root","SingleMuRun2012D.root",runPeriod,true,false,maxEvents);

  gSystem->Exec("hadd data_8TeV.root SingleMuRun2012A.root SingleMuRun2012B.root SingleMuRun2012C.root SingleMuRun2012D.root");
  gSystem->Exec("rm SingleMuRun2012A.root SingleMuRun2012B.root SingleMuRun2012C.root SingleMuRun2012D.root");
  
}
