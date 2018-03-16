#!/bin/bash                                                                                                                                                                       


declare -i nfiles
nfiles=(16340999/2000000)+1
echo "nfiles = $nfiles"


echo -e "\n@@@@  shell script is going to produce $nfiles files  @@@@"
echo -e "\n=> processing singlecand ntuple for Bs2phimm sample."

#for num in {1..4}                                                                                                                          

for ((num=1;num<=$nfiles;num++))
do
 echo -e "\n./sel mc.lite cut0  /afs/cern.ch/work/d/dsahoo/public/forNiladri/Aodntuple/BsToPhiMuMu_phimumu_2016_MC_4.root  BsToPhiMuMu_SCN_2016 -s $((2000000*$((num-1))))  -n 2000000"
 ./sel mc.lite cut0  /afs/cern.ch/work/d/dsahoo/public/forNiladri/Aodntuple/BsToPhiMuMu_phimumu_2016_MC_4.root BsToPhiMuMu_SCN_2016  -s $((2000000*$((num-1))))  -n 2000000
 echo -e "\n==> file$num is done."
 echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
done
