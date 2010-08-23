swig -python tinkercell.i
move /Y tinkercell_wrap.c ..\python
move /Y tinkercell.py ..\python
swig -octave -c++ tinkercell.i
move tinkercell_wrap.cxx ../octave/tinkercell_wrap.cpp
swig -perl tinkercell.i
move /Y tinkercell_wrap.c ..\perl
move /Y tinkercell.pm ..\perl
swig -ruby tinkercell.i
move /Y tinkercell_wrap.c ..\ruby
swig -r tinkercell.i
move /Y tinkercell_wrap.c ..\R
move /Y tinkercell.R ..\R
more ..\python\tc2py.py >> ..\python\tinkercell.py
