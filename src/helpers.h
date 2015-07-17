#include <cmath>
#include <iostream>
#include <algorithm>
#include "TRandom.h"
#include "TLorentzVector.h"
#include "TStyle.h"

enum PUJetID
{
  puJetTight  = 0,
  puJetMedium = 1,
  puJetLoose  = 2
};

enum SelectionCodes
{
  notSelected_Code = 0,

  vbfPresel_Code = 1,
  vbfBDTCut_Code = 2,
  incPresel_Code = 4,
  incBDTCut_Code = 8,
  isBB_Code = 16,
  isBO_Code = 32,
  isBE_Code = 64,
  isOO_Code = 128,
  isOE_Code = 256,
  isEE_Code = 512,
  isNotBB_Code = 1024,
};


inline float quadsum(float x, float y)
{
  return TMath::Sqrt(x*x+y*y);
};

float getPFRelIso(_MuonInfo& muon)
{
  // tracker iso cut
  //if (muon.trackIsoSumPt/muon.pt>0.1) return isKinTight_2012;  

  float result = muon.sumChargedHadronPtR04 + 
    std::max(0.0,muon.sumNeutralHadronEtR04 + muon.sumPhotonEtR04 - 0.5*muon.sumPUPtR04);

  return result/muon.pt;
}

float getTrkRelIso(_MuonInfo& muon)
{
  // tracker iso cut
  return muon.trackIsoSumPt/muon.pt;

}

bool isKinTight_2012(_MuonInfo& muon) 
{

  bool isKinTight_2012=false;

  if (!muon.isGlobal)  return isKinTight_2012;
  if (!muon.isPFMuon) return isKinTight_2012;

  // acceptance cuts
  if (muon.pt < 15)         return isKinTight_2012; // pt cut
  if (fabs(muon.eta) > 2.1) return isKinTight_2012; // eta cut

  // kinematic cuts
  if (muon.numTrackerLayers < 6) return isKinTight_2012; // # hits in tracker

  if(getPFRelIso(muon) > 0.12)
      return isKinTight_2012;

  if (fabs(muon.d0_PV) > 0.2) return isKinTight_2012;
  if (fabs(muon.dz_PV) > 0.5) return isKinTight_2012;

  if ( muon.numValidMuonHits  < 1 ) return isKinTight_2012;
  if ( muon.numValidPixelHits < 1 ) return isKinTight_2012;
  if ( muon.numOfMatchedStations < 2 ) return isKinTight_2012;
  if ( muon.normChiSquare > 10)     return isKinTight_2012;

  isKinTight_2012=true;
  return isKinTight_2012;
}

bool isKinTight_2011(_MuonInfo& muon) 
{

  bool isKinTight_2011=false;

  if (!muon.isGlobal)  return isKinTight_2011;
  if (!muon.isTracker) return isKinTight_2011;

  // acceptance cuts
  if (muon.pt < 15)         return isKinTight_2011; // pt cut
  if (fabs(muon.eta) > 2.1) return isKinTight_2011; // eta cut

  // kinematic cuts
  if (muon.numTrackerLayers < 9) return isKinTight_2011; // # hits in tracker

  if(getPFRelIso(muon) > 0.12)
      return isKinTight_2011;

  if (fabs(muon.d0_PV) > 0.2) return isKinTight_2011;
  //if (fabs(muon.dz_PV) > 0.5) return isKinTight_2011;

  if ( muon.numValidMuonHits  < 1 ) return isKinTight_2011;
  if ( muon.numValidPixelHits < 1 ) return isKinTight_2011;
  if ( muon.numOfMatchedStations < 2 ) return isKinTight_2011;
  if ( muon.normChiSquare > 10)     return isKinTight_2011;

  isKinTight_2011=true;
  return isKinTight_2011;
}

bool isKinTight_2012_noIso(_MuonInfo& muon) 
{

  bool isKinTight_2012=false;

  if (!muon.isGlobal)  return isKinTight_2012;
  if (!muon.isPFMuon) return isKinTight_2012;

  // acceptance cuts
  if (muon.pt < 15)         return isKinTight_2012; // pt cut
  if (fabs(muon.eta) > 2.1) return isKinTight_2012; // eta cut

  // kinematic cuts
  if (muon.numTrackerLayers < 6) return isKinTight_2012; // # hits in tracker

  //if(getPFRelIso(muon) > 0.12)
  //return isKinTight_2012;

  if (fabs(muon.d0_PV) > 0.2) return isKinTight_2012;
  if (fabs(muon.dz_PV) > 0.5) return isKinTight_2012;

  if ( muon.numValidMuonHits  < 1 ) return isKinTight_2012;
  if ( muon.numValidPixelHits < 1 ) return isKinTight_2012;
  if ( muon.numOfMatchedStations < 2 ) return isKinTight_2012;
  if ( muon.normChiSquare > 10)     return isKinTight_2012;

  isKinTight_2012=true;
  return isKinTight_2012;
}

bool isKinTight_2012_noIso_noPF(_MuonInfo& muon)
{

  bool isKinTight_2012=false;
  if (!muon.isGlobal)  return isKinTight_2012;

  // acceptance cuts
  if (muon.pt < 15)         return isKinTight_2012; // pt cut
  if (fabs(muon.eta) > 2.1) return isKinTight_2012; // eta cut

  // kinematic cuts
  if (muon.numTrackerLayers < 6) return isKinTight_2012; // # hits in tracker

  //if(getPFRelIso(muon) > 0.12)
  //return isKinTight_2012;

  if (fabs(muon.d0_PV) > 0.2) return isKinTight_2012;
  if (fabs(muon.dz_PV) > 0.5) return isKinTight_2012;

  if ( muon.numValidMuonHits  < 1 ) return isKinTight_2012;
  if ( muon.numValidPixelHits < 1 ) return isKinTight_2012;
  if ( muon.numOfMatchedStations < 2 ) return isKinTight_2012;
  if ( muon.normChiSquare > 10)     return isKinTight_2012;

  isKinTight_2012=true;
  return isKinTight_2012;
}

bool isKinTight_2011_noIso(_MuonInfo& muon) 
{

  bool isKinTight_2011=false;

  if (!muon.isGlobal)  return isKinTight_2011;
  if (!muon.isTracker) return isKinTight_2011;

  // acceptance cuts
  if (muon.pt < 15)         return isKinTight_2011; // pt cut
  if (fabs(muon.eta) > 2.1) return isKinTight_2011; // eta cut

  // kinematic cuts
  if (muon.numTrackerLayers < 9) return isKinTight_2011; // # hits in tracker

  //if(getPFRelIso(muon) > 0.12)
  //  return isKinTight_2011;

  if (fabs(muon.d0_PV) > 0.2) return isKinTight_2011;
  //if (fabs(muon.dz_PV) > 0.5) return isKinTight_2011;

  if ( muon.numValidMuonHits  < 1 ) return isKinTight_2011;
  if ( muon.numValidPixelHits < 1 ) return isKinTight_2011;
  if ( muon.numOfMatchedStations < 2 ) return isKinTight_2011;
  if ( muon.normChiSquare > 10)     return isKinTight_2011;

  isKinTight_2011=true;
  return isKinTight_2011;
}



bool passPUJetID(int flag, PUJetID desiredLevel)
{
 bool result = ( flag & (1 << desiredLevel) ) != 0;
 // std::cout << "puJetID: " << flag << " level: " << desiredLevel << " pass: "<< result << std::endl;
 return result;
}

float smearMC(float trueVal, float recoVal, float calib, float smearRatio, bool debug)
{
  if (trueVal > 0.0)
  {
    float uncalibratedReco = recoVal+calib;
    float dif = uncalibratedReco-trueVal;
    dif *= smearRatio;
    float result = trueVal+dif;
    if (debug)
      std::cout << "true: " << trueVal << " \t\tuncalReco: " << uncalibratedReco << " \t\trecoCand: " << recoVal << " \t\tfinal: " << result << std::endl;
    return result;
  }
  else
  {
    return recoVal+calib;
  }
}

bool isHltMatched(_MuonInfo& muon1, _MuonInfo& muon2, std::vector<int> allowedPaths)
{
  std::vector<int>::const_iterator path = allowedPaths.begin();
  std::vector<int>::const_iterator endPath = allowedPaths.end();
  for(;path != endPath;path++)
  {
    //std::cout << "muon1 index "<<*path<<": "<<muon1.isHltMatched[*path]<<std::endl;
    //std::cout << "muon2 index "<<*path<<": "<<muon2.isHltMatched[*path]<<std::endl;
    if(muon1.isHltMatched[*path]==1)
        return true;
    if(muon2.isHltMatched[*path]==1)
        return true;
  }
  return false;
}

bool isHltMatched(_MuonInfo& muon1, std::vector<int> allowedPaths)
{
  std::vector<int>::const_iterator path = allowedPaths.begin();
  std::vector<int>::const_iterator endPath = allowedPaths.end();
  for(;path != endPath;path++)
  {
    //std::cout << "muon1 index "<<*path<<": "<<muon1.isHltMatched[*path]<<std::endl;
    if(muon1.isHltMatched[*path]==1)
        return true;
  }
  return false;
}

// from Michele for Jet Energy Resolution
void resolutionBiasAndErr(float eta, float& res, float& resErr)
{
    eta = fabs(eta);
    res    = 1.0;
    resErr = 0.0;
    // from https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution
    if (eta <= 0.5) {
        res    = 1.052;
        resErr = quadsum(0.012, 0.062);
    } else if (0.5 < eta && eta <= 1.1) {
        res    = 1.057;
        resErr = quadsum(0.012, 0.062);
    } else if (1.1 < eta && eta <= 1.7) {
        res    = 1.096;
        resErr = quadsum(0.017, 0.063);
    } else if (1.7 < eta && eta <= 2.3) {
        res    = 1.134;
        resErr = quadsum(0.035, 0.087);
    } else {
        res    = 1.288;
        resErr = quadsum(0.127, 0.155);
    }
}



float jerCorr(float ptold, float oldgenpt, float etaold){

  float corrpt=ptold;
  if (oldgenpt>15. && (fabs(ptold - oldgenpt)/ oldgenpt)<0.5) {
    float res=1.;
    float resErr=0.;
    resolutionBiasAndErr(etaold,res,resErr);
    corrpt = TMath::Max(float(0.0),oldgenpt+(ptold-oldgenpt)*res);
  }
  //std::cout << " final etaold, ptold, genpt, deltapt, corrpt" << etaold  << ", " << ptold<< ", "<<oldgenpt << ", " <<  (corrpt-ptold)  << ", " << corrpt << std::endl;
  //std::cout << "   final resCorFactor: " << (corrpt-oldgenpt)/(ptold-oldgenpt)  << std::endl;
  return corrpt;
}

float corrPtUp(float ptold, float oldgenpt, float etaold){
  float corrpt=ptold;
  if (oldgenpt>15. && (fabs(ptold - oldgenpt)/ oldgenpt)<0.5) {
    float res=1.;
    float resErr=0.;
    resolutionBiasAndErr(etaold,res,resErr);
    corrpt = TMath::Max(float(0.0),oldgenpt+(ptold-oldgenpt)*(res+resErr));
  }
  //std::cout << " UP final etaold, ptold, genpt, deltapt, corrpt" << etaold  << ", " << ptold<< ", "<<oldgenpt << ", " <<  (corrpt-ptold)  << ", " << corrpt << std::endl;
  //std::cout << "   final resCorFactor: " << (corrpt-oldgenpt)/(ptold-oldgenpt)  << std::endl;
  return corrpt;
}
  
float corrPtDown(float ptold, float oldgenpt, float etaold){
  float corrpt=ptold;
  if (oldgenpt>15. && (fabs(ptold - oldgenpt)/ oldgenpt)<0.5) {
    float res=1.;
    float resErr=0.;
    resolutionBiasAndErr(etaold,res,resErr);
    corrpt = TMath::Max(float(0.0),oldgenpt+(ptold-oldgenpt)*(res-resErr));
  }
  //std::cout << " DOWN final etaold, ptold, genpt, deltapt, corrpt" << etaold  << ", " << ptold<< ", "<<oldgenpt << ", " <<  (corrpt-ptold)  << ", " << corrpt << std::endl;
  //std::cout << "   final resCorFactor: " << (corrpt-oldgenpt)/(ptold-oldgenpt)  << std::endl;
  return corrpt;
}

void setStyle()
{
  gStyle->SetCanvasColor(0);
  gStyle->SetCanvasBorderSize(10);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasDefH(700);
  gStyle->SetCanvasDefW(700);

  gStyle->SetPadColor       (0);
  gStyle->SetPadBorderSize  (10);
  gStyle->SetPadBorderMode  (0);
  gStyle->SetPadBottomMargin(0.13);
  gStyle->SetPadTopMargin   (0.08);
  gStyle->SetPadLeftMargin  (0.15);
  gStyle->SetPadRightMargin (0.05);
  gStyle->SetPadGridX       (0);
  gStyle->SetPadGridY       (0);
  gStyle->SetPadTickX       (1);
  gStyle->SetPadTickY       (1);

  gStyle->SetFrameFillStyle ( 0);
  gStyle->SetFrameFillColor ( 0);
  gStyle->SetFrameLineColor ( 1);
  gStyle->SetFrameLineStyle ( 0);
  gStyle->SetFrameLineWidth ( 1);
  gStyle->SetFrameBorderSize(10);
  gStyle->SetFrameBorderMode( 0);

  gStyle->SetNdivisions(505);

  gStyle->SetLineWidth(2);
  gStyle->SetHistLineWidth(2);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetLegendFillColor(0);
  gStyle->SetLegendFont(42);
  gStyle->SetMarkerSize(1.2);
  gStyle->SetMarkerStyle(20);

  gStyle->SetLabelSize(0.040,"X");
  gStyle->SetLabelSize(0.040,"Y");

  gStyle->SetLabelOffset(0.010,"X");
  gStyle->SetLabelOffset(0.010,"Y");

  gStyle->SetLabelFont(42,"X");
  gStyle->SetLabelFont(42,"Y");
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFont(42);
  gStyle->SetTitleFont(42,"X");
  gStyle->SetTitleFont(42,"Y");

  gStyle->SetTitleSize(0.045,"X");
  gStyle->SetTitleSize(0.045,"Y");

  gStyle->SetTitleOffset(1.4,"X");
  gStyle->SetTitleOffset(1.4,"Y");

  gStyle->SetTextSize(0.055);
  gStyle->SetTextFont(42);

  gStyle->SetOptStat(0);
}

void setHistTitles(TH1F* hist, TString xtitle, TString ytitle)
{
  hist->GetXaxis()->SetTitle(xtitle);
  hist->GetYaxis()->SetTitle(ytitle);
}

