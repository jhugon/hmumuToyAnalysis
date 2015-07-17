
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

  //////////////////////////
  // Tree Branches
  cout << "Analyzing filename: "<< inputFileName.Data() << endl;
  if (isData)
    cout << "isData\n";
  if (isSignal)
    cout << "isSignal\n";

  TFile* inFile = new TFile(inputFileName,"READ");
  TTree * tree = (TTree*) inFile->Get("tree");
  if (inFile == NULL)
  {
    cout << "Could not open file, not making tree" << endl;
    return;
  }
  if (tree == NULL)
  {
    cout << "Could not find tree" << endl;
    return;
  }


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

  reweight::LumiReWeighting* lumiWeights = NULL; 
  if (runPeriod == "7TeV")
  {
    cout << "Using 2011AB PU reweighting\n";
    lumiWeights = new reweight::LumiReWeighting("pileupDists/PileUpHistMCFall11.root","pileupDists/PileUpHist2011AB.root","pileup","pileup");
  }
  else if (runPeriod == "8TeV")
  {
    cout << "Using 2012ABCD PU reweighting\n";
    lumiWeights = new reweight::LumiReWeighting("pileupDists/PileUpHistMC2012Summer50nsPoissonOOTPU.root","pileupDists/PileUpHist2012ABCD.root","pileup","pileup");
  }

  ////////////////////////////
  // Output Tree and Branches
  //

  TTree* outtree = (TTree*) tree->Clone("outtree");

  _MuonInfo muLead;
  TBranch* muLeadBranch = outtree->Branch("muLead", &muLead, "isTracker/I:isStandAlone/I:isGlobal/I:charge/I:pt/F:ptErr/F:eta/F:phi/F:trkPt/F:trkPtErr/F:trkEta/F:trkPhi/F:normChiSquare/F:d0_BS/F:dz_BS/F:d0_PV/F:dz_PV/F:numPixelLayers/I:numTrackerLayers/I:numStripLayers/I:validFracTracker/F:numValidMuonHits/I:numValidPixelHits/I:numValidTrackerHits/I:numValidStripHits/I:numSegmentMatches/I:numOfMatchedStations/I:trackIsoSumPt/F:trackIsoSumPtCorr/F:hcalIso/F:ecalIso/F:relCombIso/F:isPFMuon/I:pfPt/F:pfEta/F:pfPhi/F:sumChargedHadronPtR03/F:sumChargedParticlePtR03/F:sumNeutralHadronEtR03/F:sumPhotonEtR03/F:sumPUPtR03/F:sumChargedHadronPtR04/F:sumChargedParticlePtR04/F:sumNeutralHadronEtR04/F:sumPhotonEtR04/F:sumPUPtR04/F:isHltMatched[3]/I:hltPt[3]/F:hltEta[3]/F:hltPhi[3]/F");

  _MuonInfo muSubLead;
  TBranch* muSubLeadBranch = outtree->Branch("muSubLead", &muSubLead, "isTracker/I:isStandAlone/I:isGlobal/I:charge/I:pt/F:ptErr/F:eta/F:phi/F:trkPt/F:trkPtErr/F:trkEta/F:trkPhi/F:normChiSquare/F:d0_BS/F:dz_BS/F:d0_PV/F:dz_PV/F:numPixelLayers/I:numTrackerLayers/I:numStripLayers/I:validFracTracker/F:numValidMuonHits/I:numValidPixelHits/I:numValidTrackerHits/I:numValidStripHits/I:numSegmentMatches/I:numOfMatchedStations/I:trackIsoSumPt/F:trackIsoSumPtCorr/F:hcalIso/F:ecalIso/F:relCombIso/F:isPFMuon/I:pfPt/F:pfEta/F:pfPhi/F:sumChargedHadronPtR03/F:sumChargedParticlePtR03/F:sumNeutralHadronEtR03/F:sumPhotonEtR03/F:sumPUPtR03/F:sumChargedHadronPtR04/F:sumChargedParticlePtR04/F:sumNeutralHadronEtR04/F:sumPhotonEtR04/F:sumPUPtR04/F:isHltMatched[3]/I:hltPt[3]/F:hltEta[3]/F:hltPhi[3]/F");

  _TrackInfo muLeadHpreFSR;
  TBranch* muLeadHpreFSRBranch = NULL;
  if(!isData && tree->GetBranchStatus("genM1HpreFSR"))
    muLeadHpreFSRBranch = outtree->Branch("muLeadHpreFSR",&muLeadHpreFSR,"charge/I:pt/F:ptErr/F:eta/F:phi/F");

  _TrackInfo muSubLeadHpreFSR;
  TBranch* muSubLeadHpreFSRBranch = NULL;
  if(!isData && tree->GetBranchStatus("genM2HpreFSR"))
    muSubLeadHpreFSRBranch = outtree->Branch("muSubLeadHpreFSR",&muSubLeadHpreFSR,"charge/I:pt/F:ptErr/F:eta/F:phi/F");

  _TrackInfo muLeadZpreFSR;
  TBranch* muLeadZpreFSRBranch = NULL;
  if(!isData && tree->GetBranchStatus("genM1ZpreFSR"))
    muLeadZpreFSRBranch = outtree->Branch("muLeadZpreFSR",&muLeadZpreFSR,"charge/I:pt/F:ptErr/F:eta/F:phi/F");

  _TrackInfo muSubLeadZpreFSR;
  TBranch* muSubLeadZpreFSRBranch = NULL;
  if(!isData && tree->GetBranchStatus("genM2ZpreFSR"))
    muSubLeadZpreFSRBranch = outtree->Branch("muSubLeadZpreFSR",&muSubLeadZpreFSR,"charge/I:pt/F:ptErr/F:eta/F:phi/F");

  _TrackInfo muLeadHpostFSR;
  TBranch* muLeadHpostFSRBranch = NULL;
  if(!isData && tree->GetBranchStatus("genM1HpostFSR"))
    muLeadHpostFSRBranch = outtree->Branch("muLeadHpostFSR",&muLeadHpostFSR,"charge/I:pt/F:ptErr/F:eta/F:phi/F");

  _TrackInfo muSubLeadHpostFSR;
  TBranch* muSubLeadHpostFSRBranch = NULL;
  if(!isData && tree->GetBranchStatus("genM2HpostFSR"))
    muSubLeadHpostFSRBranch = outtree->Branch("muSubLeadHpostFSR",&muSubLeadHpostFSR,"charge/I:pt/F:ptErr/F:eta/F:phi/F");

  _TrackInfo muLeadZpostFSR;
  TBranch* muLeadZpostFSRBranch = NULL;
  if(!isData && tree->GetBranchStatus("genM1ZpostFSR"))
    muLeadZpostFSRBranch = outtree->Branch("muLeadZpostFSR",&muLeadZpostFSR,"charge/I:pt/F:ptErr/F:eta/F:phi/F");

  _TrackInfo muSubLeadZpostFSR;
  TBranch* muSubLeadZpostFSRBranch = NULL;
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
    if(i < maxEvents)
    {
      tree->GetEvent(i);
      if (i % reportEach == 0) cout << "Event: " << i << endl;

      // PU reweight
      puweight = 1.;
      if (!isData && lumiWeights)
      {
        puweight *= lumiWeights->weight(nPU);
      }

      // Order muons by pt
      if (reco1.pt > reco2.pt)
      {
        muLead = reco1;
        muSubLead = reco2;
      }
      else
      {
        muLead = reco2;
        muSubLead = reco1;
      }

      // same for each pair of gen muons
      if (genM1HpreFSR.pt > genM2HpreFSR.pt)
      {
        muLeadHpreFSR = genM1HpreFSR;
        muSubLeadHpreFSR = genM2HpreFSR;
      }
      else
      {
        muLeadHpreFSR = genM2HpreFSR;
        muSubLeadHpreFSR = genM1HpreFSR;
      }

      if (genM1ZpreFSR.pt > genM2ZpreFSR.pt)
      {
        muLeadZpreFSR = genM1ZpreFSR;
        muSubLeadZpreFSR = genM2ZpreFSR;
      }
      else
      {
        muLeadZpreFSR = genM2ZpreFSR;
        muSubLeadZpreFSR = genM1ZpreFSR;
      }

      if (genM1HpostFSR.pt > genM2HpostFSR.pt)
      {
        muLeadHpostFSR = genM1HpostFSR;
        muSubLeadHpostFSR = genM2HpostFSR;
      }
      else
      {
        muLeadHpostFSR = genM2HpostFSR;
        muSubLeadHpostFSR = genM1HpostFSR;
      }

      if (genM1ZpostFSR.pt > genM2ZpostFSR.pt)
      {
        muLeadZpostFSR = genM1ZpostFSR;
        muSubLeadZpostFSR = genM2ZpostFSR;
      }
      else
      {
        muLeadZpostFSR = genM2ZpostFSR;
        muSubLeadZpostFSR = genM1ZpostFSR;
      }

    } // < maxEvents
    else
    {
      puweight = -999.;
      initMuonStruct(muLead);
      initTrackStruct(muLeadHpreFSR);
      initTrackStruct(muLeadZpreFSR);
      initTrackStruct(muLeadHpostFSR);
      initTrackStruct(muLeadZpostFSR);
      initMuonStruct(muSubLead);
      initTrackStruct(muSubLeadHpreFSR);
      initTrackStruct(muSubLeadZpreFSR);
      initTrackStruct(muSubLeadHpostFSR);
      initTrackStruct(muSubLeadZpostFSR);
    } 

    puweightBranch->Fill();

    muLeadBranch->Fill();
    muSubLeadBranch->Fill();

    if (muLeadHpreFSRBranch)
      muLeadHpreFSRBranch->Fill();

    if (muSubLeadHpreFSRBranch)
      muSubLeadHpreFSRBranch->Fill();

    if (muLeadZpreFSRBranch)
      muLeadZpreFSRBranch->Fill();

    if (muSubLeadZpreFSRBranch)
      muSubLeadZpreFSRBranch->Fill();

    if (muLeadHpostFSRBranch)
      muLeadHpostFSRBranch->Fill();

    if (muSubLeadHpostFSRBranch)
      muSubLeadHpostFSRBranch->Fill();

    if (muLeadZpostFSRBranch)
      muLeadZpostFSRBranch->Fill();

    if (muSubLeadZpostFSRBranch)
      muSubLeadZpostFSRBranch->Fill();

    if(i >= maxEvents)
      break;
  }//event loop

  TFile* outFile = new TFile(outputFileName,"RECREATE");
  outFile->cd();
  outtree->Write();

}
