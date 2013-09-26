
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

#include <vector>

#include "src/DataFormats.h"
#include "src/helpers.h"
#include "src/LumiReweightingStandAlone.h"
#include "src/SmearingTool.h"
#include "src/SmearingTool2011.h"

void analyzer (TString inputFileName,TString outputFileName, TString runPeriod, bool isData, bool isSignal, unsigned maxEvents)
{
  using namespace std;

  ///////////////////
  // Configuration

  float minMmm = 110;
  float maxMmm = 160;

  //gErrorIgnoreLevel = kError;
  const unsigned ISMEAR = 2;

  ///////////////////////////
  // Output Histograms

  setStyle();

  TH1F* dimuonMassHist = new TH1F("dimuonMass","",50,110,160);
  setHistTitles(dimuonMassHist,"M(#mu#mu) [GeV/c^{2}]","Events");
  dimuonMassHist->Sumw2();
  TH1F* nJetsHist = new TH1F("nJets","",10,0,10);
  setHistTitles(nJetsHist,"N_{Jets}","Events");
  nJetsHist->Sumw2();

  ///////////////////////////
  Double_t MASS_MUON = 0.105658367;    //GeV/c2

  //////////////////////////
  // Tree Branches
  cout << "Analyzing filename: "<< inputFileName.Data() << endl;

  TChain * tree = new TChain("tree");
  tree->Add(inputFileName);


  // These are the names of the muons (See src/DataFormats.h for definitions!)
  _MuonInfo reco1, reco2;

  tree->SetBranchAddress("reco1", &reco1);
  tree->SetBranchAddress("reco2", &reco2);

  // These are the dimuon mass, pt, rapidity, and phi
  float recoCandMass, recoCandPt, recoCandY, recoCandPhi;
  float recoCandMassRes, recoCandMassResCov;

  tree->SetBranchAddress("recoCandMass",       &recoCandMass);
  tree->SetBranchAddress("recoCandPt",         &recoCandPt);
  tree->SetBranchAddress("recoCandY",          &recoCandY);
  tree->SetBranchAddress("recoCandPhi",        &recoCandPhi);
  tree->SetBranchAddress("recoCandMassRes",    &recoCandMassRes);
  tree->SetBranchAddress("recoCandMassResCov", &recoCandMassResCov);

  // MC truth info
  float trueMass=-99999.0;
  if(!isData && tree->GetBranchStatus("trueMass"))
    tree->SetBranchAddress("trueMass", &trueMass);

  /// Higgs Boson MC truth info (after FSR)
  _genPartInfo genHpostFSR;
  if(!isData && tree->GetBranchStatus("genHpostFSR"))
    tree->SetBranchAddress("genHpostFSR", &genHpostFSR);

  _TrackInfo reco1GenPostFSR;
  if(!isData && tree->GetBranchStatus("genM1HpostFSR"))
    tree->SetBranchAddress("genM1HpostFSR", &reco1GenPostFSR);

  _TrackInfo reco2GenPostFSR;
  if(!isData && tree->GetBranchStatus("genM2HpostFSR"))
    tree->SetBranchAddress("genM2HpostFSR", &reco2GenPostFSR);

  /// the jet collection
  // these 'rawJets' already have Loose Jet ID applied, and JES corrections
  // and are cross-cleaned of tight muons
  // later, jets will have JER corrections, PUID, and basic cuts applied
  _PFJetInfo rawJets;
  tree->SetBranchAddress("pfJets",&rawJets);

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

  // Be careful, the met has not been well validated
  _MetInfo met;
  tree->SetBranchAddress("met",&met);

  //////////////////////////
  //for PU reweighting

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

  /////////////////////////
  // Smearing
  SmearingTool *smearPT = new SmearingTool();
  SmearingTool2011 *smearPT2011 = new SmearingTool2011();

  /////////////////////////////
  /////////////////////////////

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

    // Reject events with invalid muons
    if (reco1.pt < 0. || reco2.pt < 0.)
        continue;

    /////////////////////////////////////////////////
    // Muon Resolution Smearing to match MuScleFit

    if(isSignal) // smear only signal because it has muons from higgs 
    {
      if(reco1GenPostFSR.pt<0.)
        cout << "Muon 1 Post FSR not valid!\n";
      if(reco2GenPostFSR.pt<0.)
        cout << "Muon 2 Post FSR not valid!\n";
      float ptReco1 = -1.;
      float ptReco2 = -1.;
      if(runPeriod == "7TeV")
      {
        ptReco1 = smearPT2011 -> PTsmear(reco1GenPostFSR.pt, reco1GenPostFSR.eta, reco1GenPostFSR.charge, reco1.pt, ISMEAR);
        ptReco2 = smearPT2011 -> PTsmear(reco2GenPostFSR.pt, reco2GenPostFSR.eta, reco2GenPostFSR.charge, reco2.pt, ISMEAR);
      }
      else
      {
        ptReco1 = smearPT -> PTsmear(reco1GenPostFSR.pt, reco1GenPostFSR.eta, reco1GenPostFSR.charge, reco1.pt, ISMEAR);
        ptReco2 = smearPT -> PTsmear(reco2GenPostFSR.pt, reco2GenPostFSR.eta, reco2GenPostFSR.charge, reco2.pt, ISMEAR);
      }
      TLorentzVector reco1Vec;
      TLorentzVector reco2Vec;
      reco1Vec.SetPtEtaPhiM(ptReco1,reco1.eta,reco1.phi,MASS_MUON);
      reco2Vec.SetPtEtaPhiM(ptReco2,reco2.eta,reco2.phi,MASS_MUON);
      TLorentzVector diMuonVec = reco1Vec + reco2Vec;

      reco1.pt = ptReco1;
      reco2.pt = ptReco2;
      recoCandMass = diMuonVec.M();
      recoCandPt = diMuonVec.Pt();
      recoCandY = diMuonVec.Rapidity();
      recoCandPhi = diMuonVec.Phi();
    
      reco1Vec.SetPtEtaPhiM(ptReco1,reco1.eta,reco1.phi,MASS_MUON);
      reco2Vec.SetPtEtaPhiM(ptReco2,reco2.eta,reco2.phi,MASS_MUON);
      diMuonVec = reco1Vec + reco2Vec;
    }

    //////////////////////////////////////////
    // Muon-related cuts

    if (recoCandMass > maxMmm || recoCandMass < minMmm)
        continue;

    bool muon1PassId = (*muonIdFuncPtr)(reco1);
    bool muon2PassId = (*muonIdFuncPtr)(reco2);
    if ( !(muon1PassId && muon2PassId))
        continue;

    bool muon1PassIso = (getPFRelIso(reco1) <= 0.12);
    bool muon2PassIso = (getPFRelIso(reco2) <= 0.12);
    if ( !(muon1PassIso && muon2PassIso))
        continue;

    // Order muons by pt
    if (reco1.pt < reco2.pt)
    {
      _MuonInfo tmpMuon = reco1;
      reco1 = reco2;
      reco1 = tmpMuon;
    }

    // PU reweight
    float weight = 1.;
    if (!isData)
    {
      weight *= lumiWeights.weight(nPU);
    }

    // Jet Part
    // Do basic selection on jets and JER corrections
    std::vector<TLorentzVector> jets;
    const float jetPtCut = 30.;
    const float jetAbsEtaCut = 4.7;
    const int jetPUIDCut = 4; // >=    tight = 7, medium = 6, loose = 4. Only loose is useful!!
    for(unsigned iJet=0; (iJet < unsigned(rawJets.nJets) && iJet < 10);iJet++)
    {
      // apply jet energy resolution corrections
      if (rawJets.genPt[iJet]>0.0 && rawJets.pt[iJet]>15.)
        rawJets.pt[iJet] = jerCorr(rawJets.pt[iJet],rawJets.genPt[iJet],rawJets.eta[iJet]); 
      bool goodPt = rawJets.pt[iJet]>jetPtCut;
      bool goodEta = fabs(rawJets.eta[iJet])<jetAbsEtaCut;
      bool goodPUID = puJetFullId[iJet] >= jetPUIDCut;
      if (goodPt && goodEta && goodPUID)
      {
        TLorentzVector tmpJetVec;
        tmpJetVec.SetPtEtaPhiM(rawJets.pt[iJet],rawJets.eta[iJet],rawJets.phi[iJet],rawJets.mass[iJet]);
        jets.push_back(tmpJetVec);
      }
    }

//    /////////////////////////////////////////////
//
//    cout << "Event: "<< eventInfo.run << ":" << eventInfo.event << endl;
//
//    // print muon-related info
//    cout << "recoCandMass: " << recoCandMass << endl;
//    cout << "muon Pt1: " << reco1.pt << endl;
//    cout << "muon Pt2: " << reco2.pt << endl;
//    cout << "muon eta1: " << reco1.eta << endl;
//    cout << "muon eta2: " << reco2.eta << endl;
//    cout << "muon iso1: " << getPFRelIso(reco1) << endl;
//    cout << "muon iso2: " << getPFRelIso(reco2) << endl;
//    cout << "PU weight: " << weight << endl;
//    cout << endl;
//
//    // print jet-related info
//    cout << "nJets: " << jets.size() << endl;
//    for (unsigned iJet=0; iJet < jets.size(); iJet++)
//    {
//      cout << "Jet "<<(iJet+1)<<": pt="<< jets[iJet].Pt() << " eta="<<jets[iJet].Eta()<<endl;
//    }
//    cout << endl;
//
//    /////////////////////////////////////////////
      // Fill Histograms

      dimuonMassHist->Fill(recoCandMass,weight);
      nJetsHist->Fill(jets.size(),weight);

  }

  TFile* outFile = new TFile(outputFileName,"RECREATE");
  outFile->cd();
  dimuonMassHist->Write();
  nJetsHist->Write();

}
