"""
category: Sequence
name: Print all sequences
description: print all the sequence in this model in FASTA format
icon: plugins/c/printSequences.png
menu: yes
"""

from tinkercell import *
A = tc_itemsOfFamily('Part');

if A.length > 0:
  names = tc_getUniqueNames(A);
  attribs = ('sequence',);
  seqs = tc_getAllTextNamed(A, toStrings(attribs) );

  n = seqs.length;
  s = '';
  for i in range(0,n):
    s += '>' + tc_getString(names,i) + '\n' + tc_getString(seqs,i) + '\n';

  tc_print(s);

