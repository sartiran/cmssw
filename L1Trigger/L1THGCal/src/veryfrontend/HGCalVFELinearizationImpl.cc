#include "L1Trigger/L1THGCal/interface/veryfrontend/HGCalVFELinearizationImpl.h"

HGCalVFELinearizationImpl::
HGCalVFELinearizationImpl(const edm::ParameterSet& conf):
    linLSB_(conf.getParameter<double>("linLSB")),
    adcsaturation_(conf.getParameter<double>("adcsaturation")),
    tdcnBits_(conf.getParameter<uint32_t>("tdcnBits")),
    tdcOnsetfC_(conf.getParameter<double>("tdcOnsetfC")),
    adcnBits_(conf.getParameter<uint32_t>("adcnBits")),
    tdcsaturation_(conf.getParameter<double>("tdcsaturation"))
{
  adcLSB_ = adcsaturation_/pow(2.,adcnBits_);
  tdcLSB_ =  tdcsaturation_/pow(2.,tdcnBits_);
}

void
HGCalVFELinearizationImpl::
linearize(const std::vector<HGCDataFrame<HGCalDetId,HGCSample>>& dataframes,
          std::vector<std::pair<HGCalDetId, uint32_t > >& linearized_dataframes)
{        
    double amplitude; uint32_t amplitude_int;

    for(const auto& frame : dataframes) {//loop on DIGI
      
      if (frame[2].mode()) {//TOT mode
        amplitude =( floor(tdcOnsetfC_/adcLSB_) + 1.0 )* adcLSB_ + double(frame[2].data()) * tdcLSB_;
      }
      else {//ADC mode
        amplitude = double(frame[2].data()) * adcLSB_;
      }

      amplitude_int = uint32_t (floor(amplitude/linLSB_+0.5));  
      if (amplitude_int>65535) amplitude_int = 65535;
            
      linearized_dataframes.push_back(std::make_pair (frame.id(), amplitude_int));            
    }
    
}
