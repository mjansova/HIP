import FWCore.ParameterSet.Config as cms

from RecoLocalTracker.SiStripClusterizer.DefaultClusterizer_cff import *

siStripClusters = cms.EDProducer("SiStripClusterizer",
                               Clusterizer = DefaultClusterizer,
                               DigiProducersList = cms.VInputTag(
    #cms.InputTag('siStripDigis','ZeroSuppressed'),
    cms.InputTag('siStripZeroSuppression','VirginRaw')),
    #cms.InputTag('siStripDigis','VirginRaw')),
    #cms.InputTag('siStripZeroSuppression','ProcessedRaw'),
    #cms.InputTag('siStripZeroSuppression','ScopeMode')),
                               
                               )

siStripClusters2 = siStripClusters.clone()
siStripClusters2.Clusterizer.RemoveApvShots    = cms.bool(True)
