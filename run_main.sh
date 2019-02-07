#!/bin/bash

# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_05e.list
# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_06e.list
# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_040506e.list
# DATA=afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_06p.list
# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_07p.list
DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_0607p.list

# DATA=afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_Ariadne_Low_Q2_NC_DIS_05e.list
# DATA=afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_Ariadne_Low_Q2_NC_DIS_06e.list
# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_Ariadne_Low_Q2_NC_DIS_0607p.list
# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_Ariadne_Low_Q2_NC_DIS_05e.list


# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_ari_incl_nc_DIS_lowQ2_05e.list
# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_ari_incl_nc_DIS_lowQ2_0607p.list
# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_ari_incl_nc_DIS_lowQ2_06e.list
# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_ari_incl_nc_DIS_lowQ2_040506e.list

# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_Pythia_PHP_QCD_direct_040506e.list
# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_Pythia_PHP_QCD_resolved_040506e.list

cut_file="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"/cuts.data

WORKDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"


filename=$(basename $DATA)

output=${WORKDIR}/outpu_${filename}
mkdir -p $output
data_outdir=${output}/output

mkdir -p $data_outdir
mkdir -p $output/condor
mkdir -p $output/condor/log
mkdir -p $output/condor/err
mkdir -p $output/condor/out

while [ ! $# -eq 0 ]
do
    case "$1" in

        --run| -r)
            
            number=`cat $DATA | wc -l`
            echo 'number of file: '$number

            sed "s|NUMBER_QUEUE|$number|g ;\
            	 s|INITDIR|$output|g;
            	 s|OUT_CONDOR|$output/condor|g\
            	" $WORKDIR/QA.job > $output/QA.job

            sed "s|DATA_LIST|$DATA|g;\
            	 s|OUTPUT_PATH|$data_outdir|g\
            	" $WORKDIR/runInc.sh > $output/runInc.sh

            # ln -s $cut_file $output/cuts.data
            ln -s $WORKDIR/MakeHist_v5_1.C $output/MakeHist_v5_1.C
            ln -s $WORKDIR/JetOrange2018.h $output/JetOrange2018.h
           	condor_submit $output/QA.job

            shift
            ;;

        --hadd| -h)
        
            hadd output/${filename}.root ${data_outdir}/*
            shift
            ;;

        *)
            echo 'Not available flag!'
            break
            ;;
    esac
    shift
done