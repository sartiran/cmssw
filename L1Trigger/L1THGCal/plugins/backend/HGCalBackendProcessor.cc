#include "L1Trigger/L1THGCal/interface/backend/HGCalBackendProcessor.h"

#include "L1Trigger/L1THGCal/interface/be_algorithms/HGCalClusteringImpl.h"
#include "L1Trigger/L1THGCal/interface/be_algorithms/HGCalMulticlusteringImpl.h" 

#include <limits>

DEFINE_EDM_PLUGIN(HGCalBackendFactory, 
        HGCalBackendProcessor,
        "HGCalBackendProcessor");

HGCalBackendProcessor::HGCalBackendProcessor(const edm::ParameterSet& conf)  : HGCalBackendProcessorBase(conf)
{
}

void HGCalBackendProcessor::putInEvent(edm::Event& evt)
{ 
  evt.put(std::move(triggerCellBe_product_), "calibratedTriggerCells");
  evt.put(std::move(cluster_product_), "cluster2D");
  evt.put(std::move(multicluster_product_), "cluster3D");
}
