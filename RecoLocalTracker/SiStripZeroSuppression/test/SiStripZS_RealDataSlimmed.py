import FWCore.ParameterSet.Config as cms

process = cms.Process('TEST1')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
#process.load('CondCore.CondDB.CondDB_cfi.py')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.ReconstructionHeavyIons_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Configuration.EventContent.EventContentHeavyIons_cff')

process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('hiRecoDM nevts:2'),
    name = cms.untracked.string('PyReleaseValidation')
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(2)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
    #'file:/data/abaty/VirginRaw_CentralitySkims/VirginRAW_2010_HICorePhysics_SKIM_Cent_0_5_1.root',
    #'file:/opt/sbg/cms/ui6_data1/mjansova/RD2/streamerToRAWVR.root'
    #'file:/opt/sbg/cms/ui6_data1/mjansova/VRpp/D830B49F-2518-E611-A300-02163E013925.root'
    #'root://xrootd.unl.edu//store/data/Run2016B/VRRandom4/RAW/v2/000/273/162/00000/082F80A7-2818-E611-92E0-02163E01338D.root',
    #'file:/data/abaty/VirginRaw_CentralitySkims/VirginRAW_2010_HICorePhysics_SKIM_Cent_0_5_10.root',
    #'file:/opt/sbg/cms/ui6_data1/mjansova/VRpp/1039BBE5-2918-E611-9E9D-02163E0146A2.root', #pp virgin raw stored
    'file:/opt/sbg/cms/ui6_data1/mjansova/VRpp2509/122ACFA9-D582-E611-A16F-FA163ED18503.root', #pp virgin raw 2 stored
    #'root://xrootd.unl.edu//store/data/Run2016B/VRRandom0/RAW/v2/000/273/161/00000/CAC76541-3F18-E611-BB31-02163E01284D.root' #empty file
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
process.GlobalTag.globaltag = '80X_dataRun2_Prompt_v9'
#process.GlobalTag.globaltag = '80X_dataRun2_HLT_v15'

## Offline Silicon Tracker Zero Suppression
from RecoLocalTracker.SiStripZeroSuppression.DefaultAlgorithms_cff import *
process.siStripZeroSuppression.Algorithms.PedestalSubtractionFedMode = cms.bool(False)
process.siStripZeroSuppression.Algorithms.CommonModeNoiseSubtractionMode = cms.string("Median")
process.siStripZeroSuppression.doAPVRestore = cms.bool(True)
process.siStripZeroSuppression.produceRawDigis = cms.bool(True)
process.siStripZeroSuppression.produceCalculatedBaseline = cms.bool(True)
process.siStripZeroSuppression.storeCM = cms.bool(True)
process.siStripZeroSuppression.storeInZScollBadAPV = cms.bool(True)


process.TFileService = cms.Service("TFileService",
        fileName=cms.string("treeTest.root"))

process.baselineAna = cms.EDAnalyzer("SiStripBaselineAnalyzerSlim2",
        Algorithms = DefaultAlgorithms,
        srcBaseline =  cms.InputTag('siStripZeroSuppression','BADAPVBASELINEVirginRaw'),
        srcBaselinePoints =  cms.InputTag('siStripZeroSuppression','BADAPVBASELINEPOINTSVirginRaw'),
        srcAPVCM  =  cms.InputTag('siStripZeroSuppression','APVCMVirginRaw'),
        #srcProcessedRawDigi =  cms.InputTag('siStripZeroSuppression','VirginRaw'),
        srcProcessedRawDigi =  cms.InputTag('siStripDigis','VirginRaw'),
        srcZSdigi = cms.InputTag('siStripZeroSuppression','VirginRaw'),
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
#process.endjob_step = cms.EndPath(process.endOfProcess)
process.RECOoutput_step = cms.EndPath(process.RECOoutput)

# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.reconstruction_step, process.RECOoutput_step)

