swig -python tinkercell.i
move /Y tinkercell_wrap.c ..\python
move /Y tinkercell.py ..\python
swig -perl tinkercell.i
move /Y tinkercell_wrap.c ..\perl
move /Y tinkercell.pm ..\perl
swig -ruby tinkercell.i
move /Y tinkercell_wrap.c ..\ruby
swig -r tinkercell.i
move /Y tinkercell_wrap.c ..\R
move /Y tinkercell.R ..\R
more ..\python\tc2py.py >> ..\python\tinkercell.py
