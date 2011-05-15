import sys,os

bindir = "\"@TINKERCELL_BINARY_DIR@\""
bindir = bindir.replace("/","\\")

os.system("cd " + bindir)
os.system("mingw32-make package")

root = "@TINKERCELL_BINARY_DIR@/@TINKERCELL_PACKAGE_FOLDER@/TinkerCell"
path = os.path.join(root, "")

s = "\
[InstallDelete]\n\
Name: {app}; Type: filesandordirs\n\
[Run]\n\
Filename: \"msiexec.exe\"; Parameters: \"$i \"\"{app}\\installation\\Slik-Subversion-1.6.16-win32.msi\"\"\"; Description: \"install Subversion (for updating plugins)\"; WorkingDir: {app}; Flags: postinstall\n\
Filename: \"msiexec.exe\"; Parameters: \"$i \"\"{app}\\installation\\python-2.5.4.msi\"\"\"; Description: \"install Python\"; WorkingDir: {app}; Flags: postinstall\n\
Filename: {app}\\installation\\setuptools-0.6c11.win32-py2.5.exe; Description: \"install Python easy_install\"; WorkingDir: {app}; Flags: postinstall unchecked\n\
Filename: C:\\Python25\\Scripts\\easy_install.exe; Parameters: scipy; Description: \"install Scientific Python\"; WorkingDir: C:\\Python25\\Scripts; Flags: postinstall unchecked\n\
Filename: C:\\Python25\\Scripts\\easy_install.exe; Parameters: simplejson; Description: \"install JSON package\"; WorkingDir: C:\\Python25\\Scripts; Flags: postinstall unchecked\n\
Filename: C:\\Python25\\Scripts\\easy_install.exe; Parameters: biopython; Description: \"install BioPython package\"; WorkingDir: C:\\Python25\\Scripts; Flags: postinstall unchecked\n\
Filename: {app}\\installation\\pysces-0.7.0.win32-py2.5.exe; Description: \"install Python Cell Simulation package\"; WorkingDir: {app}; Flags: postinstall unchecked\n\
Filename: {app}\\@TINKERCELL_EXE@.exe; Description: \"run @TINKERCELL_EXE@\"; WorkingDir: {app}; Flags: postinstall\n\
[Icons]\n\
Name: {group}/@TINKERCELL_EXE@; Filename: {app}/@TINKERCELL_EXE@.exe; WorkingDir: {app}; Comment: @TINKERCELL_EXE@; Flags: createonlyiffileexists\n\
Name: {userdesktop}/@TINKERCELL_EXE@; Filename: {app}/@TINKERCELL_EXE@.exe; WorkingDir: {app}; Comment: @TINKERCELL_EXE@; Flags: createonlyiffileexists; Tasks: desktopicon\n\
[UninstallDelete]\n\
Name: {app}; Type: filesandordirs\n\
[Setup]\n\
AppName=@TINKERCELL_EXE@\n\
RestartIfNeededByRun=false\n\
AppVerName=@TINKERCELL_EXE@ 1.@TINKERCELL_VERSION@\n\
DefaultDirName={pf}/@TINKERCELL_EXE@\n\
OutputDir=@TINKERCELL_BINARY_DIR@/win32\n\
OutputBaseFilename=@TINKERCELL_EXE@Setup\n\
DefaultGroupName=Synthetic Biology\n\
[Tasks]\n\
Name: desktopicon; Description: Create a &desktop icon; GroupDescription: Additional icons:\n\
[Files]\n"

for root, dirs, files in os.walk(path):
    for fileName in files:
        if root.count(".svn") < 1:
            dest = root.replace("@TINKERCELL_BINARY_DIR@/@TINKERCELL_PACKAGE_FOLDER@/TinkerCell","")
            s += ("Source: " + root + os.sep + fileName + "; DestDir: {app}" + dest + "\n")

s = s.replace("/","\\")
s = s.replace("\\\\","\\")
s = s.replace("$","/")
f = open("@TINKERCELL_BINARY_DIR@/win32/TinkerCellSetup.iss","w")
f.write(s)
f.close()

print "Creating installer using Inno setup file: @TINKERCELL_BINARY_DIR@\\win32\\TinkerCellSetup.iss"

os.system("cd " + bindir)
os.system("cd win32")
innoexe = "\"@INNO@\""
innoexe = innoexe.replace("/","\\")

os.system(innoexe + " TinkerCellSetup.iss")

if "@TINKERCELL_BINARY_UPLOAD@" == "ON":
    print "Uploading @TINKERCELL_EXE@Setup.exe using WinSCP...\n"
    winscpexe = "\"@WINSCP@\""
    winscpexe = winscpexe.replace("/","\\")
    os.system(winscpexe + " /script=uploadTinkerCell.winscp")
