#!/bin/bash

if [[ $# -lt 1 ]]
then
  echo "Usage: $0 <corpus-dir>"
  echo "Usage: $0 --preserve-txt <corpus-dir> (minimize corpus but keep .txt files intact)"
  exit 1
fi

preserve_txt=0
if [[ "$1" == "--preserve-txt" ]]
then
  preserve_txt=1
  CORPUS_DIR=$2
else
  CORPUS_DIR=$1
fi


export FUZZ_VOICE=en
FUZZER=./synth_fuzzer

TMP_DIR=$(mktemp -d)
echo "Merging..."
`$FUZZER -merge=1 $TMP_DIR $CORPUS_DIR`
echo "Removing old files..."
if [[ $preserve_txt -eq 1 ]]
then
  echo " => Preserve .txt files"
  rm -rvf $(find $CORPUS_DIR | grep -vE "*.txt|$CORPUS_DIR") 2>/dev/null
else
  rm -rf $CORPUS_DIR/* 2>/dev/null
fi
cp $TMP_DIR/* $CORPUS_DIR 2>/dev/null
rm -rf $TMP_DIR
echo "Merging done !"
