import FWCore.ParameterSet.Config as cms

from L1Trigger.L1THGCal.hgcalTriggerGeometryESProducer_cfi import *
from Validation.HGCalTriggerPrimitives.hgcalValidationTPG_cfi import *

runHGCALlValidationTPG = cms.Sequence(hgcalTrigPrimValidation)
