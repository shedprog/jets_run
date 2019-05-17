#!/bin/bash

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#DATA
DATA_05e=/nfs/dust/zeus/group/mykytaua/Jets_nfs/List_contr_plots/Sample_05e.list
DATA_0607p=/nfs/dust/zeus/group/mykytaua/Jets_nfs/List_contr_plots/Sample_0607p.list
DATA_06e=/nfs/dust/zeus/group/mykytaua/Jets_nfs/List_contr_plots/Sample_06e.list

#MC 0506e + results for calibration
MC_05e=/nfs/dust/zeus/group/mykytaua/Jets_nfs/List_contr_plots/Sample_Ariadne_Low_Q2_NC_DIS_05e.list
MC_06e=/nfs/dust/zeus/group/mykytaua/Jets_nfs/List_contr_plots/Sample_Ariadne_Low_Q2_NC_DIS_06e.list
PATH_0506e_MC=/nfs/dust/zeus/group/mykytaua/Jets_nfs/2019.05.15/control_plots_clean/root/output_Sample_Ariadne_Low_Q2_NC_DIS_0506e.list.root
PATH_0506e_DATA=/nfs/dust/zeus/group/mykytaua/Jets_nfs/2019.05.15/control_plots_clean/root/output_Sample_0506e.list.root

#MC 0607p + results for calibration
MC_0607p=/nfs/dust/zeus/group/mykytaua/Jets_nfs/List_contr_plots/Sample_Ariadne_Low_Q2_NC_DIS_0607p.list
PATH_0607p_MC=/nfs/dust/zeus/group/mykytaua/Jets_nfs/2019.05.15/control_plots_clean/root/output_Sample_Ariadne_Low_Q2_NC_DIS_0607p.list.root
PATH_0607p_DATA=/nfs/dust/zeus/group/mykytaua/Jets_nfs/2019.05.15/control_plots_clean/root/output_Sample_0607p.list.root

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#DATA=

list_dir=/nfs/dust/zeus/group/mykytaua/Jets_nfs/List_contr_plots
WORKDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
#OUTDIR=/nfs/dust/zeus/group/mykytaua/Jets_nfs/20.04.2019/lepton_prob_energy
OUTDIR=/nfs/dust/zeus/group/mykytaua/Jets_nfs/2019.05.15/control_plots_re-weight_final
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
	
    	--run_calibr | -rc)
	    
	    DATA=( $MC_05e $MC_06e $MC_0607p )	
            
	    CALIBR_DATA=( $PATH_0506e_DATA 
	    		  $PATH_0506e_DATA
			  $PATH_0607p_DATA )

	    CALIBR_MC=(   $PATH_0506e_MC
	    		  $PATH_0506e_MC
			  $PATH_0607p_MC )

	    for (( i=1; i<4; i++ ))
	    do
		echo $DATA[$i]
		echo $CALIBR_DATA[$i]
		echo $CALIBR_MC[$i]
		
                filename=$(basename $DATA[$i])

                output=${OUTDIR}/output_${filename}
                mkdir -p $output
                data_outdir=${output}/output

                mkdir -p $data_outdir
                mkdir -p $output/condor
                mkdir -p $output/condor/log
                mkdir -p $output/condor/err
                mkdir -p $output/condor/out
                
                number=`cat $DATA[$i] | wc -l`
                echo 'number of file: '$number

                sed "s|NUMBER_QUEUE|$number|g ;\
                     s|INITDIR|$output|g;\
                     s|OUT_CONDOR|$output/condor|g\
                    " $WORKDIR/QA.job > $output/QA.job

                sed "s|DATA_LIST|$DATA[$i]|g;\
                     s|OUTPUT_PATH|$data_outdir|g;\
		     s|DATA_CALIBR|$CALIBR_DATA[$i]|g;\
		     s|MC_CALIBR|$CALIBR_MC[$i]|g;\
		     s|.*\sroot.exe.*||g;\
		     s|#root.exe|root.exe|g\
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
            
            for i in ${OUTDIR}/output_*
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
