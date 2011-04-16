import sys,os

root = "@TINKERCELL_BINARY_DIR@/@TINKERCELL_PACKAGE_FOLDER@"
path = os.path.join(root, "")

s = "\
[InstallDelete]\n\
Name: {app}; Type: filesandordirs\n\
[Run]\n\
Filename: \"msiexec.exe\"; Parameters: \"$i \"\"{app}\\Slik-Subversion-1.6.16-win32.msi\"\"\"; Description: \"Install Subversion (for updating plugins)\"; WorkingDir: {app}; Flags: postinstall\n\
Filename: \"msiexec.exe\"; Parameters: \"$i \"\"{app}\\python-2.5.4.msi\"\"\"; Description: \"Install Python\"; WorkingDir: {app}; Flags: postinstall\n\
Filename: {app}\\numpy-1.3.0-win32-superpack-python2.5.exe; Description: \"Install Python Numerical Pack\"; WorkingDir: {app}; Flags: postinstall\n\
Filename: {app}\\scipy-0.7.1-win32-superpack-python2.5.exe; Description: \"Install Python Scientific Pack\"; WorkingDir: {app}; Flags: postinstall\n\
Filename: {app}\\pysces-0.7.0.win32-py2.5.exe; Description: \"Install PySCeS\"; WorkingDir: {app}; Flags: postinstall\n\
Filename: {app}\\@TINKERCELL_EXE@.exe; WorkingDir: {app}; Flags: postinstall\n\
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
            dest = root.replace("@TINKERCELL_BINARY_DIR@/@TINKERCELL_PACKAGE_FOLDER@","")
            s += ("Source: " + root + os.sep + fileName + "; DestDir: {app}" + dest + "\n")

s = s.replace("/","\\")
s = s.replace("\\\\","\\")
s = s.replace("$","/")
f = open("@TINKERCELL_BINARY_DIR@/win32/TinkerCellSetup.iss","w")
f.write(s)

