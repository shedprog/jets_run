#!/bin/bash

# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_05e.list
# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_06e.list
# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_040506e.list
# DATA=afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_06p.list
# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_07p.list
# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_0607p.list

# DATA=afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_Ariadne_Low_Q2_NC_DIS_05e.list
# DATA=afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_Ariadne_Low_Q2_NC_DIS_06e.list
DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_Ariadne_Low_Q2_NC_DIS_0607p.list
# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_Ariadne_Low_Q2_NC_DIS_05e.list


# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_ari_incl_nc_DIS_lowQ2_05e.list
# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_ari_incl_nc_DIS_lowQ2_0607p.list
# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_ari_incl_nc_DIS_lowQ2_06e.list
# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_ari_incl_nc_DIS_lowQ2_040506e.list

# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_Pythia_PHP_QCD_direct_040506e.list
# DATA=/afs/desy.de/user/q/quintera/public/Jets/Lists/Sample_Pythia_PHP_QCD_resolved_040506e.list

list_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"/lists
WORKDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
#OUTDIR=/nfs/dust/zeus/group/mykytaua/Jets_nfs/20.04.2019/lepton_prob_energy
OUTDIR=/nfs/dust/zeus/group/mykytaua/Jets_nfs/25.04.2019/Cal_empz
mkdir -p $OUTDIR


while [ ! $# -eq 0 ]
do
    case "$1" in

        --run| -r)

            # WORKDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

            filename=$(basename $DATA)

            output=${OUTDIR}/output_${filename}
            mkdir -p $output
            data_outdir=${output}/output

            mkdir -p $data_outdir
            mkdir -p $output/condor
            mkdir -p $output/condor/log
            mkdir -p $output/condor/err
            mkdir -p $output/condor/out
            
            number=`cat $DATA | wc -l`
            echo 'number of file: '$number

            sed "s|NUMBER_QUEUE|$number|g ;\
            	 s|INITDIR|$output|g;
            	 s|OUT_CONDOR|$output/condor|g\
            	" $WORKDIR/QA.job > $output/QA.job

            sed "s|DATA_LIST|$DATA|g;\
            	 s|OUTPUT_PATH|$data_outdir|g\
            	" $WORKDIR/runInc.sh > $output/runInc.sh

	    cp -rf $WORKDIR/src $output/
	    cp -rf $WORKDIR/make_hists.cxx $output/

            # ln -s $cut_file $output/cuts.data
            #cp $WORKDIR/matches_ele_Si.C $output/matches_ele_Si.C
            #cp $WORKDIR/MakeHist_v5_1.C $output/MakeHist_v5_1.C
            #cp $WORKDIR/JetOrange2018.h $output/JetOrange2018.h

            cd $output
           	condor_submit $output/QA.job
            cd -
            shift
            ;;

        --run_all| -a)
            
            for DATA in ${list_dir}/*
            do

                filename=$(basename $DATA)

                output=${OUTDIR}/output_${filename}
                mkdir -p $output
                data_outdir=${output}/output

                mkdir -p $data_outdir
                mkdir -p $output/condor
                mkdir -p $output/condor/log
                mkdir -p $output/condor/err
                mkdir -p $output/condor/out
                
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
                # cp $WORKDIR/matches_ele_Si.C $output/matches_ele_Si.C
                # cp $WORKDIR/JetOrange2018.h $output/JetOrange2018.h

	        cp -rf $WORKDIR/src $output/
	        cp -rf $WORKDIR/make_hists.cxx $output/

                cd $output
                condor_submit $output/QA.job
                cd -
            done

            shift
            ;;

        --hadd| -h)

            mkdir ${OUTDIR}/root
            
            for i in ${OUTDIR}/output/output_*
            do
                filename=$(basename $i)
                hadd root/${filename}.root ${i}/output/*
            done 
            shift
            ;;

        *)
            echo 'Not available flag!'
            break
            shift
            ;;
    esac
    shift
done
