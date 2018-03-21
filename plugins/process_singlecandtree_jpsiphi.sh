#!/bin/bash                                                                                                                                                                       


declare -i nfiles
nfiles=(24957968/2000000)+1
echo "nfiles = $nfiles"


echo -e "\n@@@@  shell script is going to produce $nfiles files  @@@@"
echo -e "\n=> processing singlecand ntuple for Bs2jpsiphi sample."

#for num in {1..4}                                                                                                                          

for ((num=1;num<=$nfiles;num++))
do
 echo -e "\n./sel mc.lite cut0  /afs/cern.ch/work/d/dsahoo/public/forNiladri/Aodntuple/BsToPhiMuMu_JpsiPhi_MC_2016_0.root  BsToJpsiPhi_SCN_2016 -s $((2000000*$((num-1))))  -n 2000000"
 ./sel mc.lite cut0  /afs/cern.ch/work/d/dsahoo/public/forNiladri/Aodntuple/BsToPhiMuMu_JpsiPhi_MC_2016_0.root BsToJpsiPhi_SCN_2016  -s $((2000000*$((num-1))))  -n 2000000
 echo -e "\n==> file$num is done."
 echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
done
