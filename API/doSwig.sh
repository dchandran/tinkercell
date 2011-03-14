swig -python tinkercell.i
mv tinkercell_wrap.c ../python
mv tinkercell.py ../python
swig -octave -c++ tinkercell.i
mv tinkercell_wrap.cxx ../octave/tinkercell_wrap.cpp
swig -perl tinkercell.i
mv tinkercell_wrap.c ../perl
mv tinkercell.pm ../perl
swig -ruby tinkercell.i
mv tinkercell_wrap.c ../ruby
swig -r tinkercell.i
mv tinkercell_wrap.c ../R
mv tinkercell.R ../R
swig -java tinkercell.i
mv tinkercell_wrap.c ../java
mv *.java ../java
javac ../java/*.java
