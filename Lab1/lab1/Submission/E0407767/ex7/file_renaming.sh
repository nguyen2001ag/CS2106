#!/bin/bash

####################
# Lab 1 Exercise 7
# Name: Nguyen Minh Nguyen
# Student No: A0200786E
# Lab Group: B07
####################

####################
# Strings that you may need for prompt (not in order)
####################
# Enter $N numbers:
# Enter NEW prefix (only alphabets):
# Enter prefix (only alphabets):
# INVALID
# Number of files to create:
# Please enter a valid number [0-9]:
# Please enter a valid prefix [a-z A-Z]:

####################
# Strings that you may need for prompt (not in order)
####################

#################### Steps #################### 

# Fill in the code to request user for the prefix
read -p $'Enter prefix (only alphabets):\n' prefix
validPrefix='[^a-zA-Z]'

# Check the validity of the prefix #
while [[ $prefix =~ $validPrefix || -z $prefix ]]
do
    echo 'INVALID'
    read -p $'Please enter a valid prefix [a-z A-Z]:\n' prefix
done

# Enter numbers and create files #
read -p $'Number of files to create:\n' nFiles
while ! [[ $nFiles =~ ^[0-9]+$ ]] 
do
    echo 'INVALID'
    read -p $'Please enter a valid number [0-9]\n' nFiles
done

declare -a inputNumber
echo "Enter $nFiles numbers:"
for (( c=0; c<$nFiles; c++ ))
do
    read number
    while ! [[ $number =~ ^[0-9]+$ ]] 
    do
        echo 'INVALID'
        read -p $'Please enter a valid number [0-9]\n' number
    done
    inputNumber+=($number)
done


for num in ${inputNumber[@]}
do
    touch ${prefix}_$num.txt
done

echo ""
echo "Files Created"
ls *.txt
echo ""

# Fill in the code to request user for the new prefix
read -p $'Enter NEW prefix (only alphabets):\n' newPrefix
while [[ $newPrefix =~ $validPrefix || -z $newPrefix ]]
do
    echo 'INVALID'
    read -p $'Please enter a valid prefix [a-z A-Z]:\n' newPrefix
done
# Renaming to new prefix #
currentFilename=${0:2}

for file in *
do
    if [[ $file = $currentFilename ]]; then
        continue
    fi
    if [[ ${file%_*} != $prefix ]]; then
        continue
    fi
    mv $file ${newPrefix}_${file#*_}
done


echo ""
echo "Files Renamed"
ls *.txt
echo ""
