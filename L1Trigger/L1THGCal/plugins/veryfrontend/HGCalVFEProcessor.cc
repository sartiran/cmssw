#include "L1Trigger/L1THGCal/interface/veryfrontend/HGCalVFEProcessor.h"
#include <limits>

#include "DataFormats/Candidate/interface/LeafCandidate.h"


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
    tdcsaturation_(conf.getParameter<double>("tdcsaturation"))
    //vfeLinearizationImpl_(conf),
    //vfeSummationImpl_(conf)
{ 
  adcLSB_ = adcsaturation_/pow(2.,adcnBits_);
  tdcLSB_ =  tdcsaturation_/pow(2.,tdcnBits_);
}


void
HGCalVFEProcessor::
linearize(const std::vector<HGCDataFrame<HGCalDetId,HGCSample>>& dataframes,
          std::vector<std::pair<HGCalDetId, uint32_t > >& linearized_dataframes)
{   
    double amplitude; uint32_t amplitude_int;
    for(const auto& frame : dataframes) {//loop on DIGI
      //std::cout << "double(frame[2].data()) ="<< double(frame[2].data()) << std::endl; 
      if (frame[2].mode()) {//TOT mode
        amplitude =( floor(tdcOnsetfC_/adcLSB_) + 1.0 )* adcLSB_ + double(frame[2].data()) * tdcLSB_;
      }
      else {//ADC mode
        amplitude = double(frame[2].data()) * adcLSB_;
      }

      amplitude_int = uint32_t (floor(amplitude/linLSB_+0.5));  
      if (amplitude_int>65535) amplitude_int = 65535;
      
      //std::cout << " frame.id()= " << frame.id() <<  "amplitude_int ="<<  amplitude_int << std::endl; 
      linearized_dataframes.push_back(std::make_pair (frame.id(), amplitude_int)); 
    }    
}


void 
HGCalVFEProcessor::
triggerCellSums(const HGCalTriggerGeometryBase& geometry,  const std::vector<std::pair<HGCalDetId, uint32_t > >& linearized_dataframes)
{   
    if(linearized_dataframes.size()==0) return;
    std::map<HGCalDetId, uint32_t> payload;
    
    // sum energies in trigger cells
    for(const auto& frame : linearized_dataframes)
    { 
      HGCalDetId cellid(frame.first);
      
      // find trigger cell associated to cell
      uint32_t tcid = geometry.getTriggerCellFromCell(cellid);
      HGCalDetId triggercellid( tcid );
      payload.insert( std::make_pair(triggercellid, 0) ); // do nothing if key exists already
      uint32_t value = frame.second;

      // sums energy for the same triggercellid
      payload[triggercellid] += value; // 32 bits integer should be largely enough 
    }
        
    uint32_t module = geometry.getModuleFromTriggerCell(payload.begin()->first);

    HGCalTriggerGeometryBase::geom_ordered_set trigger_cells_in_module = geometry.getOrderedTriggerCellsFromModule(module);
    
    // fill data payload
    for(const auto& id_value : payload)
    { 
      l1t::HGCalTriggerCell triggerCell(reco::LeafCandidate::LorentzVector(), id_value.second, 0, 0, 0, id_value.first.rawId()); 
      // Store only energy value and detid
      // No need position here
      triggerCell_product_->push_back(0, triggerCell);
    }
}


void
HGCalVFEProcessor::
vfeProcessing(const HGCEEDigiCollection& ee,
        		const HGCHEDigiCollection& fh,
        		const HGCHEDigiCollection& ) 
{   
  std::vector<HGCDataFrame<HGCalDetId,HGCSample>> dataframes;
  std::vector<std::pair<HGCalDetId, uint32_t > > linearized_dataframes;
  
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

  linearize(dataframes, linearized_dataframes);
  triggerCellSums(*geometry_, linearized_dataframes);  
  
}


void HGCalVFEProcessor::putInEvent(edm::Event& evt)
{ 
  evt.put(std::move(triggerCell_product_), name());
  evt.put(std::move(triggerSums_product_), name());
}

