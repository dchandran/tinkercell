======================================
HOW TO BUILD TINKERCELL PROJECT
======================================

----------------------------------------------------------------------------------------
YOU WILL NEED TO INSTALL THE FOLLOWING LIBRARIES/PACKAGES
-----------------------------------------------------------------------------------------
C and C++ Compiler (XCode on Mac,  MinGW in Windows, GCC in Linux)
CMake (www.cmake.org)
Qt 4.5 or higher
python (optional)
octave (optional)

-------------------------------
COMPILING TINKERCELL
-------------------------------
1. Install the following libraries: libx11-dev, python-dev, subversion, octave
      In Linux, just use apt-get install libx11-dev python-dev subversion octave octave-headers

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

6. In the cmake-gui, select the tinkercell/trunk folder for the source folder and tinkercell/trunk/BUILD as the binary folder

7. Run "Configure". When you run the first time, you will be asked to select the compiler you want to use. Select the one that suits you. For Windows, MinGW seems to work best, but you are welcome to try Visual Studio.

8. This step only applies if you want embedded Octave or Python.
In the CMake window, change the "Simple View" to "Grouped View". This will help you identify whether or not all the directories or libraries that are needed were found. If you want embedded Python and Octave, enable them under the EMBED group. In the PYTHON group, check that the libraries and include directories are found. If they are not found, you will have to set them. Do that save for the OCTAVE group.

9. If any of the entried in the CMake window are highlighted in red, run "Configure" again.

10. Run "Generate". This will create the makefile or the project file, depending on the compiler you selected in step 7.

11. Go to the ~/tinkercell/trunk/BUILD folder

12. Run "make package" or open the project file and build the "package" project (this will take time). 
     MAC ONLY: Just do "make" because "make package" does not package correctly. 
     
13. MAC ONLY: After performing make, go to the bin folder and do "source do_name_change.sh"

14. Linux and Windows, go to _CPack_Packages/ folder and find the TinkerCell subfolder. 
      Windows, run TinkerCell.exe
      Linux: Go to the bin folder and create a script:
                    export LD_LIBRARY_PATH=<tinkercell folder>:<tinkercell folder>/plugins:<octave libraries folder>:<python libraries folder>
                    <tinkercell folder>/TinkerCell
          and then run    
                     "source run_tinkercell"

      Mac:  open the TinkerCell.app file in the bin folder

--------------------------
WIN32 PROBLEMS
--------------------------
Problem: CMake is not able to find MinGW. 
Solution: Look for a field in the CMake GUI called CMAKE_CXX_COMPILER. 
             Set the compiler manually by locating the mingw32-make.exe. 
             Click "configure" again.

Problem: CMake is not able to find qmake.
Solution: Look for a field in the CMake GUI called QT_QMAKE_EXECUTABLE. 
             Set the compiler manually by locating the qmake.exe. 
             Click "configure" again.

Problem: Visual Studio is giving link errors
Solution: let me know if you find a solution!


