import FWCore.ParameterSet.Config as cms

DefaultAlgorithms = cms.PSet(
    doAPVRestore = cms.bool(True), #@MJ@ TODO doAPVRestore is set in here!
    useCMMeanMap= cms.bool(False),
    ## Pedestal subtraction ----------------
    PedestalSubtractionFedMode = cms.bool(False),

    ## Baseline finder ---------------------
    ## Supported CMN modes: Median, Percentile, IteratedMedian, TT6, FastLinear
    CommonModeNoiseSubtractionMode = cms.string('Median'), #@MJ@ TODO IteratedMedian    

    #CutToAvoidSignal = cms.double(3.0), ## for TT6
    
    #Percentile = cms.double(25.0),      ## for Percentile

    CutToAvoidSignal = cms.double(2.0),  ## for IteratedMedian (needed by APVRestorer)
    Iterations = cms.int32(3),           ##

    ## APV restoration ---------------------
    ## Supported inspect modes: BaselineFollower, AbnormalBaseline, Null, BaselineAndSaturation
    APVInspectMode = cms.string("BaselineFollower"),
    ForceNoRestore = cms.bool(False),
    SelfSelectRestoreAlgo = cms.bool(False),
    useRealMeanCM = cms.bool(False),
    DeltaCMThreshold = cms.uint32(20),       # for BaselineFollower inspect
    distortionThreshold = cms.uint32(20),    # " "
    Fraction = cms.double(0.2),              # for AbnormalBaseline inspect
    Deviation = cms.uint32(25),              # " "
    restoreThreshold = cms.double(0.5),      # for Null inspect
    nSaturatedStrip = cms.uint32(2),         # for BaselineAndSaturation inspect

    ## Supported restore modes: Flat, BaselineFollower, IterativeMedian
    APVRestoreMode = cms.string("Flat"),     #@MJ@ TODO baseline follower
    nSigmaNoiseDerTh = cms.uint32(4),        # threshold for rejecting hit strips: nSigma * noise
    consecThreshold = cms.uint32(5),         # minimum length of flat region
    hitStripThreshold = cms.uint32(40),      # height above median when strip is definitely a hit
    nSmooth = cms.uint32(9),                 # for smoothing and local minimum determination (odd number)
    minStripsToFit = cms.uint32(4),          # minimum strips to try spline algo (otherwise default to median)
    ApplyBaselineCleaner = cms.bool(False),
    CleaningSequence = cms.uint32(1),
    slopeX=cms.int32(3),
    slopeY=cms.int32(4),
    ApplyBaselineRejection = cms.bool(True),
    MeanCM = cms.int32(0),

    #Parameters for bad APV selector
    filteredBaselineMax=cms.double(6),
    filteredBaselineDerivativeSumSquare=cms.double(30),
    
    ## Zero suppression --------------------
    SiStripFedZeroSuppressionMode = cms.uint32(4),
    TruncateInSuppressor = cms.bool(True)
    
    
    )

