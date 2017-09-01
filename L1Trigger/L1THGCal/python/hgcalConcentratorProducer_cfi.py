import FWCore.ParameterSet.Config as cms

conc_proc = cms.PSet( ConcProcessorName  = cms.string('HGCalConcentratorProcessor'),
			MaxCellsInModule = cms.uint32(116),
			NData = cms.uint32(999),
			linLSB = cms.double(100./1024.),
			TCThreshold_fC = cms.double(1.))

hgcalConcentratorProducer = cms.EDProducer(
    "HGCalConcentratorProducer",
    bxCollection = cms.InputTag('hgcalVFEProducer:HGCalVFEProcessor'),
    Concentratorparam = conc_proc.clone()
    )
