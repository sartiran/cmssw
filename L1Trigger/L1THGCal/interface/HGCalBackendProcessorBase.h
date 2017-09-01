#ifndef __L1Trigger_L1THGCal_HGCalBackendProcessorBase_h__
#define __L1Trigger_L1THGCal_HGCalBackendProcessorBase_h__

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "DataFormats/L1THGCal/interface/HGCalTriggerCell.h"

#include "L1Trigger/L1THGCal/interface/HGCalTriggerGeometryBase.h"

class HGCalBackendProcessorBase { 
  
 public:
  HGCalBackendProcessorBase(const edm::ParameterSet& conf) : 
    geometry_(nullptr),
    name_(conf.getParameter<std::string>("BeProcessorName"))
    {}
  virtual ~HGCalBackendProcessorBase() {}

  const std::string& name() const { return name_; } 
 
  virtual void setProduces(edm::stream::EDProducer<>& prod) const = 0;
  virtual void putInEvent(edm::Event& evt) = 0;
  virtual void run(const l1t::HGCalTriggerCellBxCollection& coll, 
		   const edm::EventSetup& es,
		   edm::Event &e) = 0;
		   
  virtual void reset() = 0;

 protected:
  const HGCalTriggerGeometryBase* geometry_; 
  
 private:
  const std::string name_;

};

#include "FWCore/PluginManager/interface/PluginFactory.h"
typedef edmplugin::PluginFactory< HGCalBackendProcessorBase* (const edm::ParameterSet&) > HGCalBackendFactory;

#endif

