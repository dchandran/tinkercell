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
__doc__ = '''
          PyscesLink
          ----------

          Interfaces to external software and API's, has replaced the PySCeS contrib classes.
          '''

# for METATOOLlink
import os, re, cStringIO
# for SBWWebLink
import urllib, urllib2, getpass

class SBWlink(object):
    """Generic access for local SBW services using SBWPython """
    sbw = None
    psbw = None
    sbwModuleProxy = None
    moduleDict = None
    modules = None

    def __init__(self):
        try:
            import SBW as SBW
            import SBW.psbw as psbw
            ##  reload(SBW)
            ##  reload(psbw)
            self.sbw = SBW
            self.psbw = SBW.psbw
            self.sbwModuleProxy = SBW.sbwModuleProxy
            self.moduleDict = SBW.sbwModuleProxy.moduleDict
            self.modules = []
            for m in self.moduleDict:
                if self.moduleDict[m].pythonName not in ['python']:
                    self.SBW_exposeAll(self.moduleDict[m])
                    self.modules.append(self.moduleDict[m].pythonName)
                    setattr(self, self.moduleDict[m].pythonName, self.moduleDict[m])
            print '\nSBWlink established.'
        except Exception, ex:
            print ex
            print '\nSBWlink not established.'

    def SBW_exposeAll(self, module):
        for s in module.services:
            s = getattr(module, s)
            for m in s.methods:
                getattr(s, m)

    def SBW_getActiveModules(self):
        idlist = []
        namelst = []
        for id in self.psbw.getModuleIdList():
            idlist.append(id)
            namelst.append(self.psbw.getModuleName(id))
        for id in self.moduleDict.keys():
            if id not in idlist:
                self.moduleDict.pop(id)
        for name in range(len(self.modules)-1,-1,-1):
            if self.modules[name] not in namelst:
                delattr(self, self.modules[name])
                self.modules.pop(name)
        for name in namelst:
            if name not in self.modules:
                self.SBW_loadModule(name)
        return namelst

    def SBW_loadModule(self, module_name):
        ans = 'Y'
        if module_name[-3:] == 'GUI':
            ans = raw_input('Warning! This may hang the console\n\yPress \'Y\' to continue: ')
        if ans == 'Y':
            module_id = self.psbw.SBWGetModuleInstance(module_name)
            assert module_id != None, '\nUnknow module, %s' % module_name
            module = self.sbwModuleProxy.ModuleProxy(module_id)
            self.SBW_exposeAll(module)
            if not self.moduleDict.has_key(module_id):
                print '<PySCeS_SBW> Adding ' + module.pythonName + ' to ModuleProxy (id=' + str(module_id) + ')'
                self.moduleDict.update({module_id : module})
            if module.pythonName not in self.modules:
                print '<PySCeS_SBW> Adding ' + module.pythonName + ' to SBWlink'
                self.modules.append(module.pythonName)
                setattr(self, module.pythonName, module)
        else:
            print '\nModule %s not loaded' % module_name

class SBWLayoutWebLink(object):
    """Enables access to DrawNetwork and SBMLLayout web services at www.sys-bio.org"""
    sbwhost = '128.208.17.26'
    sbml = None
    sbmllayout = None
    svg = None
    DEBUGMODE = False
    DEBUGLEVEL = 1
    DRAWNETWORKLOADED = False
    LAYOUTMODULELOADED = False

    def setProxy(self, **kwargs):
        """Set as many proxy settings as you need. You may supply a user name without
        a password in which case you will be prompted to enter one (once) when required
        (NO guarantees, implied or otherwise, on password security AT ALL). Arguments can be:

        user = 'daUser',
        pwd = 'daPassword',
        host = 'proxy.paranoid.net',
        port = 3128
        """
        proxy_info = {}
        for k in kwargs.keys():
            proxy_info.update({k : kwargs[k]})

        if proxy_info.has_key('user') and not proxy_info.has_key('pwd'):
            proxy_info.update({'pwd' : getpass.getpass()})
        proxy_support = urllib2.ProxyHandler({"http" :
                    "http://%(user)s:%(pwd)s@%(host)s:%(port)d" % proxy_info})
        opener = urllib2.build_opener(proxy_support, urllib2.HTTPHandler)
        urllib2.install_opener(opener)
        del proxy_info, proxy_support

    def loadSBMLFileFromDisk(self, File, Dir=None):
        if Dir != None:
            path = os.path.join(Dir, File)
        else:
            path = File
        if os.path.exists(path):
            self.sbmllayout = None
            self.svg = None
            self.DRAWNETWORKLOADED = False
            self.LAYOUTMODULELOADED = False
            sbmlF = file(path, 'r')
            self.sbml = sbmlF.read()
            sbmlF.close()
            return True
        else:
            print "%s is an invalid path" % path
            return False

    def loadSBMLFromString(self, str):
        self.sbmllayout = None
        self.svg = None
        self.DRAWNETWORKLOADED = False
        self.LAYOUTMODULELOADED = False
        self.sbml = str
        return True

    def urlGET(self, host, urlpath):
        url = 'http://%s%s' % (host,urlpath)
        con = urllib2.urlopen(url)
        resp = con.read()
        if self.DEBUGMODE:
            print con.headers
        if self.DEBUGMODE and self.DEBUGLEVEL == 2:
            print resp
        con.close()
        return resp

    def urlPOST(self, host, urlpath, data):
        assert type(data) == dict, '\nData must be a dictionary'
        url = 'http://%s%s' % (host, urlpath)
        con = urllib2.urlopen(url, urllib.urlencode(data))
        resp = con.read()
        if self.DEBUGMODE:
            print con.headers
        if self.DEBUGMODE and self.DEBUGLEVEL == 2:
            print resp
        con.close()
        return resp

    def getVersion(self):
        print 'Inspector.getVersion()'
        ver = self.urlGET(self.sbwhost, '/generate/Inspector.asmx/getVersion')
        ver = ver.replace('<?xml version="1.0" encoding="utf-8"?>','')
        ver = ver.replace('<string xmlns="http://www.sys-bio.org/">','')
        ver = ver.replace('</string>','')
        return ver

    def drawNetworkLoadSBML(self):
        print 'DrawNetwork.loadSBML()'
        assert self.sbml != None, '\nNo SBML file loaded'
        data = {'var0' : self.sbml}
        self.DRAWNETWORKLOADED = True
        return self.urlPOST(self.sbwhost, '/generate/DrawNetwork.asmx/loadSBML', data)

    def drawNetworkGetSBMLwithLayout(self):
        print 'DrawNetwork.getSBML()'
        assert self.DRAWNETWORKLOADED, '\nSBML not loaded into DrawNetwork module'
        sbml = self.urlGET(self.sbwhost, '/generate/DrawNetwork.asmx/getSBML')
        sbml = sbml.replace('&gt;','>')
        sbml = sbml.replace('&lt;','<')
        sbml = sbml.replace('''<string xmlns="http://www.sys-bio.org/"><?xml version="1.0" encoding="utf-8"?>''','')
        sbml = sbml.replace('</string>','')
        self.sbmllayout = sbml
        return True

    def layoutModuleLoadSBML(self):
        print 'SBMLLayoutModule.loadSBML()'
        assert self.sbmllayout != None, '\nNo SBML Layout loaded'
        data = {'var0' : self.sbmllayout}
        self.LAYOUTMODULELOADED = True
        return self.urlPOST(self.sbwhost, '/generate/SBMLLayoutModule.asmx/loadSBML', data)

    def layoutModuleGetSVG(self):
        assert self.LAYOUTMODULELOADED, '\nSBML not loaded into SBMLLayout module'
        svg = self.urlGET(self.sbwhost, '/generate/SBMLLayoutModule.asmx/getSVG')
        svg = svg.replace('&gt;','>')
        svg = svg.replace('&lt;','<')
        svg = svg.replace('''<string xmlns="http://www.sys-bio.org/">''','')
        svg = svg.replace('''<?xml version="1.0" encoding="utf-8"?>''','')
        svg = svg.replace('</string>','')
        self.svg = svg
        return True

    def getSBML(self):
        return self.sbml

    def getSBMLlayout(self):
        return self.sbmllayout

    def getSVG(self):
        return self.svg


class METATOOLlink(object):
    """New interface to METATOOL binaries"""

    __metatool_path__ = None
    __mod__ = None
    __emode_exe_int__ = None
    __emode_exe_dbl__ = None
    __emode_intmode__ = 0
    __emode_userout__ = 0
    __emode_file__ = None
    __metatool_file__ = None
    #EModes = ''

    def __init__(self, mod, __metatool_path__=None):
        # Initialise elementary modes
        self.__mod__ = mod
        if __metatool_path__ == None:
            self.__metatool_path__ = os.path.join(mod.__pysces_directory__, 'metatool')
        else:
            self.__metatool_path__ = os.path.join(__metatool_path__, 'metatool')
        assert self.__metatool_path__ != None, '\nPySCeS not found'

        self.__emode_file__ = self.__mod__.ModelFile[:-4] + '_emodes'
        self.__metatool_file__ = self.__mod__.ModelFile[:-4] + '_metatool'
        if os.sys.platform == 'win32':
            self.__emode_exe_int__ = os.path.join(self.__metatool_path__,'meta43_int.exe')
            self.__emode_exe_dbl__ = os.path.join(self.__metatool_path__,'meta43_double.exe')
        else:
            self.__emode_exe_int__ = os.path.join(self.__metatool_path__,'meta43_int')
            self.__emode_exe_dbl__ = os.path.join(self.__metatool_path__,'meta43_double')

        if os.path.exists(self.__emode_exe_int__):
            print 'Using METATOOL int',
            self.__emode_intmode__ = True
        else:
            self.__emode_exe_int__ = None
        if os.path.exists(self.__emode_exe_dbl__):
            print '\b\b\b\bdbl'
            self.__emode_intmode__ = False
        else:
            self.__emode_exe_dbl__ = None
        assert self.__emode_exe_dbl__ != None or self.__emode_exe_int__ != None, "\nMETATOOL binaries not available"

    def doEModes(self):
        """
        doEModes()

        Calculate the elementary modes by way of an interface to MetaTool.

        METATOOL is a C program developed from 1998 to 2000 by Thomas Pfeiffer (Berlin)
        in cooperation with Stefan Schuster and Ferdinand Moldenhauer (Berlin) and Juan Carlos Nuno (Madrid).
        http://www.biologie.hu-berlin.de/biophysics/Theory/tpfeiffer/metatool.html

        Arguments:
        None

        """
        print 'METATOOL is a C program developed from 1998 to 2000 by Thomas Pfeiffer (Berlin)'
        print 'in cooperation with Stefan Schuster and Ferdinand Moldenhauer (Berlin) and Juan Carlos Nuno (Madrid).'
        print 'http://www.biologie.hu-berlin.de/biophysics/Theory/tpfeiffer/metatool.html'

        goMode = 0
        fileIn = 'pysces_metatool.dat'
        fileOut = 'pysces_metatool.out'

        goMode = 1
        if goMode == 1:
            # Build MetaTool input file
            File = open(os.path.join(self.__mod__.ModelOutput,fileIn),'w')
            # Determine type of reaction
            out1 = []
            for key in self.__mod__.__nDict__:
                #print key
                #print self.__mod__.__nDict__[key]['Type']
                out1.append((key,self.__mod__.__nDict__[key]['Type']))
            #print '\nExtracting metatool information from network dictionary ...\n'
            File.write('-ENZREV\n')
            for x in out1:
                if x[1] == 'Rever':
                    File.write(x[0] + ' ')
            File.write('\n\n')
            File.write('-ENZIRREV\n')
            for x in out1:
                if x[1] == 'Irrev':
                    File.write(x[0] + ' ')
            File.write('\n\n')
            File.write('-METINT\n')
            for x in self.__mod__.__species__:
                File.write(x + ' ')
            File.write('\n\n')
            File.write('-METEXT\n')
            for x in self.__mod__.__fixed_species__:
                File.write(x + ' ')
            File.write('\n\n')

            output = []
            allInt = 1
            for x in self.__mod__.__nDict__:
                reList = self.__mod__.__nDict__[x]['Reagents']
                subs = ''
                prods = ''
                #print 'Reaction: ' + x
                for y in reList:
                    if self.__emode_intmode__ == 1: # use int elementary modes
                        if abs(int(reList[y]))/abs(float(reList[y])) != 1.0:
                            print 'INFO: Coefficient not integer = ' + `reList[y]`
                            allInt = 0
                        if reList[y] < 0:
                            #print y.replace('self.','') + ' : substrate'
                            if abs(int(reList[y])) != 1:
                                subs += `abs(int(reList[y]))` + ' '
                            subs += y.replace('self.','')
                            subs += ' + '
                        else:
                            #print y.replace('self.','') + ' : product '
                            if abs(int(reList[y])) != 1:
                                prods += `abs(int(reList[y]))` + ' '
                            prods += y.replace('self.','')
                            prods += ' + '
                        #output.append(x + ' : ' + subs[:-3] + ' = ' + prods[:-3] + ' .')
                    else: # use float/double elementary mode
                        if reList[y] < 0.0:
                            #print y.replace('self.','') + ' : substrate'
                            if abs(float(reList[y])) != 1.0:
                                subs += `abs(float(reList[y]))` + ' '
                            subs += y.replace('self.','')
                            subs += ' + '
                        else:
                            #print y.replace('self.','') + ' : product '
                            if abs(float(reList[y])) != 1.0:
                                prods += `abs(float(reList[y]))` + ' '
                            prods += y.replace('self.','')
                            prods += ' + '
                output.append(x + ' : ' + subs[:-3] + ' = ' + prods[:-3] + ' .')

            File.write('-CAT\n')
            for x in output:
                File.write(x + '\n')
            File.write('\n')

            File.flush()
            File.close()

            if allInt == 1:
                if self.__emode_intmode__ == 1:
                    eModeExe = self.__emode_exe_int__
                else:
                    eModeExe = self.__emode_exe_dbl__

                print '\nMetatool running ...\n'
                ######### UPDATE:
                # Actually works fine on windows and posix - johann 20081128
                print 'Generic run'
                os.spawnl(os.P_WAIT, eModeExe, eModeExe, os.path.join(self.__mod__.ModelOutput,fileIn), os.path.join(self.__mod__.ModelOutput,fileOut))
                print '\nMetatool analysis complete\n'

                # Parse MetaTool output file and store the result in a string
                go = 0
                go2  = 0
                result = ''
                end = ''

                try:
                    file2 = open(os.path.join(self.__mod__.ModelOutput,fileOut), 'r')
                    for line in file2:
                        c = re.match('ELEMENTARY MODES',line)
                        d = re.match(' enzymes',line)
                        e = re.match('The elementary mode',line)
                        f = re.match('\n',line)
                        g = re.match('The elementary',line)
                        if c != None:
                            go = 1
                            go2 = 0
                        if d != None:
                            go2 = 1
                        if e != None:
                            go2 = 0
                        if go == 1 and go2 == 1 and f == None:
                            line = line.replace('reversible','\n  reversible\n')
                            line = line.replace('ir\n  ','\n  ir')
                            if self.__emode_intmode__ == 1:
                                line = line.replace('] ',']\n ')
                            else:
                                line = line.replace(') ',')\n ',1)
                            result += line
                        if go == 1 and g != None:
                            end += line
                    result += end
                    result += '\n'

                    file2.close()

                    if self.__emode_userout__ == 1:
                        fileo = open(os.path.join(self.__mod__.ModelOutput,self.__metatool_file__) + '.in','w')
                        filer = open(os.path.join(self.__mod__.ModelOutput,fileIn),'r')
                        for line in filer:
                            fileo.write(line)
                        fileo.write('\n\n')
                        filer.close()
                        fileo.close()
                        filer = open(os.path.join(self.__mod__.ModelOutput,fileOut),'r')
                        fileo = open(os.path.join(self.__mod__.ModelOutput,self.__metatool_file__) + '.out','w')
                        for line in filer:
                            fileo.write(line)
                        filer.close()
                        fileo.close()
                    os.remove(os.path.join(self.__mod__.ModelOutput,fileIn))
                    os.remove(os.path.join(self.__mod__.ModelOutput,fileOut))
                except Exception, EX:
                    print 'doEmode:', EX
                    print 'WARNING: Unable to open MetaTool output file\nPlease check the MetaTool executables: '
                    if os.name == 'posix':
                        print '/MetaTool/meta43_double /MetaTool/meta43_int\nand their permissions'
                    else:
                        print '/MetaTool/meta43_double.exe /MetaTool/meta43_int.exe'
            else:
                print '\nINFO: non-integer coefficients\
                \nTry using the double eMode function: self.__emode_intmode__=0'
                result = 'Elementary modes not calculated\n'
        else:
            print '\nNo elementary mode calculation possible - no meta43_xxx.exe'
            result = 'Elementary modes not calculated\n'
        self.EModes = result

    def getEModes(self):
        """
        getEModes()

        Returns the elementary modes as a linked list of fluxes

        """
        try:
            a = self.EModes
            FF = cStringIO.StringIO()
            FF.write(self.EModes)
            FF.reset()
            output = []
            for line in FF:
                if re.match('  ',line) and not re.match('  reversible',line) and not re.match('  irreversible',line):
                    tmp = [el for el in line.replace('\n','').split(' ') if el != '']
                    tmpOut = []
                    skip = False
                    for el in range(len(tmp)):
                        if skip:
                            skip = False
                        elif tmp[el][0] != '(':
                            tmpOut.append(tmp[el])
                        elif tmp[el][0] == '(':
                            tmpOut.append(tmp[el]+')'+tmp[el+1][:-1])
                            skip = True
                    output.append(tmpOut)
            return output
        except AttributeError, atx:
            print atx
            print '\nINFO: Please run doEModes() first\n'

    def showEModes(self,File=None):
        """
        showEModes(File=None)

        Print the results of an elementary mode analysis, generated with doEModes(),
        to screen or file.

        Arguments:
        File [default=None]: Boolean, if True write parsed elementary modes to file

        """
        try:
            if File != None:
                #assert type(File) == file, 'showEmodes() needs an open file object'
                print '\nElementary modes written to file\n'
                f = open(os.path.join(self.__mod__.ModelOutput,self.__emode_file__ + '.out'),'w')
                f.write('\n## Elementary modes\n')
                f.write(self.EModes)
                f.close()
            else:
                print '\nElementary modes\n'
                print self.EModes
        except AttributeError, atx:
            print atx
            print '\nINFO: Please run doEModes() first\n'





















