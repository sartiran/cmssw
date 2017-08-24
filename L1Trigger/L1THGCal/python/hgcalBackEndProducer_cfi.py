import FWCore.ParameterSet.Config as cms

be_proc = cms.PSet( BeProcessorName  = cms.string('HGCalBackendProcessor')
                   )


hgcalBackEndProducer = cms.EDProducer(
    "HGCalBackendProducer",
    bxCollection_be = cms.InputTag('hgcalConcentratorProducer:HGCalConcentratorProcessor'),
    Backendparam = be_proc.clone()
    )
