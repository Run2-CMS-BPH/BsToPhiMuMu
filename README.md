# Run2-Bs2PhiMuMu 
Run2 Bs --> Phi(KK) mu+ mu- analysis

# How to build:
-------------
$ cmsrel CMSSW_8_0_24

$ cd CMSSW_8_0_24/src/

$ cmsenv

$ git clone https://github.com/Run2-CMS-BPH/BsToPhiMuMu.git

$ scram b 


# How to run:
-----------
$ cd BsToPhiMuMu/python/

For data, do

$ cmsRun file.py
where file stands for 
- bstophimumu_Run2016BG_23SepRepro_LMNRtrk        (for 2016B-G datasets with LMNRtrk trigger path)
- bstophimumu_Run2016H_PromptReco_LMNRtrk         (for 2016H dataset with LMNRtrk trigger path)

For mc, do

$ cmsRun bstophimumu_2016_mc_may8.py  (to run over signal mc)

$ cmsRun bstophimumu_2016_mc_JPSI_june1.py (to run over J/psiphi mc)

$ cmsRun bstophimumu_2016_mc_PSIP_mar10.py (to run over psi'phi mc)





