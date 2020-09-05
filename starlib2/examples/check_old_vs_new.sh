#!/bin/bash

OLD_PROG=/bmrb/linux/bin/formatNMRSTAR
NEW_PROG=./formatNMRSTAR

num=0
for fname in $*
do
  num=$(( $num + 1 ))
  $OLD_PROG < $fname > $fname.old
  $NEW_PROG < $fname > $fname.new
  diff $fname.old $fname.new > $fname.diff
  if [ -s $fname.diff ]
  then
    echo "$fname has a diff between old and new:"
    cat $fname.diff
  fi
  rm $fname.diff
  rm $fname.old
  rm $fname.new
  echo "file number $num" 1>&2
done
