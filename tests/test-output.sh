#!/bin/bash

project_dir=$1

samples_dir=./samples


for frag in $(ls $samples_dir/*.[fp]* ); do
    output_file=$(basename $frag | sed 's/\.[fp].*/\.out/')
    $project_dir/build/bin/decaf-22 $frag > $output_file
done

for out in $(ls *.out); do
    diff -w $out "$samples_dir/$out"
    rm $out
done