#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/L1THGCal/interface/HGCalTriggerCell.h"
#include "DataFormats/L1THGCal/interface/HGCalTriggerSums.h"
#include "DataFormats/HGCDigi/interface/HGCDigiCollections.h"

#include "L1Trigger/L1THGCal/interface/HGCalTriggerGeometryBase.h"


#include <sstream>
#include <memory>


class HGCalVFEProducer : public edm::EDProducer  {  
 public:    
  HGCalVFEProducer(const edm::ParameterSet&);
  ~HGCalVFEProducer() { }
  
  virtual void beginRun(const edm::Run&, 
                        const edm::EventSetup&);
  virtual void produce(edm::Event&, const edm::EventSetup&);
  
 private:
  // inputs
  edm::EDGetToken inputee_, inputfh_;
  edm::ESHandle<HGCalTriggerGeometryBase> triggerGeometry_;
  
};

DEFINE_FWK_MODULE(HGCalVFEProducer);

HGCalVFEProducer::
HGCalVFEProducer(const edm::ParameterSet& conf):
  inputee_(consumes<HGCEEDigiCollection>(conf.getParameter<edm::InputTag>("eeDigis"))),
  inputfh_(consumes<HGCHEDigiCollection>(conf.getParameter<edm::InputTag>("fhDigis"))) 
{ //std::cout << " ### HGCalVFEProducer::HGCalVFEProducer" << std::endl;
  
  //setup VFE parameters
  const edm::ParameterSet& vfeParamConfig = conf.getParameterSet("VFEparam");
  const std::string& vfeProcessorName = vfeParamConfig.getParameter<std::string>("VFEProcessorName");
  
  produces<l1t::HGCalTriggerCellBxCollection>("bxCollection");
  produces<l1t::HGCalTriggerSumsBxCollection>("bxCollection");
  

  //std::cout << " ### after creating l1t::HGCalTriggerCellBxCollection" << std::endl;
}

void HGCalVFEProducer::beginRun(const edm::Run& /*run*/, 
                                          const edm::EventSetup& es) {
					  
  //std::cout << " ### HGCalVFEProducer::beginRun" << std::endl;					  
  es.get<IdealGeometryRecord>().get(triggerGeometry_);
}

void HGCalVFEProducer::produce(edm::Event& e, const edm::EventSetup& es) {
  //std::cout << " ### HGCalVFEProducer::produce" << std::endl;
  
  std::unique_ptr<l1t::HGCalTriggerCellBxCollection> 
    		fe_output_trig_cell( new l1t::HGCalTriggerCellBxCollection );
 
  std::unique_ptr<l1t::HGCalTriggerSumsBxCollection> 
    		fe_output_trig_sums( new l1t::HGCalTriggerSumsBxCollection );
  
  
 
  e.put(std::move(fe_output_trig_cell), "bxCollection");
  e.put(std::move(fe_output_trig_sums), "bxCollection");
   
}
