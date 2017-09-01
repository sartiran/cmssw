import FWCore.ParameterSet.Config as cms

import SimCalorimetry.HGCalSimProducers.hgcalDigitizer_cfi as digiparam


# Digitization parameters
adcSaturation_fC = digiparam.hgceeDigitizer.digiCfg.feCfg.adcSaturation_fC
adcNbits = digiparam.hgceeDigitizer.digiCfg.feCfg.adcNbits
tdcSaturation_fC = digiparam.hgceeDigitizer.digiCfg.feCfg.tdcSaturation_fC
tdcNbits = digiparam.hgceeDigitizer.digiCfg.feCfg.tdcNbits
tdcOnset_fC = digiparam.hgceeDigitizer.digiCfg.feCfg.tdcOnset_fC

# Parameters used in several places
triggerCellLsbBeforeCompression = 100./1024.
#triggerCellTruncationBits = 0
                    
vfe_proc = cms.PSet( VFEProcessorName  = cms.string('HGCalVFEProcessor'),
		     # Digitization parameters
		     linLSB = cms.double(100./1024.),
		     triggerCellTruncationBits = cms.uint32(7),
		     tdcOnsetfC = tdcOnset_fC,     
		     abcsaturation = adcSaturation_fC,
		     tdcsaturation = digiparam.hgceeDigitizer.digiCfg.feCfg.tdcSaturation_fC,
	             tdcnBits = tdcNbits,		     
		     adcnBits = adcNbits,
		     adcsaturation = adcSaturation_fC
                   )

hgcalVFEProducer = cms.EDProducer(
    	"HGCalVFEProducer",
    	eeDigis = cms.InputTag('mix:HGCDigisEE'),
    	fhDigis = cms.InputTag('mix:HGCDigisHEfront'),
    	VFEparam = vfe_proc.clone()
    )
