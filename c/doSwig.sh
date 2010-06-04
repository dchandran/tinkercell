swig -python tinkercell.i
mv tinkercell_wrap.c ../python
mv tinkercell.py ../python
swig -octave -c++ tinkercell.i -o tinkercell_wrap.cxx
mv tinkercell_wrap.cxx ../octave
swig -perl tinkercell.i
mv tinkercell_wrap.c ../perl
mv tinkercell.pm ../perl
swig -ruby tinkercell.i
mv tinkercell_wrap.c ../ruby
swig -r tinkercell.i
mv tinkercell_wrap.c ../R
mv tinkercell.R ../R
more ../python/tc2py.py >> ../python/tinkercell.py
