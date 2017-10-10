#ifndef __L1Trigger_L1THGCal_HGCalVFEProcessor_h__
#define __L1Trigger_L1THGCal_HGCalVFEProcessor_h__

#include "L1Trigger/L1THGCal/interface/HGCalVFEProcessorBase.h"

#include "L1Trigger/L1THGCal/interface/veryfrontend/HGCalVFELinearizationImpl.h"
#include "L1Trigger/L1THGCal/interface/veryfrontend/HGCalVFESummationImpl.h"

#include "DataFormats/L1THGCal/interface/HGCalTriggerCell.h"
#include "DataFormats/L1THGCal/interface/HGCalTriggerSums.h"

#include "DataFormats/L1THGCal/interface/HGCalCluster.h"


class HGCalVFEProcessor : public HGCalVFEProcessorBase
{
    
  public:
    
    HGCalVFEProcessor(const edm::ParameterSet& conf);
    
    void vfeProcessing(const HGCEEDigiCollection& ee,
                	   const HGCHEDigiCollection& fh,
                	   const HGCHEDigiCollection& bh );
    
    void putInEvent(edm::Event& evt);
    
    virtual void setProduces(edm::stream::EDProducer<>& prod) const override final 
    { 
      prod.produces<l1t::HGCalTriggerCellBxCollection>(name());
      prod.produces<l1t::HGCalTriggerSumsBxCollection>(name());
    }
    
    virtual void reset() override final 
    { 
      triggerCell_product_.reset( new l1t::HGCalTriggerCellBxCollection );
      triggerSums_product_.reset( new l1t::HGCalTriggerSumsBxCollection );
    }
    
    typedef std::unique_ptr<HGCalVFEProcessorBase> vfeProcessing_ptr;
    
  private:
    std::vector<vfeProcessing_ptr> vfeProcessing_;
    double   adcLSB_;
    double   linLSB_;
    double   adcsaturation_;
    uint32_t tdcnBits_ ;
    double   tdcOnsetfC_;
    uint32_t adcnBits_;
    double   tdcsaturation_ ;
    double tdcLSB_;
    HGCalVFELinearizationImpl vfeLinearizationImpl_;
    HGCalVFESummationImpl vfeSummationImpl_; 
    std::map<HGCalDetId, uint32_t> payload_;
    
    std::unique_ptr<l1t::HGCalTriggerCellBxCollection> triggerCell_product_;
    std::unique_ptr<l1t::HGCalTriggerSumsBxCollection> triggerSums_product_;
    
};    
    
#endif
