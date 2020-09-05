#!/bin/bash

for fName in `(cd /share/subedit/entries/ && ls -d bmr[4-9][0-9][0-9][0-9])`
do
  num=${fName#bmr}
  echo "$num" 1>&2

  same='n'
  if [ -f /share/subedit/entries/$fName/work/${fName}_3.str ]
  then
    v3Exists='y'
    if stardiff -null . -null '?' -null ''  /share/subedit/entries/$fName/work/auto_convert/${fName}_autogen_3.str /share/subedit/entries/$fName/work/${fName}_3.str >/dev/null 2>&1
    then
      same='y'
    fi
  else
    v3Exists='n'
  fi

  if [ $same = "y" -o $v3Exists = "n" ]
  then
    if [ -f /share/subedit/entries/$fName/work/${fName}_21.str ]
    then
      ./experiment_grep /share/subedit/entries/$fName/work/${fName}_21.str | sed "s/^/${fName}:/g" 
    fi
  fi
  
done
