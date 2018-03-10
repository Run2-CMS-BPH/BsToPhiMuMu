#######################################
#   THIS RECIPE NEEDS TO BE UPDATED   #
#######################################

echo -e "---- JOB START ----"

cmsRun bstophimumu_2016_mc.py
echo -e "Bs2phimumu mc Ntuple produced."

mv  BsToPhiMuMu_2016.root BsToPhiMuMu_2016_mc.root
echo -e "renamed the (mc) root file."

cmsRun bstophimumu_Run2016.py
echo -e "Bs2phimumu data Ntuple produced."

mv BsToPhiMuMu_2016.root BsToPhiMuMu_2016_data.root
echo -e "renamed the (data) root file."

echo -e "----  JOB FINISH ----"