#!/bin/bash
for file in ../TestCases/*.tig
do
  echo -n "Parsing ${file}... "
  ./parsetest $file
done
