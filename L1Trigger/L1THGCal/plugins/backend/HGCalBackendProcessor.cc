#include "L1Trigger/L1THGCal/interface/HGCalBackendProcessorBase.h"
//#include "L1Trigger/L1THGCal/interface/backend/HGCalBackendProcessorImpl.h"

#include "DataFormats/L1THGCal/interface/HGCalTriggerCell.h"
#include "DataFormats/L1THGCal/interface/HGCalTriggerSums.h"
#include "DataFormats/L1THGCal/interface/HGCalCluster.h"
#include "DataFormats/L1THGCal/interface/HGCalMulticluster.h"

#include "L1Trigger/L1THGCal/interface/be_algorithms/HGCalTriggerCellCalibration.h"
#include "L1Trigger/L1THGCal/interface/be_algorithms/HGCalClusteringImpl.h"
#include "L1Trigger/L1THGCal/interface/be_algorithms/HGCalMulticlusteringImpl.h" 

//#include "L1Trigger/L1THGCal/interface/backend/HGCalBackendProcessor.h"

#include "L1Trigger/L1THGCal/interface/be_algorithms/HGCalClusteringImpl.h"
#include "L1Trigger/L1THGCal/interface/be_algorithms/HGCalMulticlusteringImpl.h" 

#include <limits>

	
class HGCalBackendProcessor : public HGCalBackendProcessorBase 
{

 public:
   HGCalBackendProcessor(const edm::ParameterSet& conf)  : 
		HGCalBackendProcessorBase(conf),
		HGCalEESensitive_( conf.getParameter<std::string>("HGCalEESensitive_tag") ),
                HGCalHESiliconSensitive_( conf.getParameter<std::string>("HGCalHESiliconSensitive_tag") ),
		calibration_( conf.getParameterSet("calib_parameters") ),
		clustering_( conf.getParameterSet("C2d_parameters") ),
		multiclustering_( conf.getParameterSet("C3d_parameters" ) )
  {
  }
  
    
  virtual void setProduces(edm::stream::EDProducer<>& prod) const override final 
  { 
    prod.produces<l1t::HGCalTriggerCellBxCollection>( "calibratedTriggerCells" );            
    prod.produces<l1t::HGCalClusterBxCollection>( "cluster2D" );
    prod.produces<l1t::HGCalMulticlusterBxCollection>( "cluster3D" ); 
  }
  
    void run(const l1t::HGCalTriggerCellBxCollection& coll, const edm::EventSetup& es,edm::Event&evt)
  {
     
    es.get<IdealGeometryRecord>().get( HGCalEESensitive_,        hgceeTopoHandle_ );
    es.get<IdealGeometryRecord>().get( HGCalHESiliconSensitive_, hgchefTopoHandle_ );
    es.get<IdealGeometryRecord>().get("", triggerGeometry_);
    
    int bxFirst = coll.getFirstBX();
    int bxLast = coll.getLastBX();
     
    // loop over BX
    for(int ibx = bxFirst; ibx < bxLast+1; ++ibx) {
        
      //std::unique_ptr< std::vector<l1t::HGCalTriggerCell> > trigCellVec (new std::vector<l1t::HGCalTriggerCell>);
      std::vector<l1t::HGCalTriggerCell> trigCellVec;
    
      //loop over HGCalTriggerCellBxCollection for a given bx and put the HGCalTriggerCell objects into a std::vector
      for( std::vector<l1t::HGCalTriggerCell>::const_iterator it = coll.begin(ibx) ; it != coll.end(ibx) ; ++it )
      { 
        trigCellVec.push_back(*it); 
      }
      
      // loop over std::vector<l1t::HGCalTriggerCell>
      for( auto trigCell = trigCellVec.begin(); trigCell != trigCellVec.end(); ++trigCell){
        if( trigCell->hwPt() > 0 )
        {
	
	  HGCalDetId detid(trigCell->detId());
          int subdet = detid.subdetId();
          int cellThickness = 0;
	  
	  if( subdet == HGCEE ){ 
            cellThickness = hgceeTopoHandle_->dddConstants().waferTypeL( (unsigned int)detid.wafer() );
          }
          else if( subdet == HGCHEF ){
            cellThickness = hgchefTopoHandle_->dddConstants().waferTypeL( (unsigned int)detid.wafer() );
          }
          else if( subdet == HGCHEB ){
            edm::LogWarning("DataNotFound") << "ATTENTION: the BH trigger cells are not yet implemented";
          }
	  
	  l1t::HGCalTriggerCell calibratedtriggercell( *trigCell );
          calibration_.calibrateInGeV( calibratedtriggercell, cellThickness );
	   
          triggerCellBe_product_->push_back( 0, calibratedtriggercell );
          
	}
      }
  
    }
    
    /* orphan handles to the collections of trigger-cells, clusters and multiclusters */
    edm::OrphanHandle<l1t::HGCalTriggerCellBxCollection> triggerCellsHandle;
    edm::OrphanHandle<l1t::HGCalClusterBxCollection> clustersHandle;
    edm::OrphanHandle<l1t::HGCalMulticlusterBxCollection> multiclustersHandle;
    
    /* retrieve the orphan handle to the trigger-cells collection and put the collection in the event */
    triggerCellsHandle = evt.put( std::move( triggerCellBe_product_ ), "calibratedTriggerCells");

    /* create a persistent vector of pointers to the trigger-cells */
    edm::PtrVector<l1t::HGCalTriggerCell> triggerCellsPtrs;
    for( unsigned i = 0; i < triggerCellsHandle->size(); ++i ) {
      edm::Ptr<l1t::HGCalTriggerCell> ptr(triggerCellsHandle,i);
      triggerCellsPtrs.push_back(ptr);
    }
    
    /* call to C2d clustering */
    switch(clusteringAlgorithmType_){
      case dRC2d : 
        clustering_.clusterizeDR( triggerCellsPtrs, *cluster_product_);
        break;
      case NNC2d:
        clustering_.clusterizeNN( triggerCellsPtrs, *cluster_product_, *triggerGeometry_ );
        break;
      default:
        // Should not happen, clustering type checked in constructor
        break;
    }
    
    /* retrieve the orphan handle to the clusters collection and put the collection in the event */
    clustersHandle = evt.put( std::move( cluster_product_ ), "cluster2D");

    /* create a persistent vector of pointers to the trigger-cells */
    edm::PtrVector<l1t::HGCalCluster> clustersPtrs;
    for( unsigned i = 0; i < clustersHandle->size(); ++i ) {
        edm::Ptr<l1t::HGCalCluster> ptr(clustersHandle,i);
        clustersPtrs.push_back(ptr);
    }
    
    /* call to multiclustering */
    multiclustering_.clusterize( clustersPtrs, *multicluster_product_ );

    /* retrieve the orphan handle to the multiclusters collection and put the collection in the event */
    multiclustersHandle = evt.put( std::move( multicluster_product_ ), "cluster3D");
        
  }
  virtual void reset() override final 
  { 
    triggerCellBe_product_.reset( new l1t::HGCalTriggerCellBxCollection );
    cluster_product_.reset( new l1t::HGCalClusterBxCollection );
    multicluster_product_.reset( new l1t::HGCalMulticlusterBxCollection );
  }
  
  virtual void setGeometry(const HGCalTriggerGeometryBase* const geom) override final {
      HGCalBackendProcessorBase::setGeometry(geom);
  }

  
  void putInEvent(edm::Event& evt)
  { 
    //evt.put(std::move(triggerCellBe_product_), "calibratedTriggerCells");
    //evt.put(std::move(cluster_product_), "cluster2D");
    //evt.put(std::move(multicluster_product_), "cluster3D");
  }
   
   
 private:
 
  enum ClusterType{
            dRC2d,
            NNC2d
  };

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
  HGCalTriggerCellCalibration calibration_;
  HGCalClusteringImpl clustering_;
  HGCalMulticlusteringImpl multiclustering_;
  
  /* algorithm type */
  ClusterType clusteringAlgorithmType_;

};

DEFINE_EDM_PLUGIN(HGCalBackendFactory, 
        HGCalBackendProcessor,
        "HGCalBackendProcessor");



