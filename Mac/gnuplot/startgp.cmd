/* StartGnuplot.cmd , startgp.cmd */

/* release 19990512 */

/*
   Utility to _temporarily_ set the necessary environment variables for
   the gnuplot binary in the same directory of this utility (often
   called "startgp.cmd").
   You may start it from an arbitrary directory then.
   It passes all command line parameters to the gnuplot executable.
*/

Parse Arg param
Parse Source os env full_name

call setlocal
bindir   = FileSpec("path", full_name)
bindrive = FileSpec("drive", full_name)
binpath  = bindrive''bindir
path     =  Value('PATH',,'OS2ENVIRONMENT')
helpfile = bindrive''bindir'gnuplot.gih'
call Value 'GNUHELP', helpfile, 'OS2ENVIRONMENT'
call Value 'GNUPLOT', bindir,   'OS2ENVIRONMENT'
call Value 'PATH', path';'binpath,   'OS2ENVIRONMENT'

/* the actual call to gnuplot */
'@'bindrive''bindir'gnuplot.exe 'param

call endlocal
exit
