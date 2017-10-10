#include "L1Trigger/L1THGCal/interface/veryfrontend/HGCalVFESummationImpl.h"

HGCalVFESummationImpl::
HGCalVFESummationImpl(const edm::ParameterSet& conf)
{}

void 
HGCalVFESummationImpl::
triggerCellSums(const HGCalTriggerGeometryBase& geometry, 
		const std::vector<std::pair<HGCalDetId, uint32_t > >& linearized_dataframes,
		std::map<HGCalDetId, uint32_t>& payload		
		)
{   
    if(linearized_dataframes.size()==0) return;
    
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
            	
}
