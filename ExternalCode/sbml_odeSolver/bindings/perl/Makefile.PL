# Last changed Time-stamp: <2005-05-23 17:42:02 xtof>
# $Id: Makefile.PL.in,v 1.1.1.1 2005/05/30 19:49:13 raimc Exp $
use ExtUtils::MakeMaker;

my @Xtralib = map{chomp; $_} <DATA>;
WriteMakefile(MAKEFILE => 'Makefile-perl',
              NAME     => 'LibODES',
              LIBS     => [join(' ', @Xtralib)],
	      dynamic_lib =>
	      {LDDLFLAGS=> '-Wl,-rpath,/usr/local/lib -L/usr/local/lib -lsbml'},
              INC      => '-I../swig -I../../src -I/usr/local/include -I/usr/local/include -I/usr/local/include/sbml',
              MYEXTLIB => '../../src/libODES.a',
              OBJECT   => 'LibODES_wrap.o',
              AUTHOR   => 'Christoph Flamm <xtof@tbi.univie.ac.at>',
              VERSION  => '1.6.0',
);

sub MY::postamble {
	return <<'MAKE_FRAG';
LibODES_wrap.c:
	swig -Wall -perl5 -proxy -I../../src/ -I/usr/local/include -I/usr/local/include/sbml LibODES.i
MAKE_FRAG
}

__DATA__
-L/scr/fremdling/raim/lib -lsundials_kinsol -lsundials_cvodes -lsundials_cvode -lsundials_nvecserial -lsundials_shared
-L/usr/local/grace/lib -lgrace_np
-L/usr/local/lib/graphviz -ldotneato
-L/scr/fremdling/raim/lib -lsundials_kinsol -lsundials_cvodes -lsundials_cvode -lsundials_nvecserial -lsundials_shared
-L/usr/local/lib -lsbml
-lxerces-c
-lm
