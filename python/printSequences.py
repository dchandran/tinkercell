#TINKERCELL HEADER BEGIN
#category: Sequence
#name: Print all sequences
#descr: print all the sequence in this model in FASTA format
#icon: Plugins/c/printSequences.png
#menu: yes
#TINKERCELL HEADER END

import pytc
A = pytc.itemsOfFamily('Part');

if (len(A) > 0):
  names = pytc.getNames(A);
  attribs = ('sequence',);
  seqs = pytc.getAllTextNamed(A,attribs);

  n = len(seqs);
  s = '';
  for i in range(0,n):
    s += '>' + names[i] + '\n' + seqs[i] + '\n';

  pytc.write(s);

