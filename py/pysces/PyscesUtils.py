"""
PySCeS - Python Simulator for Cellular Systems (http://pysces.sourceforge.net)

Copyright (C) 2004-2009 B.G. Olivier, J.M. Rohwer, J.-H.S Hofmeyr all rights reserved,

Brett G. Olivier (bgoli@users.sourceforge.net)
Triple-J Group for Molecular Cell Physiology
Stellenbosch University, South Africa.

Permission to use, modify, and distribute this software is given under the
terms of the PySceS (BSD style) license. See LICENSE.txt that came with
this distribution for specifics.

NO WARRANTY IS EXPRESSED OR IMPLIED.  USE AT YOUR OWN RISK.
Brett G. Olivier
"""

from pysces.version import __version__
__doc__ = '''General utility module'''

import fileinput
import os,sys
import time


def VersionCheck(ver='0.1.5'):
    print "dead"


def str2bool(s):
    """
    Tries to convert a string to a Python boolean

    - *s* True if 'True', 'true' or'1' else False
    """
    if s in ['True','true', '1']:
        return True
    else:
        return False

class TimerBox:
    """A timer "container" class that can be used to hold user defined timers"""
    __go__ = 1

    def __init__(self):
        if os.sys.version_info[0] < 2 or os.sys.version_info[1] < 3:
            print 'Your version of Python (' + str(os.sys.version_info[:3]) + ') might be too old\
            to use this class. Python 2.2 or newer is required'

    def normal_timer(self,name):
        """
        normal_timer(name)

        Creates a normal timer method with <name> in the TimerBox instance. Normal timers
        print the elapsed time since creation when called.

        Arguments:
        =========
        name: the timer name

        """
        assert type(name) == str
        def timer(startTime=time.time()):
            while self.__go__:
                ms = divmod(time.time() - startTime,60)
                nowTime = str(int(ms[0])) + ' min ' + str(int(ms[1])) + ' sec'
                yield nowTime
        setattr(self,name,timer())

    def step_timer(self,name,maxsteps):
        """
        step_timer(name,maxsteps)

        Creates a step timer method with <name> in the TimerBox instance. Step timers
        print the elapsed time as well as the next step out of maxsteps when called.

        Arguments:
        =========
        name: the timer name
        maxsteps: the maximum number of steps associated with this timer

        """
        assert type(name) == str, 'Name is a string representing the timer name'
        assert type(maxsteps) == int or type(maxsteps) == float, 'int or float needed'

        setattr(self,name+'_cstep',0)
        def timer(startTime=time.time(),maxS=maxsteps):
            while self.__go__:
                ms = divmod(time.time() - startTime,60)
                nowStep = 'step ' + str(getattr(self,name+'_cstep')) + ' of ' + str(maxS)+ ' ('+\
                          str(int(ms[0])) + ' min ' + str(int(ms[1])) + ' sec)'
                yield nowStep
        setattr(self,name,timer())

    def stop(self,name):
        """
        stop(name)

        Delete the timer <name> from the TimerBox instance

        Arguments:
        =========
        name: the timer name to delete

        """
        delattr(self, name)
        try:
            getattr(self,name+'_cstep')
            delattr(self,name+'_cstep')
        except:
            pass

    def reset_step(self,name):
        """
        reset_step(name)

        Reset the number of steps of timer <name> in the TimerBox to zero

        Arguments:
        =========
        name: the step timer whose steps should be reset

        """
        try:
            getattr(self,name+'_cstep')
            setattr(self,name+'_cstep',0)
        except:
            pass

def CopyTestModels(*args, **kwargs):
    print "moved to PyscesTest"

def CopyModels(*args, **kwargs):
    print "dead"


##  # these are going to be turned into methods ... perhaps a FileUtil class?
##  def CopyModels(dirIn=os.path.join(pysces.install_dir,'pscmodels'),dirOut=pysces_model,overwrite=0):
    ##  """
    ##  CopyModels(dirIn=os.path.join(pysces_install,'pscmodels'),dirOut=pysces_model,overwrite=0)

    ##  'Copy all PySCeS model files contained in dirIn to dirOut, defaults to:
    ##  in: pysces/pscmodels
    ##  out: pysces.model_dir

    ##  Arguments:
    ##  =========
    ##  dirIn [default=os.path.join(pysces_install,'pscmodels')]: target directory
    ##  dirOut [default=pysces_model]: destination directory
    ##  overwrite [default=0]: automaitcally (1) overwrite target files

    ##  """
    ##  if os.path.exists(dirIn):
        ##  if os.path.exists(dirOut):
            ##  print 'src : ' + dirIn
            ##  print 'dest: ' + dirOut
            ##  flist = os.listdir(dirIn)
            ##  if overwrite == 0:
                ##  flist2 = os.listdir(dirOut)
            ##  maxlen = 0
            ##  for x in flist:
                ##  if len(x) > maxlen: maxlen = len(x)
            ##  if len(flist) != 0:
                ##  for File in flist:
                    ##  if File[-4:] == '.psc':
                        ##  if overwrite == 0:
                            ##  try:
                                ##  a = flist2.index(File)
                                ##  #print File +  (maxlen-len(File))*'.' + ' skipped'
                            ##  except:
                                ##  shutil.copy(os.path.join(dirIn,File),os.path.join(dirOut,File))
                                ##  print File +  (maxlen-len(File))*'.' + ' ok'
                        ##  else:
                            ##  shutil.copy(os.path.join(dirIn,File),os.path.join(dirOut,File))
                            ##  print File +  (maxlen-len(File))*'.' + ' ok'
            ##  else:
                ##  print 'Empty directory?'
        ##  else:
            ##  print dirOut + ' does not exist'
    ##  else:
        ##  print dirIn + ' does not exist'

def LoadContrib():
    print 'dead'

##  # pysces contrib loader class - brett 20050613
##  def LoadContrib():
    ##  """
    ##  LoadContrib()

    ##  Initialize the PySCeS dynamic model loader. This method attempts to autoload the PySCeS
    ##  extension modules that have been added to the /contrib tree. Returns an instantiated Contrib instance

    ##  Arguments:
    ##  None

    ##  """
    ##  import PyscesContrib
    ##  contrib = PyscesContrib.contrib()
    ##  return contrib

def ConvertFileD2U(Filelist):
    """
    ConvertFileD2U(Filelist)

    Converts a [Filename] from \r\n to \n inplace no effect if the line termination is correct

    Arguments:
    =========
    Filelist: a file or list of files to convert

    """
    for line in fileinput.input(Filelist,inplace=1):
        try:
            if line[-2] == '\r':
                print line[:-2]+'\n',
            else:
                print line,
        except:
            print line,

def ConvertFileU2D(Filelist):
    """
    ConvertFileU2D(Filelist)

    Converts a [Filename] from \n to \r\n inplace no effect if the line termination is correct

    Arguments:
    =========
    Filelist: a file or list of files to convert

    """
    for line in fileinput.input(Filelist,inplace=1):
        try:
            if line[-2] != '\r':
                print line[:-1]+'\n',
            else:
                print line,
        except:
            print '\n',

