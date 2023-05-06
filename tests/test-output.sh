#!/bin/bash

echo $@

project_dir=$1
shift
test_type=$1
shift
OPTS=$*

samples_dir="$project_dir/tests/samples"
output_file_arr=()

for frag in $(ls $samples_dir/$test_type/*.[fd]* ); do
    output_file=$(basename $frag | sed 's/\.[fd].*/\.out/')
    output_file_arr+=( $output_file )
    
    spim_file=$(basename $frag | sed 's/\.[fd].*/\.s/')

    echo "Running decaf-22 $OPTS $frag > $output_file"
    $project_dir/build/bin/decaf-22 $OPTS $frag  > $output_file

    if [[ $test_type == "codegen" && -f $spim_file ]]; then
        echo "Loading definitions and running MIPS file"
        cat ../spim-defs/defs.asm >> ./$spim_file
        ./spim -file ./$spim_file >> $output_file
        echo "" >> $output_file
    fi
done

ret=0
for out in ${output_file_arr[@]}; do
    # err_output=$(diff -w $out $samples_dir/$out)
    echo $out
    diff -w $out $samples_dir/$test_type/$out

    if [[ $? -ne 0 ]]; then
        ret=1
    else
        rm $out
    fi
done


exit $ret