
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

void ptOrderedTreeMaker (TString inputFileName,TString outputFileName, TString runPeriod, bool isData, bool isSignal, unsigned maxEvents)
{
  using namespace std;

  ///////////////////
  // Configuration

  //gErrorIgnoreLevel = kError;
  const unsigned ISMEAR = 2;

  ///////////////////////////
  Double_t MASS_MUON = 0.105658367;    //GeV/c2

  //////////////////////////
  // Tree Branches
  cout << "Analyzing filename: "<< inputFileName.Data() << endl;
  if (isData)
    cout << "isData\n";
  if (isSignal)
    cout << "isSignal\n";

  TChain * tree = new TChain("tree");
  tree->Add(inputFileName);


  // These are the names of the muons (See src/DataFormats.h for definitions!)
  _MuonInfo reco1, reco2;

  tree->SetBranchAddress("reco1", &reco1);
  tree->SetBranchAddress("reco2", &reco2);

  _TrackInfo genM1HpreFSR;
  if(!isData && tree->GetBranchStatus("genM1HpreFSR"))
    tree->SetBranchAddress("genM1HpreFSR", &genM1HpreFSR);

  _TrackInfo genM2HpreFSR;
  if(!isData && tree->GetBranchStatus("genM2HpreFSR"))
    tree->SetBranchAddress("genM2HpreFSR", &genM2HpreFSR);

  _TrackInfo genM1ZpreFSR;
  if(!isData && tree->GetBranchStatus("genM1ZpreFSR"))
    tree->SetBranchAddress("genM1ZpreFSR", &genM1ZpreFSR);

  _TrackInfo genM2ZpreFSR;
  if(!isData && tree->GetBranchStatus("genM2ZpreFSR"))
    tree->SetBranchAddress("genM2ZpreFSR", &genM2ZpreFSR);

  _TrackInfo genM1HpostFSR;
  if(!isData && tree->GetBranchStatus("genM1HpostFSR"))
    tree->SetBranchAddress("genM1HpostFSR", &genM1HpostFSR);

  _TrackInfo genM2HpostFSR;
  if(!isData && tree->GetBranchStatus("genM2HpostFSR"))
    tree->SetBranchAddress("genM2HpostFSR", &genM2HpostFSR);

  _TrackInfo genM1ZpostFSR;
  if(!isData && tree->GetBranchStatus("genM1ZpostFSR"))
    tree->SetBranchAddress("genM1ZpostFSR", &genM1ZpostFSR);

  _TrackInfo genM2ZpostFSR;
  if(!isData && tree->GetBranchStatus("genM2ZpostFSR"))
    tree->SetBranchAddress("genM2ZpostFSR", &genM2ZpostFSR);

  int nPU=0;
  if (!isData)
    {
      tree->SetBranchAddress("nPU",&nPU);
    }
  _VertexInfo vertexInfo;
  tree->SetBranchAddress("vertexInfo",&vertexInfo);
  _EventInfo eventInfo;
  tree->SetBranchAddress("eventInfo",&eventInfo);

  //////////////////////////
  //for PU reweighting

  reweight::LumiReWeighting* lumiWeights; 
  if (runPeriod == "7TeV")
  {
    cout << "Using 2011AB PU reweighting\n";
    *lumiWeights = reweight::LumiReWeighting("pileupDists/PileUpHistMCFall11.root","pileupDists/PileUpHist2011AB.root","pileup","pileup");
  }
  else if (runPeriod == "8TeV")
  {
    cout << "Using 2012ABCD PU reweighting\n";
    *lumiWeights = reweight::LumiReWeighting("pileupDists/PileUpHistMC2012Summer50nsPoissonOOTPU.root","pileupDists/PileUpHist2012ABCD.root","pileup","pileup");
  }

  ////////////////////////////
  // Output Tree and Branches
  //

  TTree* outtree = (TTree*) tree->Clone("outtree");

  _MuonInfo muLead;
  outtree->Branch("muLead", &muLead
                   "isTracker/I:isStandAlone/I:isGlobal/I:"
                   "charge/I:pt/F:ptErr/F:eta/F:phi/F:"
                   "trkPt/F:trkPtErr/F:trkEta/F:trkPhi/F:"
                   "normChiSquare/F:"
                   "d0_BS/F:dz_BS/F:"
                   "d0_PV/F:dz_PV/F:"
                   "numPixelLayers/I:"
                   "numTrackerLayers/I:"
                   "numStripLayers/I:"
                   "validFracTracker/F:"
                   "numValidMuonHits/I:"
                   "numValidPixelHits/I:"    
                   "numValidTrackerHits/I:"  
                   "numValidStripHits/I:"    
                   "numSegmentMatches/I:"    
                   "numOfMatchedStations/I:"
                   "trackIsoSumPt/F:"
                   "trackIsoSumPtCorr/F:"      
                   "hcalIso/F:"
                   "ecalIso/F:"
                   "relCombIso/F:"
                   "isPFMuon/I:"
                   "pfPt/F:"
                   "pfEta/F:"
                   "pfPhi/F:"
                   "sumChargedHadronPtR03/F:"
                   "sumChargedParticlePtR03/F:"
                   "sumNeutralHadronEtR03/F:"
                   "sumPhotonEtR03/F:"
                   "sumPUPtR03/F:"
                   "sumChargedHadronPtR04/F:"
                   "sumChargedParticlePtR04/F:"
                   "sumNeutralHadronEtR04/F:"
                   "sumPhotonEtR04/F:"
                   "sumPUPtR04/F:"
                   "isHltMatched[3]/I:"
                   "hltPt[3]/F:"
                   "hltEta[3]/F:"
                   "hltPhi[3]/F");

  _MuonInfo muSubLead;
  outtree->Branch("muSubLead", &muSubLead,
                   "isTracker/I:isStandAlone/I:isGlobal/I:"
                   "charge/I:pt/F:ptErr/F:eta/F:phi/F:"
                   "trkPt/F:trkPtErr/F:trkEta/F:trkPhi/F:"
                   "normChiSquare/F:"
                   "d0_BS/F:dz_BS/F:"
                   "d0_PV/F:dz_PV/F:"
                   "numPixelLayers/I:"
                   "numTrackerLayers/I:"
                   "numStripLayers/I:"
                   "validFracTracker/F:"
                   "numValidMuonHits/I:"
                   "numValidPixelHits/I:"    
                   "numValidTrackerHits/I:"  
                   "numValidStripHits/I:"    
                   "numSegmentMatches/I:"    
                   "numOfMatchedStations/I:"
                   "trackIsoSumPt/F:"
                   "trackIsoSumPtCorr/F:"      
                   "hcalIso/F:"
                   "ecalIso/F:"
                   "relCombIso/F:"
                   "isPFMuon/I:"
                   "pfPt/F:"
                   "pfEta/F:"
                   "pfPhi/F:"
                   "sumChargedHadronPtR03/F:"
                   "sumChargedParticlePtR03/F:"
                   "sumNeutralHadronEtR03/F:"
                   "sumPhotonEtR03/F:"
                   "sumPUPtR03/F:"
                   "sumChargedHadronPtR04/F:"
                   "sumChargedParticlePtR04/F:"
                   "sumNeutralHadronEtR04/F:"
                   "sumPhotonEtR04/F:"
                   "sumPUPtR04/F:"
                   "isHltMatched[3]/I:"
                   "hltPt[3]/F:"
                   "hltEta[3]/F:"
                   "hltPhi[3]/F");

  _TrackInfo muLeadHpreFSR;
  TBranch* muLeadHpreFSRBranch;
  if(!isData && tree->GetBranchStatus("genM1HpreFSR"))
    muLeadHpreFSRBranch = outtree->Branch("muLeadHpreFSR",&muLeadHpreFSR,"charge/I:pt/F:ptErr/F:eta/F:phi/F");

  _TrackInfo muSubLeadHpreFSR;
  TBranch* muSubLeadHpreFSRBranch;
  if(!isData && tree->GetBranchStatus("genM2HpreFSR"))
    muSubLeadHpreFSRBranch = outtree->Branch("muSubLeadHpreFSR",&muSubLeadHpreFSR,"charge/I:pt/F:ptErr/F:eta/F:phi/F");

  _TrackInfo muLeadZpreFSR;
  TBranch* muLeadZpreFSRBranch;
  if(!isData && tree->GetBranchStatus("genM1ZpreFSR"))
    muLeadZpreFSRBranch = outtree->Branch("muLeadZpreFSR",&muLeadZpreFSR,"charge/I:pt/F:ptErr/F:eta/F:phi/F");

  _TrackInfo muSubLeadZpreFSR;
  TBranch* muSubLeadZpreFSRBranch;
  if(!isData && tree->GetBranchStatus("genM2ZpreFSR"))
    muSubLeadZpreFSRBranch = outtree->Branch("muSubLeadZpreFSR",&muSubLeadZpreFSR,"charge/I:pt/F:ptErr/F:eta/F:phi/F");

  _TrackInfo muLeadHpostFSR;
  TBranch* muLeadHpostFSRBranch;
  if(!isData && tree->GetBranchStatus("genM1HpostFSR"))
    muLeadHpostFSRBranch = outtree->Branch("muLeadHpostFSR",&muLeadHpostFSR,"charge/I:pt/F:ptErr/F:eta/F:phi/F");

  _TrackInfo muSubLeadHpostFSR;
  TBranch* muSubLeadHpostFSRBranch;
  if(!isData && tree->GetBranchStatus("genM2HpostFSR"))
    muSubLeadHpostFSRBranch = outtree->Branch("muSubLeadHpostFSR",&muSubLeadHpostFSR,"charge/I:pt/F:ptErr/F:eta/F:phi/F");

  _TrackInfo muLeadZpostFSR;
  TBranch* muLeadZpostFSRBranch;
  if(!isData && tree->GetBranchStatus("genM1ZpostFSR"))
    muLeadZpostFSRBranch = outtree->Branch("muLeadZpostFSR",&muLeadZpostFSR,"charge/I:pt/F:ptErr/F:eta/F:phi/F");

  _TrackInfo muSubLeadZpostFSR;
  TBranch* muSubLeadZpostFSRBranch;
  if(!isData && tree->GetBranchStatus("genM2ZpostFSR"))
    muSubLeadZpostFSRBranch = outtree->Branch("muSubLeadZpostFSR",&muSubLeadZpostFSR,"charge/I:pt/F:ptErr/F:eta/F:phi/F");

  Float_t puweight;
  TBranch* puweightBranch = outtree->Branch("puweight",&puweight,"puweight/F");

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
      break;

    tree->GetEvent(i);
    if (i % reportEach == 0) cout << "Event: " << i << endl;

    // PU reweight
    puweight = 1.;
    if (!isData && lumiWeights != NULL)
    {
      puweight *= lumiWeights->weight(nPU);
    }
    puweightBranch->Fill();

    // Order muons by pt
    if (reco1.pt > reco2.pt)
    {
      muLead = reco1;
      muLeadHpreFSR = genM1HpreFSR;
      muLeadZpreFSR = genM1ZpreFSR;
      muLeadHpostFSR = genM1HpostFSR;
      muLeadZpostFSR = genM1ZpostFSR;
      muSubLead = reco2;
      muSubLeadHpreFSR = genM2HpreFSR;
      muSubLeadZpreFSR = genM2ZpreFSR;
      muSubLeadHpostFSR = genM2HpostFSR;
      muSubLeadZpostFSR = genM2ZpostFSR;
    }
    else
    {
      muLead = reco2;
      muLeadHpreFSR = genM2HpreFSR;
      muLeadZpreFSR = genM2ZpreFSR;
      muLeadHpostFSR = genM2HpostFSR;
      muLeadZpostFSR = genM2ZpostFSR;
      muSubLead = reco1;
      muSubLeadHpreFSR = genM1HpreFSR;
      muSubLeadZpreFSR = genM1ZpreFSR;
      muSubLeadHpostFSR = genM1HpostFSR;
      muSubLeadZpostFSR = genM1ZpostFSR;
    }

  if muLeadHpreFSRBranch != NULL;
    muLeadHpreFSRBranch->Fill();

  if muSubLeadHpreFSRBranch != NULL;
    muSubLeadHpreFSRBranch->Fill();

  if muLeadZpreFSRBranch != NULL;
    muLeadZpreFSRBranch->Fill();

  if muSubLeadZpreFSRBranch != NULL;
    muSubLeadZpreFSRBranch->Fill();

  if muLeadHpostFSRBranch != NULL;
    muLeadHpostFSRBranch->Fill();

  if muSubLeadHpostFSRBranch != NULL;
    muSubLeadHpostFSRBranch->Fill();

  if muLeadZpostFSRBranch != NULL;
    muLeadZpostFSRBranch->Fill();

  if muSubLeadZpostFSRBranch != NULL;
    muSubLeadZpostFSRBranch->Fill();

  }//event loop

  TFile* outFile = new TFile(outputFileName,"RECREATE");
  outFile->cd();
  outtree->Write();

}
