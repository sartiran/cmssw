#ifndef __L1Trigger_L1THGCal_HGCalBackendProcessor_h__
#define __L1Trigger_L1THGCal_HGCalBackendProcessor_h__

#include "L1Trigger/L1THGCal/interface/HGCalBackendProcessorBase.h"
//#include "L1Trigger/L1THGCal/interface/backend/HGCalBackendProcessorImpl.h"

#include "DataFormats/L1THGCal/interface/HGCalTriggerCell.h"
#include "DataFormats/L1THGCal/interface/HGCalTriggerSums.h"
#include "DataFormats/L1THGCal/interface/HGCalCluster.h"
#include "DataFormats/L1THGCal/interface/HGCalMulticluster.h"

class HGCalBackendProcessor : public HGCalBackendProcessorBase 
{

 public:
  HGCalBackendProcessor(const edm::ParameterSet& conf);
  
  virtual void setProduces(edm::stream::EDProducer<>& prod) const override final 
  { 
    prod.produces<l1t::HGCalTriggerCellBxCollection>( "calibratedTriggerCells" );            
    prod.produces<l1t::HGCalClusterBxCollection>( "cluster2D" );
    prod.produces<l1t::HGCalMulticlusterBxCollection>( "cluster3D" ); 
  }
  
  virtual void reset() override final 
  { 
    triggerCellBe_product_.reset( new l1t::HGCalTriggerCellBxCollection );
    cluster_product_.reset( new l1t::HGCalClusterBxCollection );
    multicluster_product_.reset( new l1t::HGCalMulticlusterBxCollection );
  }
  
  virtual void run(const l1t::HGCalTriggerCellBxCollection& coll, const edm::EventSetup& es,
		         edm::Event&evt
			) override final
  {
     // Will be implemented in the next version
  }

  void putInEvent(edm::Event& evt);
  
 private:
  //HGCalBackendProcessorImpl BackendProcImpl_;
  
  std::unique_ptr<l1t::HGCalTriggerCellBxCollection> triggerCellBe_product_;
  std::unique_ptr<l1t::HGCalClusterBxCollection> cluster_product_;
  std::unique_ptr<l1t::HGCalMulticlusterBxCollection> multicluster_product_;

  /* lables of sensitive detector (geometry record) */
  std::string HGCalEESensitive_;
  std::string HGCalHESiliconSensitive_;

  /* handles to the detector topologies */
  edm::ESHandle<HGCalTopology> hgceeTopoHandle_;
  edm::ESHandle<HGCalTopology> hgchefTopoHandle_;
  edm::ESHandle<HGCalTriggerGeometryBase> triggerGeometry_;
  
  /* algorithms instances */
  //HGCalTriggerCellCalibration calibration_;
  //HGCalClusteringImpl clustering_;
  //HGCalMulticlusteringImpl multiclustering_;

};

#endif
