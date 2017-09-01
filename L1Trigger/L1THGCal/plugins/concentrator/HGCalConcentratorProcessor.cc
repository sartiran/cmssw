#include "L1Trigger/L1THGCal/interface/concentrator/HGCalConcentratorProcessor.h"
#include <limits>

#include "DataFormats/HGCDigi/interface/HGCDigiCollections.h"

DEFINE_EDM_PLUGIN(HGCalConcentratorFactory, 
        HGCalConcentratorProcessor,
        "HGCalConcentratorProcessor");

HGCalConcentratorProcessor::HGCalConcentratorProcessor(const edm::ParameterSet& conf)  : 
	HGCalConcentratorProcessorBase(conf), 
	nData_(conf.getParameter<uint32_t>("NData")),
	nCellsInModule_(conf.getParameter<uint32_t>("MaxCellsInModule")),
	linLSB_(conf.getParameter<double>("linLSB")),
	TCThreshold_fC_(conf.getParameter<double>("TCThreshold_fC"))
{ 
  // Cannot have more selected cells than the max number of cells
  if(nData_>nCellsInModule_) nData_ = nCellsInModule_;
  TCThreshold_ADC_ = (int) (TCThreshold_fC_ / linLSB_);

}

void HGCalConcentratorProcessor::putInEvent(edm::Event& evt)
{ 
  evt.put(std::move(triggerCellConc_product_), name());
  evt.put(std::move(triggerSumsConc_product_), name());
}

void HGCalConcentratorProcessor::bestChoiceSelect(const l1t::HGCalTriggerCellBxCollection& coll)
{   
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
           
    // sort, reverse order
    sort(trigCellVec.begin(), trigCellVec.end(),
            	[](const l1t::HGCalTriggerCell& a, 
                 const  l1t::HGCalTriggerCell& b) -> bool
    { 
      return a.hwPt() > b.hwPt(); 
    } 
    );        
    
    // keep only the first trigger cells
    if(trigCellVec.size()>nData_) trigCellVec.resize(nData_);
    
    // Transform std::vector<l1t::HGCalTriggerCell> to BXVector<HGCalTriggerCell> HGCalTriggerCellBxCollection
    for( auto trigCell = trigCellVec.begin(); trigCell != trigCellVec.end(); ++trigCell)
      triggerCellConc_product_->push_back(ibx, *trigCell);

  }

}

void 
HGCalConcentratorProcessor::
thresholdSelect(const l1t::HGCalTriggerCellBxCollection& coll)
{  
  int bxFirst = coll.getFirstBX();
  int bxLast = coll.getLastBX();
    
  // loop over BX
  for(int ibx = bxFirst; ibx < bxLast+1; ++ibx) {
  
    std::vector<l1t::HGCalTriggerCell> trigCellVec;
    
    //loop over HGCalTriggerCellBxCollection for a given bx and put the HGCalTriggerCell objects into a std::vector
    for( std::vector<l1t::HGCalTriggerCell>::const_iterator it = coll.begin(ibx) ; it != coll.end(ibx) ; ++it )
    { 
      trigCellVec.push_back(*it); 
    }

    for (size_t i = 0; i<trigCellVec.size();i++){
      if (trigCellVec[i].hwPt() < TCThreshold_ADC_)  trigCellVec[i].setHwPt(0);
    }
  
    // Transform std::vector<l1t::HGCalTriggerCell> to BXVector<HGCalTriggerCell> HGCalTriggerCellBxCollection
    for( auto trigCell = trigCellVec.begin(); trigCell != trigCellVec.end(); ++trigCell)
      triggerCellConc_product_->push_back(ibx, *trigCell);
  
  }
  
}
