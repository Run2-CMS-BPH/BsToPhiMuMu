import FWCore.ParameterSet.Config as cms

process = cms.Process("Ntuple")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.options.allowUnscheduled = cms.untracked.bool(True)


#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )

process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
#from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag

#process.load('Configuration.StandardSequences.MagneticField_cff')
#process.load('Configuration.StandardSequences.GeometryExtended_cff')
#process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
#process.load('Configuration.Geometry.GeometryIdeal_cff') 
#process.load("PhysicsTools.PatAlgos.patSequences_cff")

# make patCandidates, select and clean them
process.load('PhysicsTools.PatAlgos.producersLayer1.patCandidates_cff')
process.load('PhysicsTools.PatAlgos.selectionLayer1.selectedPatCandidates_cff')
process.load('PhysicsTools.PatAlgos.cleaningLayer1.cleanPatCandidates_cff')
#process.patMuons.embedTrack  = True

#make patTracks
from PhysicsTools.PatAlgos.tools.trackTools import makeTrackCandidates
makeTrackCandidates(process,
                    label        = 'TrackCands',                  # output collection
                    tracks       = cms.InputTag('generalTracks'), # input track collection
                   ## particleType = 'pi+',                         # particle type (for assigning a mass)
                    particleType = 'K+',                         # particle type (for assigning a mass)
                    preselection = 'pt > 0.4',                    # preselection cut on candidates
                    selection    = 'pt > 0.4',                    # selection on PAT Layer 1 objects
                    isolation    = {},                            # isolations to use (set to {} for None)
                    isoDeposits  = [],
                    mcAs         = None                           # replicate MC match as the one used for Muons
                    )



process.load('PhysicsTools.PatAlgos.slimming.genParticles_cff')
process.packedGenParticles.inputVertices = cms.InputTag('offlinePrimaryVertices')


from PhysicsTools.PatAlgos.tools.coreTools import runOnData
runOnData( process, outputModules = [] )

process.ntuple = cms.EDAnalyzer(
    'BsToPhiMuMu',

    OutputFileName = cms.string("BsToPhiMuMu_2016.root"),
    BuildBsToPhiMuMu = cms.untracked.bool(True), 

    MuonMass = cms.untracked.double(0.10565837), 
    MuonMassErr = cms.untracked.double(3.5e-9),   
    KaonMass = cms.untracked.double(0.493677), 
    KaonMassErr = cms.untracked.double(1.6e-5),
    BsMass = cms.untracked.double(5.36677),          ## put the Bs Mass (pdg value)

    # labels
    GenParticlesLabel = cms.InputTag("genParticles"),
    TriggerResultsLabel = cms.InputTag("TriggerResults","", 'HLT'),
    BeamSpotLabel = cms.InputTag('offlineBeamSpot'),
    VertexLabel = cms.InputTag('offlinePrimaryVertices'),
    MuonLabel = cms.InputTag('cleanPatMuonsTriggerMatch'),
    TrackLabel = cms.InputTag('cleanPatTrackCands'), 
    TriggerNames = cms.vstring([]),
    LastFilterNames = cms.vstring([]),

    # gen particle 
    IsMonteCarlo = cms.untracked.bool(False),
    KeepGENOnly  = cms.untracked.bool(False),
    TruthMatchMuonMaxR = cms.untracked.double(0.004), # [eta-phi]
    TruthMatchKaonMaxR = cms.untracked.double(0.3), # [eta-phi]

    # check confdb for details
    MuonMinPt = cms.untracked.double(4.0), # 3.0 [GeV]
    MuonMaxEta = cms.untracked.double(2.2),  
    MuonMaxDcaBs = cms.untracked.double(2.0), # [cm]

    MuMuMinPt = cms.untracked.double(6.9),      # [GeV/c]
##    MuMuMinInvMass = cms.untracked.double(1.0), # [GeV/c2]
##    MuMuMaxInvMass = cms.untracked.double(4.8), # [GeV/c2]
 
    MuMuMinInvMass1 = cms.untracked.double(2.9), # [GeV/c2]
    MuMuMinInvMass2 = cms.untracked.double(2.9), # [GeV/c2]   
    MuMuMaxInvMass1 = cms.untracked.double(3.3), # [GeV/c2]
    MuMuMaxInvMass2 = cms.untracked.double(3.3), # [GeV/c2]


    MuMuMinVtxCl = cms.untracked.double(0.10), # 0.05
    MuMuMinLxySigmaBs = cms.untracked.double(3.0), 
    MuMuMaxDca = cms.untracked.double(0.5), # [cm]
    MuMuMinCosAlphaBs = cms.untracked.double(0.9),

    # pre-selection cuts 
    TrkMinPt = cms.untracked.double(0.8), # 0.4 [GeV/c]
    TrkMinDcaSigBs = cms.untracked.double(0.8), # 0.8 hadron DCA/sigma w/respect to BS (=>changed Max to Min)
    TrkMaxR = cms.untracked.double(110.0), # [cm] ==> size of tracker volume in radial direction
    TrkMaxZ = cms.untracked.double(280.0), # [cm] ==> size of tracker volume in Z direction

    ## phi(1020) mass = 1019.461 +/- 0.019 MeV, full width = 4.266 +/- 0.031 MeV
    PhiMinMass = cms.untracked.double(1.00), # [GeV/c2]  - 3 sigma of the width(~5MeV)
    PhiMaxMass = cms.untracked.double(1.04), # [GeV/c2]  + 3 sigma of the width

    BsMinVtxCl = cms.untracked.double(0.01), 
    BsMinMass = cms.untracked.double(4.7), # [GeV/c2] 
    BsMaxMass = cms.untracked.double(6.0), # [GeV/c2]  

)

##from PhysicsTools.PatAlgos.tools.coreTools import runOnData
##runOnData( process, outputModules = [] )

##process.TFileService = cms.Service("TFileService",
##        fileName = cms.string('BsToPhiMuMu_2016.root'),
##)

process.p = cms.Path(process.ntuple)
