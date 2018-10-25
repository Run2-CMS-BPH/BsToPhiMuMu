#=======================================
# @author: N.Sahoo <nsahoo@cern.ch>
# @date: 2017-06-30
#=======================================

#-------------
#cmssw config
#-------------
import FWCore.ParameterSet.Config as cms
from bstophimumu_2016_cfi_may8 import process

process.options = cms.untracked.PSet(SkipEvent = cms.untracked.vstring('ProductNotFound'))
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
     ####'/store/user/nsahoo/PYTHIA8_Bs2PhiMuMu_GenOnly_13TeV_NoFilter/crab_edm_Bs2PhiMuMu_MC_GenOnly_13TeV_NoFilter/170630_082400/0000/PYTHIA8_Bs2PhiMuMu_TuneCUEP8M1_13TeV_GenOnly_NoFilter_1.root',
     '/store/user/nsahoo/PYTHIA8_Bs2JPSiPhi_13TeV_GenOnly_NoFilter_Oct18/PYTHIA8_Bs2JPSiPhi_13TeV_GenOnly_NoFilter/EDM_Bs2JPSiPhi_MC_GenOnly_13TeV_NoFilter/181018_104804/0000/PYTHIA8_Bs2JPSiPhi_TuneCUEP8M1_13TeV_GenOnly_NoFilter_1.root',
##'/store/user/dsahoo/PYTHIA8_Bs2PhiMuMu_GenOnly_13TeV_NoFilter/crab3_Bs2PhiMuMu_MC_GenOnly_13TeV_NoFilter_new/170701_212745/0000/PYTHIA8_Bs2PhiMuMu_TuneCUEP8M1_13TeV_GenOnly_NoFilter_1.root',
                           )
                        )

##from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
##process.GlobalTag = GlobalTag(process.GlobalTag, '80X_mcRun2_asymptotic_2016_TrancheIV_v6', '')

process.ntuple.IsMonteCarlo = cms.untracked.bool(True)
process.ntuple.KeepGENOnly = cms.untracked.bool(True)
process.p = cms.Path(process.ntuple)
