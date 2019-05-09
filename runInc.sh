#!/bin/bash

echo `date`
data=DATA_LIST
outdir=OUTPUT_PATH
counter=0
for infile in  `cat $data` ; do
    if [ "$counter" = "$1" ]; then  # Run parallel
	root.exe ./MakeHists_v5_1.C\(\"$infile\",\"$outdir\"\)
	let counter=counter+1
	break
    fi
    let counter=counter+1
done   #End runnumber
echo `date`
