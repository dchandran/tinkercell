===============================
HOW TO BUILD TINKERCELL PROJECT
===============================

YOU WILL NEED TO INSTALL THE FOLLOWING LIBRARIES/PACKAGES
-----------------------------------------------------------------------------------------
C and C++ Compiler (XCode on Mac,  MinGW in Windows, GCC in Linux)
CMake (www.cmake.org)
Qt 4.5 or higher
libxml2
python (optional)
octave (optional)


COMPILING TINKERCELL
-------------------------------
1. Install the following libraries: libxml2, libxml2-dev , libx11-dev, python-dev, subversion, octave
      In Linux, just use apt-get install libxml2 libxml2-dev libx11-dev python-dev subversion octave octave-headers

2.  Install cmake, available from www.cmake.org
      In Linux, use apt-get install cmake cmake-qt-gui. 

3. Install Qt.
      In Linux, use apt-get install libqt4-core libqt4-gui libqt4-xml libqt4-opengl qt4-dev-tools.

     On other platforms, do the following:
         1. download and install Qt 4.5.0 (or later) from http://qt.nokia.com/products
         2. extract the Qt compressed file and inside the folder, run the following command:
               1. ./configure -debug-and-release -no-accessibility -no-sql-db2 -no-sql-ibase -no-sql-mysql -no-sql-oci -no-sql-odbc -no-sql-psql -no-sql-sqlite -no-sql-sqlite2 -no-sql-tds -no-mmx -no-3dnow -no-sse -no-sse2 -qt-zlib -qt-gif -qt-libtiff -qt-libpng -qt-libmng -qt-libjpeg -no-openssl -no-nis -iconv -no-qdbus -no-nas-sound -opengl -no-sm -no-xshape -no-xinerama -no-xcursor -no-xfixes -no-xrandr -no-xrender -no-fontconfig -no-xkb
               2. make  (this will take time)
               3. make install  (this will also take time)

4. Download TinkerCell source: http://sourceforge.net/projects/tinkercell/files/TinkerCellSource.tgz/download

5. Run cmake-gui

6. In the cmake-gui, select the ~/tinkercell/trunk folder for the source folder and ~/tinkercell/trunk/BUILD as the binary folder

7. Run "Configure". When you run the first time, you will be asked to select the compiler you want to use.

8. If libxml2 is not found, set the following manually (using the Add Entry button in cmake-gui):
         1. LIBXML2_INCLUDE_DIR = /usr/include/libxml2
         2. LIBXML2_LIBRARIES = /usr/lib/libxml2.so.2.7.5   (or whichever file you have)
    In Windows, TinkerCell comes with libxml2 libraries, so this step will not be needed

9. Run "Configure" a few times until all the redness disappears

10. Run "Generate". This will create the makefile or the project file, depending on the compiler you selected

11. Go to the ~/tinkercell/trunk/BUILD folder

12. Run "make package" or open the project file and compile (this will take time). 
     MAC ONLY: Just do "make" because "make package" does not do package correctly. 
     
13. MAC ONLY: After performing make, go to the bin folder and do "source do_name_change.sh"

14. Linux and Windows, go to _CPack_Packages/ folder and find the TinkerCell subfolder. 
      Windows, run TinkerCell.exe
      Linux: Go to the bin folder and create a script:
                    export LD_LIBRARY_PATH=<tinkercell folder>:<tinkercell folder>/plugins:<octave libraries folder>:<python libraries folder>
                    <tinkercell folder>/TinkerCell
          and then run    
                     "source run_tinkercell"

      Mac:  open the TinkerCell.app file in the bin folder


WIN32 PROBLEMS
--------------------------
Problem: CMake is not able to find MinGW. 
Solution: Look for a field in the CMake GUI called CMAKE_CXX_COMPILER. 
             Set the compiler manually by locating the mingw32-make.exe. 
             Click "configure" again.

Problem: Visual Studio is giving link errors
Solution: let me know if you find a solution!


