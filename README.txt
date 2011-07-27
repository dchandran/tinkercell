===============================================================
HOW TO BUILD TINKERCELL PROJECT FROM SOURCE CODE
===============================================================

             Table of Contents
     -------------------------------------
     Required Libraries and Packages
     Building TinkerCell
	 Win32 Problems and Solutions
	 What Each Folder Contains
     
====================================
REQUIRED LIBRARIES & PACKAGES
====================================

C and C++ Compiler (see below for recommended compilers)
CMake (www.cmake.org)
Qt 4.5 or higher (qt.nokia.com/products/)

OPTIONAL (recommended):
Python -- installers are located in win32 folder
GNU Octave (optional) -- download the installer from www.octave.org

WINDOWS ONLY:
IF you want to automatically create the Windows installer and/or upload the installer to Sourceforce (developers),
then you will also need to install Inno Setup and WinSCP. You will also need to specify the location of
these executables in the CMake GUI (details given in step 12 below)

UBUNTU NATTY NARWHAL ISSUES:
Apparently, the following libraries have been moved out of /usr/lib and not found in the library path:
libSM.* libz.* libICE.* libX11.*
Copy them from /usr/lib/i386-linux-gnu or /usr/lib/x86_64-linux-gnu into /usr/lib

RECOMMENDED C/C++ COMPILERS:
XCode on Mac
MinGW in Windows (MS Visual Studio also works, but there might be some glicthes)
GCC in Linux


====================================
BUILDING TINKERCELL
====================================

1. Install the following libraries or packages: python, subversion, octave (optional)
In Linux, just use apt-get install libx11-dev python-dev subversion octave octave-headers


2. Install cmake, available from www.cmake.org
In Linux, use apt-get install cmake cmake-qt-gui


3. Install Qt (4.5 or above)
In Linux, use apt-get install libqt4-core libqt4-gui libqt4-xml libqt4-opengl qt4-dev-tools

In Windows and Mac, download the QtSDK installer and just follow the normal installation process

On all other platforms, do the following:
3a. download and install Qt 4.5.0 (or later) from http://qt.nokia.com/products
3b. extract the Qt compressed file and inside the folder, run the following command:
3c. ./configure -debug-and-release -no-accessibility -no-sql-db2 -no-sql-ibase -no-sql-mysql -no-sql-oci -no-sql-odbc -no-sql-psql -no-sql-sqlite -no-sql-sqlite2 -no-sql-tds -no-mmx -no-3dnow -no-sse -no-sse2 -qt-zlib -qt-gif -qt-libtiff -qt-libpng -qt-libmng -qt-libjpeg -no-openssl -no-qdbus -opengl -no-sm -no-xshape -no-xinerama -no-xcursor -no-xfixes -no-xrandr -no-xrender -no-fontconfig -no-xkb
3d. make (this will take time)
3e. make install (this will also take time)

4. Run cmake-gui (in Windows, double click on the CMake icon on the desktop)

5. In the CMake window,
set tinkercell/trunk folder for the source folder
set tinkercell/trunk/BUILD as the binary folder
change the "Simple View" to "Grouped View" (located next to the search box)


6. Inside the cmake-gui, click the "Configure" button.
When you run this for the first time, you will be asked to select a compiler of your choice.
Suggested compilers: GCC for Linux and Mac (aka. Unix default), MinGW for Windows


7. (optional) This step only applies if you want enable Octave, Python, or Ruby.
This will help you identify whether or not all the directories or libraries that are needed were found.
If you want embedded Python and Octave, enable them under the EMBED group.
In the PYTHON group, check that the libraries and include directories are found.
If they are not found, you will have to set them. Do that same for the OCTAVE group.
Octave headers are problematic in Windows; see the "win32 problems" section near the end of this document


8. (optional) Select one of the TinkerCell lite versions from the TINKERCELL group if you are interested in a different variant of TinkerCell


9. If the "Generate" button is disabled, click the "Configure" button again


10. After Configure is done, click the "Generate" button.
This will create the makefile or the project file, depending on the compiler you selected in step 7.
Pay attention to the messages in red in the CMake window (not all of them are errors)

THE REST OF THE STEPS DEPEND ON YOUR OPERATING SYSTEM. Follow steps in one of the sections below

--------------------- Linux ------------------------------------
12. Be sure that CPACK_TGZ is selected in the CMake gui
13. Go to BUILD/
14. Run "make package"
15. Run BUILD/_CPack_Packages/Linux/TGZ/run_tinkercell.sh

--------------------- Max ------------------------------------
12. Run BUILD/bin/create_bundled_app.sh
The program will be visible on the desktop

--------------------- Windows ------------------------------------
If you want to create an installer, then pick option 12(a). If you just want to run TinkerCell, select option 12(b)

12(a). If you want to create the TinkerCell installation file using Inno setup, be sure that the
check the TINKERCELL_INSTALLER checkbox in the TINKERCELL group located in the CMake GUI window
If you want to upload the installer to Sourceforge automatically, check the TINKERCELL_INSTALLER_UPLOAD
You must also have Python installed, because a Python script is used to generate the Inno setup file.
Run BUILD/win32/CreateInstaller.bat

12(b). Go to the BUILD folder are run "mingw32-make package"
Run TinkerCell executable located inside /BUILD/_CPack_Packages/.../TinkerCell

--------------------- Other operating systems ------------------------------------
12. Go to BUILD/
13. Run "make package"
14. Go to _CPack_Packages folder and find the TinkerCell subfolder.
Create the following script and save it as "run_tinkercell"
#!/bin/bash
export LD_LIBRARY_PATH=<tinkercell folder>:<tinkercell folder>/plugins:<octave libraries folder>:<python libraries folder>
<tinkercell folder>/TinkerCell
15. Run run_tinkercell


====================================
WIN32 PROBLEMS & SOLUTIONS
====================================

Problem: CMake is not able to find MinGW.
Solution: Look for a field in the CMake GUI called CMAKE_CXX_COMPILER.
Set the compiler manually by locating the mingw32-make.exe.
Click "configure" again.


Problem: CMake is not able to find qmake.
Solution: Look for a field in the CMake GUI called QT_QMAKE_EXECUTABLE.
Set the compiler manually by locating the qmake.exe.
Click "configure" again.

Problem: Compile issues due to octave header files
Solution: open the config.h find in the Octave include folder and comment 
the #define HAVE_HDF5 and HAVE_REGEX lines (unless you have these packages 
installed, which is not included with MinGW).

Problem: Visual Studio is giving link errors when linking against TinkerCellCore
Solution: Try adding all the MOC files from TinkerCellCore as source files into the project that you are building. 
MOC files will have a moc_ before their names and .cxx suffix. They will be located in the Build folder.


=====================================
   WHAT EACH FOLDER CONTAINS
=====================================
Main: the main executable and qss style file

icons: all icons used inside the Core library. The qrc file in Core is generated using the perl script in icons/

Core = TinkerCellCore library
Core/plots = plotting classes in the Core library
Core/plugins = some basic plugins such as alignment tools and slider windows
Core/fileIO = classes for reading and writing TinkerCell files (generic)
Core/interpreters = classes for embedding Python or Octave inside TinkerCell
Core/coding = classes that use the Core/interpreters to enable C, Python, and Octave within TinkerCell and provide a nice coding window

python = contains C code for embedding python (required by Python interpreter in Core library)
             also contains all the python scripts used in the final program (not as important)
octave = contains C++ code for embedding Octave (required by Octave interpreter in Core library)
             also contains all the octave scripts used in the final program (not as important)
plugins folders:

Plugins:
BasicTools = inserting, selecting, appearance dialog, etc.
NodesTree = nodes and connections catalog files and supporting classes
GeneticNetwork = classes that are specific for genetic networks and aligning parts (linear or circular)
ImportExportTools = classes that import or export TinkerCell models (SBML, Copasi, Antimony, plain English, Matlab)
OtherTools = classes which don't belong to any other category

NodeGraphics = graphics drawing program used to generate everything in Graphics

Graphics = folder containing all graphical files used in TinkerCell



