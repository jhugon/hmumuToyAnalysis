
#include <TSystem.h>
#include <TROOT.h>
#include <TClonesArray.h>
#include <TParticle.h>
#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TProfile.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TF1.h>
#include <TRandom3.h>

#include "DataFormats.h"
#include "helpers.h"
#include "LumiReweightingStandAlone.h"

void analyzer ()
{
  using namespace std;

  ///////////////////
  // Configuration

  unsigned maxEvents = 100;
  bool isData = false;

  TString inputFileName("/raid/raid8/jhugon/higgsSamples/stage1/8TeV/GluGlu_HToMM_M-125.root");
  TString runPeriod("8TeV");

  float minMmm = 110;
  float maxMmm = 160;

  //gErrorIgnoreLevel = kError;

  ///////////////////////////
  Double_t MASS_MUON = 0.105658367;    //GeV/c2

  //////////////////////////
  // Tree Branches

  TChain * tree = new TChain("tree");
  tree->Add(inputFileName);

  _MuonInfo reco1, reco2;

  tree->SetBranchAddress("reco1", &reco1);
  tree->SetBranchAddress("reco2", &reco2);

  float recoCandMass, recoCandPt, recoCandY, recoCandPhi;
  float recoCandMassRes, recoCandMassResCov;

  tree->SetBranchAddress("recoCandMass",       &recoCandMass);
  tree->SetBranchAddress("recoCandPt",         &recoCandPt);
  tree->SetBranchAddress("recoCandY",          &recoCandY);
  tree->SetBranchAddress("recoCandPhi",        &recoCandPhi);
  tree->SetBranchAddress("recoCandMassRes",    &recoCandMassRes);
  tree->SetBranchAddress("recoCandMassResCov", &recoCandMassResCov);

  float trueMass=-99999.0;
  if(!isData && tree->GetBranchStatus("trueMass"))
    tree->SetBranchAddress("trueMass", &trueMass);

  /// Higgs Boson 
  _genPartInfo genHpostFSR;
  if(!isData && tree->GetBranchStatus("genHpostFSR"))
    tree->SetBranchAddress("genHpostFSR", &genHpostFSR);

  _TrackInfo reco1GenPostFSR;
  if(!isData && tree->GetBranchStatus("genM1HpostFSR"))
    tree->SetBranchAddress("genM1HpostFSR", &reco1GenPostFSR);

  _TrackInfo reco2GenPostFSR;
  if(!isData && tree->GetBranchStatus("genM2HpostFSR"))
    tree->SetBranchAddress("genM2HpostFSR", &reco2GenPostFSR);

  _PFJetInfo jets;
  tree->SetBranchAddress("pfJets",&jets);

  float puJetFullDisc[10];
  float puJetSimpleDisc[10];
  float puJetCutDisc[10];

  tree->SetBranchAddress("puJetFullDisc",&puJetFullDisc);
  tree->SetBranchAddress("puJetSimpleDisc",&puJetSimpleDisc);
  tree->SetBranchAddress("puJetCutDisc",&puJetCutDisc);

  float puJetFullId[10];
  float puJetSimpleId[10];
  float puJetCutId[10];

  tree->SetBranchAddress("puJetFullId",&puJetFullId);
  tree->SetBranchAddress("puJetSimpleId",&puJetSimpleId);
  tree->SetBranchAddress("puJetCutId",&puJetCutId);

  int nPU=0;
  if (!isData)
    {
      tree->SetBranchAddress("nPU",&nPU);
    }
  _VertexInfo vertexInfo;
  tree->SetBranchAddress("vertexInfo",&vertexInfo);
  _EventInfo eventInfo;
  tree->SetBranchAddress("eventInfo",&eventInfo);
  _MetInfo met;
  tree->SetBranchAddress("met",&met);


  //////////////////////////
  //for PU reweighting

  TRandom3 random(1457);

  reweight::LumiReWeighting lumiWeights("pileupDists/PileUpHistMC2012Summer50nsPoissonOOTPU.root","pileupDists/PileUpHist2012ABCD.root","pileup","pileup");
  if (runPeriod == "7TeV")
  {
    cout << "Using 2011AB PU reweighting\n";
    lumiWeights = reweight::LumiReWeighting("pileupDists/PileUpHistMCFall11.root","pileupDists/PileUpHist2011AB.root","pileup","pileup");
  }
  else
  {
    cout << "Using 2012ABCD PU reweighting\n";
  }

  ///////////////////////////////
  // Which Muon Selection to Use

  bool (*muonIdFuncPtr)(_MuonInfo&);
  if (runPeriod == "7TeV")
    {
      cout << "Using 2011 Tight Muon Selection\n";
      muonIdFuncPtr = &isKinTight_2011_noIso;
    }
  else
    {
      cout << "Using 2012 Tight Muon Selection\n";
      muonIdFuncPtr = &isKinTight_2012_noIso;
    }

  unsigned nEvents = tree->GetEntries();
  unsigned reportEach=1000;
  if (nEvents/1000>reportEach)
    reportEach = nEvents/1000;

  ///////////////////////////////
  ///////////////////////////////
  ///////////////////////////////
  // Event Loop

  for(unsigned i=0; i<nEvents;i++)
  {
    if(i >= maxEvents)
      continue;

    tree->GetEvent(i);
    if (i % reportEach == 0) cout << "Event: " << i << endl;

    // Muon-related cuts

    //if (recoCandMass > maxMmm || recoCandMass < minMmm)
    //    continue;

    bool muon1PassId = (*muonIdFuncPtr)(reco1);
    bool muon2PassId = (*muonIdFuncPtr)(reco2);
    if ( !(muon1PassId && muon2PassId))
        continue;

    bool muon1PassIso = (getPFRelIso(reco1) <= 0.12);
    bool muon2PassIso = (getPFRelIso(reco2) <= 0.12);
    if ( !(muon1PassIso && muon2PassIso))
        continue;

    // PU reweight
    float weight = 1.;
    if (!isData)
    {
      weight *= lumiWeights.weight(nPU);
    }

    cout << "recoCandMass: " << recoCandMass << endl;
    cout << "muon Pt1: " << reco1.pt << endl;
    cout << "muon Pt2: " << reco2.pt << endl;
    cout << "muon eta1: " << reco1.eta << endl;
    cout << "muon eta2: " << reco2.eta << endl;
    cout << "muon iso1: " << getPFRelIso(reco1) << endl;
    cout << "muon iso2: " << getPFRelIso(reco2) << endl;
    cout << "PU weight: " << weight << endl;
    cout << endl;


  }

}
