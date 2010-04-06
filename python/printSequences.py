"""
category: Sequence
name: Print all sequences
description: print all the sequence in this model in FASTA format
icon: Plugins/c/printSequences.png
menu: yes
"""

from tinkercell import *
A = tc_itemsOfFamily('Part');

if A.length > 0:
  names = tc_getNames(A);
  attribs = ('sequence',);
  seqs = tc_getAllTextNamed(A, toStrings(attribs) );

  n = seqs.length;
  s = '';
  for i in range(0,n):
    s += '>' + nthString(names,i) + '\n' + nthString(seqs,i) + '\n';

  tc_print(s);

