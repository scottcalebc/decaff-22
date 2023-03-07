#!/bin/bash

project_dir=$1

samples_dir=./samples


for frag in $(ls $samples_dir/*.[fd]* ); do
    output_file=$(basename $frag | sed 's/\.[fd].*/\.out/')
    $project_dir/build/bin/decaf-22 $frag > $output_file
done

ret=0
for out in $(ls *.out); do
    # err_output=$(diff -w $out $samples_dir/$out)

    diff -w $out $samples_dir/$out

    if [[ $? -ne 0 ]]; then
        ret=1
    else
        rm $out
    fi
done


exit $ret