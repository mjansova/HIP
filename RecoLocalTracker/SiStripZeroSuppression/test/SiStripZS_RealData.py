import FWCore.ParameterSet.Config as cms

process = cms.Process('TEST1')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
print "l1"
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
print "l2"
process.load('FWCore.MessageService.MessageLogger_cfi')
print "l3"
process.load('Configuration.StandardSequences.GeometryDB_cff')
print "l4"
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
print "l5"
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
#process.load('CondCore.CondDB.CondDB_cfi.py')
print "l6"
process.load('Configuration.StandardSequences.L1Reco_cff')
print "l7"
process.load('Configuration.StandardSequences.ReconstructionHeavyIons_cff')
print "l8"
process.load('Configuration.StandardSequences.EndOfProcess_cff')
print "l9"
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
print "l10"
process.load('Configuration.EventContent.EventContentHeavyIons_cff')
print "l11"

process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('hiRecoDM nevts:2'),
    name = cms.untracked.string('PyReleaseValidation')
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(3)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
    #'file:/data/abaty/VirginRaw_CentralitySkims/VirginRAW_2010_HICorePhysics_SKIM_Cent_0_5_1.root',
    #'file:/opt/sbg/cms/ui6_data1/mjansova/RD2/streamerToRAWVR.root'
    'file:/opt/sbg/cms/ui6_data1/mjansova/VRpp/082F80A7-2818-E611-92E0-02163E01338D.root',
    #'file:/opt/sbg/cms/ui6_data1/mjansova/VRpp2509/122ACFA9-D582-E611-A16F-FA163ED18503.root',
    #'root://cms-xrd-global.cern.ch//store/data/Run2016B/VRZeroBias5/RAW/v2/000/273/162/00000/30ACD44B-1718-E611-8E70-02163E013392.root', #secon version
    #'file:/data/abaty/VirginRaw_CentralitySkims/VirginRAW_2010_HICorePhysics_SKIM_Cent_0_5_10.root',
    #'file:/opt/sbg/cms/ui6_data1/mjansova/VRpp/1039BBE5-2918-E611-9E9D-02163E0146A2.root', #new root file!
    #'root://cmsxrootd.fnal.gov//store/data/Run2016B/VRRandom0/RAW/v2/000/273/162/00000/8C92C454-2718-E611-9F2F-02163E01355D.root', #Olivier
   )
)

#process.source = cms.Source("NewEventStreamFileReader",
#fileNames = cms.untracked.vstring('root://eoscms.cern.ch//store/t0streamer/Data/HIPhysicsVirginRaw/000/262/296/run262296_ls0223_streamHIPhysicsVirginRaw_StorageManager.dat'
#)
#)

# Output definition
process.RECOoutput = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
    fileName = cms.untracked.string('RECO.root'),
    dataset = cms.untracked.PSet(
        filterName = cms.untracked.string(''),
        dataTier = cms.untracked.string('GEN-SIM-RECO')
    )
)


process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = '80X_dataRun2_HLT_v12'

## Offline Silicon Tracker Zero Suppression
from RecoLocalTracker.SiStripZeroSuppression.DefaultAlgorithms_cff import *
process.siStripZeroSuppression.Algorithms.PedestalSubtractionFedMode = cms.bool(False) #what FED does
process.siStripZeroSuppression.Algorithms.CommonModeNoiseSubtractionMode = cms.string("Median")
process.siStripZeroSuppression.doAPVRestore = cms.bool(True)
process.siStripZeroSuppression.produceRawDigis = cms.bool(True)
process.siStripZeroSuppression.produceCalculatedBaseline = cms.bool(True)
process.siStripZeroSuppression.storeCM = cms.bool(True)
process.siStripZeroSuppression.storeInZScollBadAPV = cms.bool(True)


process.TFileService = cms.Service("TFileService",
        fileName=cms.string("BaselinesForVisualisation.root"))

process.baselineAna = cms.EDAnalyzer("SiStripBaselineAnalyzer",
        Algorithms = DefaultAlgorithms,
        srcBaseline =  cms.InputTag('siStripZeroSuppression','BADAPVBASELINEVirginRaw'),
        srcBaselinePoints =  cms.InputTag('siStripZeroSuppression','BADAPVBASELINEPOINTSVirginRaw'),
        srcAPVCM  =  cms.InputTag('siStripZeroSuppression','APVCMVirginRaw'),
        #srcProcessedRawDigi =  cms.InputTag('siStripZeroSuppression','VirginRaw'),
        srcProcessedRawDigi =  cms.InputTag('siStripDigis','VirginRaw'),
        #srcZSdigi = cms.InputTag('siStripZeroSuppression','VirginRaw'),
        srcZSdigi = cms.InputTag('siStripZeroSuppression','ZeroSuppressedNBandTVirginRaw'), #@MJ@ TODO new ZS -exchanged!!!
        nModuletoDisplay = cms.uint32(3000000),
        hitStripThreshold = cms.uint32(40), #taken from default algorithms
        plotClusters = cms.bool(True),
        plotBaseline = cms.bool(True),
        plotBaselinePoints = cms.bool(True),
        plotRawDigi     = cms.bool(True),
        plotAPVCM   = cms.bool(True),
        plotPedestals = cms.bool(False)
)
    
								  
# Path and EndPath definitions
process.raw2digi_step = cms.Path(process.siStripDigis)
process.reconstruction_step = cms.Path(process.striptrackerlocalreco+process.baselineAna)
process.reconstruction_step2 = cms.Path(process.striptrackerlocalreco+process.reconstructionHeavyIons+process.baselineAna)
#process.endjob_step = cms.EndPath(process.endOfProcess)
process.RECOoutput_step = cms.EndPath(process.RECOoutput)

# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.reconstruction_step, process.RECOoutput_step)

