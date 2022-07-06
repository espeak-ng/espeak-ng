#!/bin/python3
# /*
#  * Copyright (C) 2022 Anna Stan ,  Mamaodou Dramé Kalilou , Nicolas Morel
#  *
#  * This program is free software; you can redistribute it and/or modify
#  * it under the terms of the GNU General Public License as published by
#  * the Free Software Foundation; either version 3 of the License, or
#  * (at your option) any later version.
#  *
#  * This program is distributed in the hope that it will be useful,
#  * but WITHOUT ANY WARRANTY; without even the implied warranty of
#  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  * GNU General Public License for more details.
#  *
#  * You should have received a copy of the GNU General Public License
#  * along with this program; if not, see: <http://www.gnu.org/licenses/>.
#  */
import sys
import mmap
import argparse
import shutil
import os
from os import O_RDONLY, O_RDWR, O_WRONLY, O_TRUNC, O_CREAT, SEEK_END, SEEK_CUR, SEEK_SET

def main(argc, argv):
  if argc < 2:
    print('Summary: add file to the corpus ', file=sys.stderr)
    print(f'Usage: {argv[0]} -c <corpus_dir>', file=sys.stderr)
    exit(1)

  ap = argparse.ArgumentParser()

  # Add the arguments to the parser
  ap.add_argument("-c", "--corpus_dir", required=True,
  help="corpus directory where to add the file")
  args = vars(ap.parse_args())


  lang_list=os.getenv("FUZZ_VOICE")
  if(lang_list):
    list=lang_list+"_list"
  else:
    list="en_list"
  output_name = list+"_dict_corpus.txt"
  output_path=args['corpus_dir']+output_name
  output= open(output_path, "w")

  path="../../dictsource/"+list
  

  file = open( path, "r")
  lines=file.readlines()
  index=1
  for line in lines:
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
if __name__ == "__main__":
  main(len(sys.argv), sys.argv)
