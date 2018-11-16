#################################
# CHECK THE SITES FOR DATASET
#################################

PD=Charmonium
year=Run2016
tag=23Sep2016


f=(B C D E F G)

for i in ${f[@]}; do
    ##echo "f=$i"
    echo "/$PD/$year$i-$tag*/AOD"
    ###dasgoclient --query="site dataset=/Charmonium/Run2016$i-07Aug17-v1/AOD"
    dasgoclient --query="dataset=/$PD/$year$i-$tag*/AOD"
    dasgoclient --query="site dataset=/$PD/$year$i-$tag*/AOD"

done