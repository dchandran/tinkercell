#include <stdlib.h>
#include <stdio.h>

//#define CRUFT_DLL
//#define OCTAVE_DLL
//#define OCTINTERP_DLL

#include <iostream>
#include <fstream>

#include <octave/config.h>
#include <octave/octave.h>
#include <octave/symtab.h>
#include <octave/parse.h>
#include <octave/unwind-prot.h>
#include <octave/toplev.h>
#include <octave/error.h>
#include <octave/quit.h>
#include <octave/variables.h>
#include <octave/sighandlers.h>
#include <octave/sysdep.h>

void recover_from_exception (void)
{
  unwind_protect::run_all ();
  can_interrupt = true;
  octave_interrupt_immediately = 0;
  octave_interrupt_state = 0;
  //octave_allocation_error = 0;
  octave_restore_signal_mask ();
  octave_catch_interrupts ();
}

void print_octave_value_list(const octave_value_list& list, std::ofstream& file)
{
	int len = list.length();
	int i;
	for (i=0; i < len; ++i)
	{
		list(i).print(file);
		if (i < (len-1))
			file << "\n";
	}
}

int octave_call(const char *string)
{
  int parse_status = 0;

  octave_save_signal_mask ();
  if (octave_set_current_context)
  {
#if defined (USE_EXCEPTIONS_FOR_INTERRUPTS)
      panic_impossible ();
#else
      unwind_protect::run_all ();
      raw_mode (0);
      octave_restore_signal_mask ();
#endif
  }

  can_interrupt = true;
  octave_catch_interrupts ();
  octave_initialized = true;

  // XXX FIXME XXX need to give caller an opaque pointer
  // so that they can define and use separate namespaces
  // when calling octave, with a shared global namespace.
  // Something like:
  //   int call_octave (const char *string, void *psymtab = NULL) {
  //     ...
  //     curr_sym_tab = psymtab == NULL ? top_level_sym_tab : symbol_table;
  // I suppose to be safe from callbacks (surely we have to
  // provide some way to call back from embedded octave into
  // the user's application), we should push and pop the current
  // symbol table.

  // Note that I'm trying to distinguish exception from 
  // failure in the return codes. I believe failure is 
  // indicated by -1.  I have execution exception (including
  // user interrupt and more dramatic failures) returning -2
  // and memory failure returning -3.  We should formalize
  // this with error codes defined in embed_octave.h.  Maybe
  // a more fine-grained approach could be used within octave
  // proper.
  try 
    {
      //curr_sym_tab = top_level_sym_tab;
      reset_error_handler ();
      eval_string(string, false, parse_status);
      //octave_value_list list = eval_string(string, false, parse_status,1);
      //std::ofstream outfile;
      //outfile.open(filename);
      //print_octave_value_list(list,outfile);
      //outfile.close();
    }
  catch (octave_interrupt_exception)
    {
      recover_from_exception (); 
      error_state = -2; 
    }
  catch (std::bad_alloc)
    {
      recover_from_exception ();
      error_state = -3;
    }

  octave_restore_signal_mask();
  octave_initialized = false;

  // XXX FIXME XXX callbacks calling embed_octave
  // may or may not want error_state reset.
  return error_state;
}

extern "C"
{

	OCTAVE_EXPORT void initialize()
	{
		int argc = 0;
		char * argv[0];
		octave_main(argc,argv,1);
	}

	OCTAVE_EXPORT void exec(const char * input, const char * outfile, const char * errfile)
	{
        FILE * f1 = freopen (errfile,"w+",stderr);
        FILE * f2 = freopen (outfile,"w+",stdout);
		  octave_call(input);
		  fclose(f1);
		  fclose(f2);
	}

	OCTAVE_EXPORT void finalize()
	{
 		 do_octave_atexit();
	}
}

