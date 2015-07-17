{
  gROOT->ProcessLine(".L ptOrderedTreeMaker.C++");
  
  unsigned maxEvents = 1000;
  TString runPeriod("8TeV");
  ptOrderedTreeMaker("/raid/raid9/jhugon/hmmNtuples/stage1/8TeV/GluGlu_HToMM_M-125.root","ggHmumu_8TeV_ptOrdered.root",runPeriod,false,true,maxEvents);
  //ptOrderedTreeMaker("/raid/raid9/jhugon/hmmNtuples/stage1/8TeV/VBF_HToMM_M-125.root","vbfHmumu_8TeV_ptOrdered.root",runPeriod,false,true,maxEvents);
  //ptOrderedTreeMaker("/raid/raid9/jhugon/hmmNtuples/stage1/8TeV/WH_HToMuMu_M-125.root","wHmumu_8TeV_ptOrdered.root",runPeriod,false,true,maxEvents);
  //ptOrderedTreeMaker("/raid/raid9/jhugon/hmmNtuples/stage1/8TeV/ZH_HToMuMu_M-125.root","zHmumu_8TeV_ptOrdered.root",runPeriod,false,true,maxEvents);

  //ptOrderedTreeMaker("/raid/raid9/jhugon/hmmNtuples/stage1/8TeV/DYJetsToLL_minimal.root","DY_8TeV_ptOrdered.root",runPeriod,false,false,maxEvents);
  //ptOrderedTreeMaker("/raid/raid9/jhugon/hmmNtuples/stage1/8TeV/TTJets_minimal.root","TT_8TeV_ptOrdered.root",runPeriod,false,false,maxEvents);
  //ptOrderedTreeMaker("/raid/raid9/jhugon/hmmNtuples/stage1/8TeV/WW_minimal.root","WW_8TeV_ptOrdered.root",runPeriod,false,false,maxEvents);
  //ptOrderedTreeMaker("/raid/raid9/jhugon/hmmNtuples/stage1/8TeV/WZ_minimal.root","WZ_8TeV_ptOrdered.root",runPeriod,false,false,maxEvents);
  //ptOrderedTreeMaker("/raid/raid9/jhugon/hmmNtuples/stage1/8TeV/ZZ_minimal.root","ZZ_8TeV_ptOrdered.root",runPeriod,false,false,maxEvents);

  //ptOrderedTreeMaker("/raid/raid9/jhugon/hmmNtuples/stage1/8TeV/SingleMuRun2012A-13Jan2013-v1_minimal.root","SingleMuRun2012A_ptOrdered.root",runPeriod,true,false,maxEvents);
  //ptOrderedTreeMaker("/raid/raid9/jhugon/hmmNtuples/stage1/8TeV/SingleMuRun2012B-13Jan2013-v1_minimal.root","SingleMuRun2012B_ptOrdered.root",runPeriod,true,false,maxEvents);
  //ptOrderedTreeMaker("/raid/raid9/jhugon/hmmNtuples/stage1/8TeV/SingleMuRun2012C-13Jan2013-v1_minimal.root","SingleMuRun2012C_ptOrdered.root",runPeriod,true,false,maxEvents);
  //ptOrderedTreeMaker("/raid/raid9/jhugon/hmmNtuples/stage1/8TeV/SingleMuRun2012D-13Jan2013-v1_minimal.root","SingleMuRun2012D_ptOrdered.root",runPeriod,true,false,maxEvents);

  //gSystem->Exec("hadd -f data_8TeV_ptOrdered.root SingleMuRun2012A_ptOrdered.root SingleMuRun2012B_ptOrdered.root SingleMuRun2012C_ptOrdered.root SingleMuRun2012D_ptOrdered.root");
  //gSystem->Exec("rm SingleMuRun2012A_ptOrdered.root SingleMuRun2012B_ptOrdered.root SingleMuRun2012C_ptOrdered.root SingleMuRun2012D_ptOrdered.root");
  
}
