#!/bin/bash
source /cvmfs/sft.cern.ch/lcg/views/setupViews.sh LCG_91 x86_64-slc6-gcc7-opt

echo `date`
data=DATA_LIST
outdir=OUTPUT_PATH
counter=0
for infile in  `cat $data` ; do
    if [ "$counter" = "$1" ]; then  # Run parallel
	root -b -q ./make_hists.cxx\(\"$infile\",\"$outdir\"\)
	#root.exe ./make_hists.cxx\(\"$infile\",\"$outdir\",\"DATA_CALIBR\",\"MC_CALIBR\"\)
	let counter=counter+1
	break
    fi
    let counter=counter+1
done   #End runnumber
echo `date`
