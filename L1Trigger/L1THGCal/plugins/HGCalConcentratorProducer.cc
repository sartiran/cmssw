#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/L1THGCal/interface/HGCalTriggerCell.h"
#include "DataFormats/L1THGCal/interface/HGCalTriggerSums.h"
#include "DataFormats/HGCDigi/interface/HGCDigiCollections.h"

#include "L1Trigger/L1THGCal/interface/HGCalTriggerGeometryBase.h"

#include "L1Trigger/L1THGCal/interface/HGCalConcentratorProcessorBase.h"

#include <sstream>
#include <memory>


class HGCalConcentratorProducer : public edm::stream::EDProducer<> {  
 public:    
  HGCalConcentratorProducer(const edm::ParameterSet&);
  ~HGCalConcentratorProducer() { }
  
  virtual void beginRun(const edm::Run&, 
                        const edm::EventSetup&);
  virtual void produce(edm::Event&, const edm::EventSetup&);
  
 private:
  // inputs
  edm::EDGetToken input_cell_, input_sums_;
  edm::ESHandle<HGCalTriggerGeometryBase> triggerGeometry_;
  std::string choice_;
  
  std::unique_ptr<HGCalConcentratorProcessorBase> concentratorProcess_;
};

DEFINE_FWK_MODULE(HGCalConcentratorProducer);

HGCalConcentratorProducer::
HGCalConcentratorProducer(const edm::ParameterSet& conf):
  input_cell_(consumes<l1t::HGCalTriggerCellBxCollection>(conf.getParameter<edm::InputTag>("bxCollection"))),
  input_sums_(consumes<l1t::HGCalTriggerSumsBxCollection>(conf.getParameter<edm::InputTag>("bxCollection")))
{
  //setup Concentrator parameters
  const edm::ParameterSet& concParamConfig = conf.getParameterSet("Concentratorparam");
  const std::string& concProcessorName = concParamConfig.getParameter<std::string>("ConcProcessorName");
  choice_ = concParamConfig.getParameter<std::string>("Method");
  HGCalConcentratorProcessorBase* concProc = HGCalConcentratorFactory::get()->create(concProcessorName, concParamConfig);
  concentratorProcess_.reset(concProc);

  concentratorProcess_->setProduces(*this);

}

void HGCalConcentratorProducer::beginRun(const edm::Run& /*run*/, 
                                          const edm::EventSetup& es) {
  es.get<IdealGeometryRecord>().get(triggerGeometry_);
  
  concentratorProcess_->setGeometry(triggerGeometry_.product());

}

void HGCalConcentratorProducer::produce(edm::Event& e, const edm::EventSetup& es) {
  
  edm::Handle<l1t::HGCalTriggerCellBxCollection> trigCellBxColl;
  edm::Handle<l1t::HGCalTriggerSumsBxCollection> trigSumsBxColl;

  e.getByToken(input_cell_,trigCellBxColl);
  e.getByToken(input_sums_,trigSumsBxColl);

  const l1t::HGCalTriggerCellBxCollection& trigCell = *trigCellBxColl;
  //const l1t::HGCalTriggerSumsBxCollection& trigSums = *trigSumsBxColl;
  		
  concentratorProcess_->reset();
  if (choice_ == "bestChoiceSelect"){
    concentratorProcess_->bestChoiceSelect(trigCell);			
  }
  else if (choice_ == "thresholdSelect"){
    concentratorProcess_->thresholdSelect(trigCell);
  }
  concentratorProcess_->putInEvent(e);
   
}
