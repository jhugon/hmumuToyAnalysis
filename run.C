{
  gSystem->cd("src");
  gROOT->ProcessLine(".L FuncSmearingZmumu2011ChargePtCorr2.C+");
  gROOT->ProcessLine(".L FuncSmearingZmumu2012PtCorr2.C+");
  gSystem->cd("../");
  gROOT->ProcessLine(".L analyzer.C++");
  
  analyzer();
  
}
