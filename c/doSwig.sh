swig -python tinkercell.i
mv tinkercell_wrap.c ../python
mv tinkercell.py ../python
swig -perl tinkercell.i
mv tinkercell_wrap.c ../perl
mv tinkercell.pm ../perl
swig -ruby tinkercell.i
mv tinkercell_wrap.c ../ruby
swig -r tinkercell.i
mv tinkercell_wrap.c ../R
mv tinkercell.R ../R
more ../python/tc2py.py >> ../python/tinkercell.py
