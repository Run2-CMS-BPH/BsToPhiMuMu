
print "\n=> running on 2016 data \n"

#####################
#  cmssw configs    #
#####################

import FWCore.ParameterSet.Config as cms
##from bstophimumu_2016_cfi import process 
from bstophimumu_2016_cfi_may8 import process

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )

process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring(
#'/store/data/Run2012A/DoubleMuParked/AOD/22Jan2013-v1/20000/002557DC-16D4-E211-9415-20CF3027A5AB.root')
#'/store/data/Run2012A/MuOnia/AOD/22Jan2013-v1/30000/000D2FF5-EE82-E211-BEBA-0026189438A5.root')
#'/DoubleMuParked/Run2012A-22Jan2013-v1/AOD')
#'/MuOniaParked/Run2012D-22Jan2013-v1/AOD')
#'/store/data/Run2012D/MuOniaParked/AOD/22Jan2013-v1/10000/0009C032-C48D-E211-83FA-003048FEB956.root' )
#'/store/data/Run2016B/Charmonium/AOD/PromptReco-v2/000/273/158/00000/14E579B3-271A-E611-911E-02163E013584.root')
#'/store/data/Run2016G/DoubleMuonLowMass/AOD/23Sep2016-v1/90000/F2D4D1D6-1597-E611-8963-0CC47A0AD63E.root')
#'/store/data/Run2016B/DoubleMuonLowMass/AOD/23Sep2016-v3/00000/000D5D1A-3798-E611-982A-008CFA0647BC.root')
'/store/data/Run2016G/DoubleMuonLowMass/AOD/23Sep2016-v1/100000/001A518F-368C-E611-B0C9-0CC47A4C8E38.root')
#'/store/data/Run2016E/DoubleMuonLowMass/AOD/23Sep2016-v1/100000/003C2808-EF93-E611-AD7E-0CC47A7C361E.root')
#'/store/data/Run2016B/DoubleMuonLowMass/AOD/23Sep2016-v3/00000/001AD4E7-4498-E611-B144-002590E7DF2A.root')
#'/store/data/Run2016H/Charmonium/AOD/PromptReco-v2/000/281/207/00000/0042DCF9-6382-E611-ABFD-02163E0133B7.root')
    )

#process.GlobalTag.globaltag = cms.string('FT53_V21A_AN6::All')
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
##process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data') 
process.GlobalTag = GlobalTag(process.GlobalTag, '80X_dataRun2_2016SeptRepro_v7','') 



#print "\nGlobalTag : FT53_V21A_AN6::All\n"

# do trigger matching for muons
triggerProcessName = 'HLT'

process.cleanMuonTriggerMatchHLT0 = cms.EDProducer(
    # match by DeltaR only (best match by DeltaR)
    'PATTriggerMatcherDRLessByR',                         
    src                   = cms.InputTag('cleanPatMuons'),
    # default producer label as defined in
    # PhysicsTools/PatAlgos/python/triggerLayer1/triggerProducer_cfi.py
    matched               = cms.InputTag('patTrigger'),
    #matchedCuts           = cms.string('path("HLT_DoubleMu3p5_LowMass_Displaced*",0,0)'),
    matchedCuts           = cms.string('path("HLT_DoubleMu4_LowMassNonResonantTrk_Displaced*",0,0)'),
    maxDeltaR             = cms.double(0.1),
    # only one match per trigger object
    resolveAmbiguities    = cms.bool(True),
    # take best match found per reco object (by DeltaR here, see above)       
    resolveByMatchQuality = cms.bool(False))

from PhysicsTools.PatAlgos.tools.trigTools import *
switchOnTriggerMatchEmbedding(process, triggerMatchers = ['cleanMuonTriggerMatchHLT0'],
                              hltProcess = triggerProcessName, outputModule = '')

g_TriggerNames_LastFilterNames = [
    #('HLT_DoubleMu3p5_LowMass_Displaced',  'hltDisplacedmumuFilterDoubleMu3p5LowMass') 
    ('HLT_DoubleMu4_LowMassNonResonantTrk_Displaced',  'hltLowMassNonResonantTkVertexFilter') 
    ]

g_TriggerNames = [i[0] for i in g_TriggerNames_LastFilterNames]
g_LastFilterNames = [i[1] for i in g_TriggerNames_LastFilterNames]

process.ntuple.TriggerNames = cms.vstring(g_TriggerNames)
process.ntuple.LastFilterNames = cms.vstring(g_LastFilterNames)
