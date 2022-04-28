#!/bin/python3

import sys
import mmap
import argparse
import shutil
import os
from os import O_RDONLY, O_RDWR, O_WRONLY, O_TRUNC, O_CREAT, SEEK_END, SEEK_CUR, SEEK_SET




lang_list=os.getenv("FUZZ_VOICE")
if(lang_list):
  list=lang_list+"_list"
else:
  list=en_list
output_name = list+"_dict_corpus.txt"
output= open(output_name, "w")

path="../../dictsource/"+list

file = open( path, "r")
lines=file.readlines()
index=1
for line in lines:
  li=[]
  lli=""
  if line[0]=='/' and line[1]=='/':
    continue
  res = line.split()
  if len(res):
    output.write("kw")
    output.write(str(index))
    index=index+1
    output.write("=")
    output.write(res[0])
    output.write('\n')
file.close()
output.close()
