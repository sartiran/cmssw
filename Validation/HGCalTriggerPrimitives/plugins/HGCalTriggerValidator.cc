// -*- C++ -*-
//
// Package:    
// Class:      HGCalTriggerValidator
// 
/**\class 
 Description: [one line class summary]
 Implementation:
     [Notes on implementation]
*/

// system include files
#include <memory>
#include <iostream>
#include <string>
#include <vector>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Common/interface/Provenance.h"

#include <DQMServices/Core/interface/DQMStore.h>
#include <DQMServices/Core/interface/MonitorElement.h>
#include <DQMServices/Core/interface/DQMEDAnalyzer.h>

#include "DataFormats/ForwardDetId/interface/HGCalDetId.h"
#include "DataFormats/L1THGCal/interface/HGCalTriggerCell.h"
#include "DataFormats/L1THGCal/interface/HGCalCluster.h"
#include "DataFormats/L1THGCal/interface/HGCalMulticluster.h"
#include "DataFormats/L1THGCal/interface/HGCalTower.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "L1Trigger/L1THGCal/interface/HGCalTriggerTools.h"

#include "L1Trigger/L1THGCal/interface/backend/HGCalTriggerClusterIdentificationBase.h"

//
// class declaration
//

class HGCalTriggerValidator : public DQMEDAnalyzer {

  public:
    explicit HGCalTriggerValidator(const edm::ParameterSet&);
    ~HGCalTriggerValidator() override;

    void analyze(const edm::Event&, const edm::EventSetup&) override;

  protected:
    void bookHistograms(DQMStore::IBooker &, edm::Run const &, edm::EventSetup const &) override;

  private:
// ----------member data ---------------------------
    //histogram tc related
    MonitorElement *h_tc_n, *h_tc_id, *h_tc_subdet, *h_tc_zside, *h_tc_layer;
    MonitorElement *h_tc_wafer, *h_tc_wafertype, *h_tc_cell, *h_tc_data;
    MonitorElement *h_tc_mipPt, *h_tc_pt, *h_tc_simenergy, *h_tc_x, *h_tc_y; 
    MonitorElement *h_tc_z, *h_tc_energy, *h_tc_eta, *h_tc_phi;

    //histogram cl related
    MonitorElement *h_cl_n, *h_cl_mipPt, *h_cl_pt; 
    MonitorElement *h_cl_energy, *h_cl_eta, *h_cl_phi;
    MonitorElement *h_cl_layer, *h_cl_subdet, *h_cl_cells_n;

    //histogram multicl related
    MonitorElement *h_cl3d_n, *h_cl3d_pt, *h_cl3d_energy;
    MonitorElement *h_cl3d_eta, *h_cl3d_phi, *h_cl3d_clusters_n;
    // cluster shower shapes
    MonitorElement *h_cl3d_showerlength, *h_cl3d_coreshowerlength, *h_cl3d_firstlayer;
    MonitorElement *h_cl3d_maxlayer, *h_cl3d_seetot, *h_cl3d_seemax;
    MonitorElement *h_cl3d_spptot, *h_cl3d_sppmax, *h_cl3d_szz;
    MonitorElement *h_cl3d_srrtot, *h_cl3d_srrmax, *h_cl3d_srrmean;
    MonitorElement *h_cl3d_emaxe, *h_cl3d_bdteg, *h_cl3d_quality;

    //histogram tower related
    MonitorElement *h_tower_n, *h_tower_pt, *h_tower_energy;
    MonitorElement *h_tower_eta, *h_tower_phi, *h_tower_etEm;
    MonitorElement *h_tower_etHad, *h_tower_iEta, *h_tower_iPhi;


    edm::EDGetToken trigger_cells_token_;
    edm::EDGetToken clusters_token_;
    edm::EDGetToken multiclusters_token_; 
    edm::EDGetToken towers_token_;
   
   std::unique_ptr<HGCalTriggerClusterIdentificationBase> id_;
   
private:
    int tc_n;
    int cl_n;
    int cl3d_n;
    int tower_n;
    
    HGCalTriggerTools triggerTools_;
};

HGCalTriggerValidator::HGCalTriggerValidator(const edm::ParameterSet& iConfig){

  trigger_cells_token_ = consumes<l1t::HGCalTriggerCellBxCollection>(iConfig.getParameter<edm::InputTag>("TriggerCells"));
  clusters_token_ = consumes<l1t::HGCalClusterBxCollection>(iConfig.getParameter<edm::InputTag>("Clusters"));
  multiclusters_token_ = consumes<l1t::HGCalMulticlusterBxCollection>(iConfig.getParameter<edm::InputTag>("Multiclusters"));
  towers_token_ = consumes<l1t::HGCalTowerBxCollection>(iConfig.getParameter<edm::InputTag>("Towers"));
  
  id_.reset( HGCalTriggerClusterIdentificationFactory::get()->create("HGCalTriggerClusterIdentificationBDT") );
  id_->initialize(iConfig.getParameter<edm::ParameterSet>("EGIdentification"));

}

HGCalTriggerValidator::~HGCalTriggerValidator(){
}

void HGCalTriggerValidator::bookHistograms(DQMStore::IBooker &iBooker, edm::Run const &, edm::EventSetup const &) {

  //initiating histograms
  // trigger cells 
  h_tc_energy = iBooker.book1D("tc_energy","trigCell energy; energy", 70, 0, 70);
  h_tc_eta = iBooker.book1D("tc_eta","trigCell eta; eta", 8, -4, 4);
  h_tc_phi = iBooker.book1D("tc_phi","trigCell phi; phi", 8, -4, 4);
  h_tc_n = iBooker.book1D("tc_n","trigCell number", 400, 0, 400);
  h_tc_id = iBooker.book1D("tc_id","trigCell id", 30000, 1600000000, 1900000000);
  h_tc_subdet = iBooker.book1D("tc_subdet","trigCell subdet", 4, 2, 6 ); 
  h_tc_zside = iBooker.book1D("tc_side","trigCell side", 4, -2, 2); 
  h_tc_layer = iBooker.book1D("tc_layer","trigCell layer", 50, 0, 50);
  h_tc_wafer = iBooker.book1D("tc_wafer","trigCell wafer", 460, 0, 460);
  h_tc_wafertype = iBooker.book1D("tc_wafertype","trigCell wafertype", 3, 0, 3);
  h_tc_cell = iBooker.book1D("tc_cell","trigCell cell", 50, 0, 50);
  h_tc_data = iBooker.book1D("tc_data","trigCell data", 5000, 0, 50000);
  h_tc_mipPt = iBooker.book1D("tc_mipPt","trigCell mipPt", 400, 0, 400);
  h_tc_pt = iBooker.book1D("tc_pt","trigCell pt", 15, 0, 15);
  h_tc_x = iBooker.book1D("tc_x","trigCell x", 500, -250, 250);
  h_tc_y = iBooker.book1D("tc_y","trigCell y", 500, -250, 250); 
  h_tc_z = iBooker.book1D("tc_z","trigCell z", 1100, -550, 550);

  // cluster 2D histograms
  h_cl_energy= iBooker.book1D("cl_energy","cluster2D energy; energy", 80, 0, 80);
  h_cl_eta = iBooker.book1D("cl_eta","cluster2D eta; eta", 8, -4, 4);
  h_cl_phi = iBooker.book1D("cl_phi","cluster2D phi; phi", 8, -4, 4);
  h_cl_n = iBooker.book1D("cl_n","cluster2D number",80, 0, 80);
  h_cl_mipPt = iBooker.book1D("cl_mipPt","cluster2D mipPt",600, 0, 600);
  h_cl_pt = iBooker.book1D("cl_pt","cluster2D pt",20, 0, 20);
  h_cl_layer = iBooker.book1D("cl_layer","cluster2D layer", 50, 0, 50);
  h_cl_subdet = iBooker.book1D("cl_subdet","cluster2D subdet", 4, 2, 6); 
  h_cl_cells_n = iBooker.book1D("cl_cells_n","cluster2D cells_n", 16, 0, 16);

  // multiclusters
  h_cl3d_energy = iBooker.book1D("cl3d_energy","cl3duster3D energy; energy", 80, 0, 80);
  h_cl3d_eta = iBooker.book1D("cl3d_eta","cl3duster3D eta; eta", 8, -4, 4);
  h_cl3d_phi = iBooker.book1D("cl3d_phi","cl3duster3D phi; phi", 8, -4, 4);
  h_cl3d_n = iBooker.book1D("cl3d_n","cl3duster3D number", 12, 0, 12);
  h_cl3d_pt = iBooker.book1D("cl3d_pt","cl3duster3D pt", 50, 0, 50);
  h_cl3d_clusters_n = iBooker.book1D("cl3d_clusters_n","cl3duster3D clusters_n", 30, 0, 30);
  // cluster shower shapes
  h_cl3d_showerlength = iBooker.book1D("cl3d_showerlength","cl3duster3D showerlength", 50, 0, 50);
  h_cl3d_coreshowerlength = iBooker.book1D("cl3d_coreshowerlength","cl3duster3D coreshowerlength", 16, 0, 16);
  h_cl3d_firstlayer = iBooker.book1D("cl3d_firstlayer","cl3duster3D firstlayer", 50, 0, 50);
  h_cl3d_maxlayer = iBooker.book1D("cl3d_maxlayer","cl3duster3D maxlayer", 50, 0, 50);
  h_cl3d_seetot = iBooker.book1D("cl3d_seetot","cl3duster3D seetot", 50, 0, 0.05);
  h_cl3d_seemax = iBooker.book1D("cl3d_seemax","cl3duster3D seemax", 40, 0, 0.04);
  h_cl3d_spptot = iBooker.book1D("cl3d_spptot","cl3duster3D spptot", 800, 0, 0.08);
  h_cl3d_sppmax = iBooker.book1D("cl3d_sppmax","cl3duster3D sppmax", 800, 0, 0.08);
  h_cl3d_szz = iBooker.book1D("cl3d_szz","cl3duster3D szz", 50, 0, 50);
  h_cl3d_srrtot = iBooker.book1D("cl3d_srrtot","cl3duster3D srrtot", 800, 0, 0.008);
  h_cl3d_srrmax = iBooker.book1D("cl3d_srrmax","cl3duster3D srrmax", 900, 0, 0.009);
  h_cl3d_srrmean = iBooker.book1D("cl3d_srrmean","cl3duster3D srrmean", 800, 0, 0.008);
  h_cl3d_emaxe = iBooker.book1D("cl3d_emaxe","cl3duster3D emaxe", 15, 0, 1.5);
  h_cl3d_bdteg = iBooker.book1D("cl3d_bdteg","cl3duster3D bdteg", 30, -0.7, 0.4);
  h_cl3d_quality = iBooker.book1D("cl3d_quality","cl3duster3D quality", 20, 0, 2);

  // towers
  h_tower_n = iBooker.book1D("tower_n","tower n", 400, 1200, 1600);
  h_tower_pt = iBooker.book1D("tower_pt","tower pt", 50, 0, 50);
  h_tower_energy = iBooker.book1D("tower_energy","tower energy", 200, 0, 200);
  h_tower_eta = iBooker.book1D("tower_eta","tower eta", 8, -4, 4); 
  h_tower_phi = iBooker.book1D("tower_phi","tower phi", 8, -4, 4);
  h_tower_etEm = iBooker.book1D("tower_etEm","tower etEm",  50, 0, 50);
  h_tower_etHad = iBooker.book1D("tower_etHad","tower etHad", 30, 0, 0.3);
  h_tower_iEta = iBooker.book1D("tower_iEta","tower iEta", 20, 0, 20);
  h_tower_iPhi = iBooker.book1D("tower_iPhi","tower iPhi", 80, 0, 80);
}

void HGCalTriggerValidator::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){
  using namespace edm;

  tc_n = 0;
  cl_n = 0;
  cl3d_n = 0;
  tower_n = 0;

  triggerTools_.eventSetup(iSetup);

  // retrieve trigger cells
  edm::Handle<l1t::HGCalTriggerCellBxCollection> trigger_cells_h;
  iEvent.getByToken(trigger_cells_token_, trigger_cells_h);
  const l1t::HGCalTriggerCellBxCollection& trigger_cells = *trigger_cells_h;

  if (trigger_cells_h.isValid()){
    for(auto tc_itr=trigger_cells.begin(0); tc_itr!=trigger_cells.end(0); tc_itr++)
    {
      tc_n++;
      HGCalDetId id(tc_itr->detId());
      h_tc_energy->Fill(tc_itr->energy());
      h_tc_eta->Fill(tc_itr->eta());
      h_tc_phi->Fill(tc_itr->phi());
      h_tc_id->Fill(tc_itr->detId());
      h_tc_subdet->Fill(id.subdetId());
      h_tc_zside->Fill(id.zside());
      h_tc_layer->Fill(triggerTools_.layerWithOffset(id));
      h_tc_wafer->Fill(id.wafer());
      h_tc_wafertype->Fill(id.waferType());
      h_tc_cell->Fill(id.cell());
      h_tc_data->Fill(tc_itr->hwPt());
      h_tc_mipPt->Fill(tc_itr->mipPt());
      h_tc_pt->Fill(tc_itr->pt());
      h_tc_x->Fill(tc_itr->position().x());
      h_tc_y->Fill(tc_itr->position().y()); 
      h_tc_z->Fill(tc_itr->position().z());
    }
  }
  h_tc_n->Fill(tc_n);

  // retrieve clusters
  edm::Handle<l1t::HGCalClusterBxCollection> clusters_h;
  iEvent.getByToken(clusters_token_, clusters_h);
  const l1t::HGCalClusterBxCollection& clusters = *clusters_h;

  if (clusters_h.isValid()){
    for(auto cl_itr=clusters.begin(0); cl_itr!=clusters.end(0); cl_itr++){
      cl_n++;
      h_cl_mipPt->Fill(cl_itr->mipPt());
      h_cl_pt->Fill(cl_itr->pt());
      h_cl_energy->Fill(cl_itr->energy());
      h_cl_eta->Fill(cl_itr->eta());
      h_cl_phi->Fill(cl_itr->phi());
      h_cl_layer->Fill(triggerTools_.layerWithOffset(cl_itr->detId()));
      h_cl_subdet->Fill(cl_itr->subdetId());
      h_cl_cells_n->Fill(cl_itr->constituents().size());
    }
  }
  h_cl_n->Fill(cl_n);

  // retrieve clusters 3D
  edm::Handle<l1t::HGCalMulticlusterBxCollection> multiclusters_h;
  iEvent.getByToken(multiclusters_token_, multiclusters_h);
  const l1t::HGCalMulticlusterBxCollection& multiclusters = *multiclusters_h;

  if (multiclusters_h.isValid()){
       
    for(auto cl3d_itr=multiclusters.begin(0); cl3d_itr!=multiclusters.end(0); cl3d_itr++){
      
      cl3d_n++;
      h_cl3d_pt->Fill(cl3d_itr->pt());
      h_cl3d_energy->Fill(cl3d_itr->energy());
      h_cl3d_eta->Fill(cl3d_itr->eta());
      h_cl3d_phi->Fill(cl3d_itr->phi());
      h_cl3d_clusters_n->Fill(cl3d_itr->constituents().size());
      // cluster shower shapes
      h_cl3d_showerlength->Fill(cl3d_itr->showerLength());
      h_cl3d_coreshowerlength->Fill(cl3d_itr->coreShowerLength());
      h_cl3d_firstlayer->Fill(cl3d_itr->firstLayer());
      h_cl3d_maxlayer->Fill(cl3d_itr->maxLayer());
      h_cl3d_seetot->Fill(cl3d_itr->sigmaEtaEtaTot());
      h_cl3d_seemax->Fill(cl3d_itr->sigmaEtaEtaMax());
      h_cl3d_spptot->Fill(cl3d_itr->sigmaPhiPhiTot());
      h_cl3d_sppmax->Fill(cl3d_itr->sigmaPhiPhiMax());
      h_cl3d_szz->Fill(cl3d_itr->sigmaZZ());
      h_cl3d_srrtot->Fill(cl3d_itr->sigmaRRTot());
      h_cl3d_srrmax->Fill(cl3d_itr->sigmaRRMax());
      h_cl3d_srrmean->Fill(cl3d_itr->sigmaRRMean());
      h_cl3d_emaxe->Fill(cl3d_itr->eMax()/cl3d_itr->energy());
      h_cl3d_bdteg->Fill(id_->value(*cl3d_itr));
      h_cl3d_quality->Fill(cl3d_itr->hwQual());
    }
  }
  h_cl3d_n->Fill(cl3d_n);

  // retrieve towers
  edm::Handle<l1t::HGCalTowerBxCollection> towers_h;
  iEvent.getByToken(towers_token_, towers_h);
  const l1t::HGCalTowerBxCollection& towers = *towers_h;

  if (towers_h.isValid()){
    for(auto tower_itr=towers.begin(0); tower_itr!=towers.end(0); tower_itr++){   
      tower_n++;
      h_tower_pt->Fill(tower_itr->pt());
      h_tower_energy->Fill(tower_itr->energy());
      h_tower_eta->Fill(tower_itr->eta());
      h_tower_phi->Fill(tower_itr->phi());
      h_tower_etEm->Fill(tower_itr->etEm());
      h_tower_etHad->Fill(tower_itr->etHad());
      h_tower_iEta->Fill(tower_itr->id().iEta());
      h_tower_iPhi->Fill(tower_itr->id().iPhi());
    }
  }
  h_tower_n->Fill(tower_n);

}

#include "FWCore/Framework/interface/MakerMacros.h"

//define this as a plug-in
DEFINE_FWK_MODULE(HGCalTriggerValidator);
