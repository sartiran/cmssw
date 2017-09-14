#include "L1Trigger/L1THGCal/interface/veryfrontend/HGCalVFEProcessor.h"
#include <limits>

#include "DataFormats/Candidate/interface/LeafCandidate.h"
#include "DataFormats/L1THGCal/interface/HGCalTriggerCell.h"

DEFINE_EDM_PLUGIN(HGCalVFEProcessorBaseFactory, 
        HGCalVFEProcessor,
        "HGCalVFEProcessor");


HGCalVFEProcessor::
HGCalVFEProcessor(const edm::ParameterSet& conf) : HGCalVFEProcessorBase(conf),
    linLSB_(conf.getParameter<double>("linLSB")),
    adcsaturation_(conf.getParameter<double>("adcsaturation")),
    tdcnBits_(conf.getParameter<uint32_t>("tdcnBits")),
    tdcOnsetfC_(conf.getParameter<double>("tdcOnsetfC")),
    adcnBits_(conf.getParameter<uint32_t>("adcnBits")),
    tdcsaturation_(conf.getParameter<double>("tdcsaturation")),
    vfeLinearizationImpl_(conf),
    vfeSummationImpl_(conf)
{ 
  adcLSB_ = adcsaturation_/pow(2.,adcnBits_);
  tdcLSB_ =  tdcsaturation_/pow(2.,tdcnBits_);
}

void
HGCalVFEProcessor::
vfeProcessing(const HGCEEDigiCollection& ee,
        		const HGCHEDigiCollection& fh,
        		const HGCHEDigiCollection& ) 
{   
  std::vector<HGCDataFrame<HGCalDetId,HGCSample>> dataframes;
  std::vector<std::pair<HGCalDetId, uint32_t > > linearized_dataframes;
  std::map<HGCalDetId, uint32_t> payload;
  
  // convert ee and fh hit collections into the same object  
  if(ee.size()>0)
  { 
    for(const auto& eedata : ee)
    {
      dataframes.emplace_back(eedata.id());
      for(int i=0; i<eedata.size(); i++)
      {
        dataframes.back().setSample(i, eedata.sample(i));
      }
    }
  }

  vfeLinearizationImpl_.linearize(dataframes, linearized_dataframes);
  vfeSummationImpl_.triggerCellSums(*geometry_, linearized_dataframes, payload);  
  
  l1t::HGCalTriggerCell triggerCell(reco::LeafCandidate::LorentzVector(), 0, 0, 0, 0, 0);
  triggerCell_product_->push_back(0, triggerCell);
  
}


void HGCalVFEProcessor::putInEvent(edm::Event& evt)
{ 
  evt.put(std::move(triggerCell_product_), name());
  evt.put(std::move(triggerSums_product_), name());
}

