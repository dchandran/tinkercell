===============================
HOW TO BUILD TINKERCELL PROJECT
===============================

ALL SYSTEMS EXCEPT WIN32
------------------------
1. Go to the Tinkercell source directory (e.g. Tinkercell/trunk) and create a new folder called BUILD.
2. Install CMake (www.cmake.org)
3. Run the CMake GUI, located in the CMake folder
4. Select the <Tinkercell folder>/trunk as the "source code" folder
5. Select the <Tinkercell folder>/trunk/BUILD as the "build the binaries" folder
6. Change the view to "Grouped view" -- this makes editting the options easier
7. Click "configure". 
8. If this is the first time you are configuring, you will be asked to select the compiler. 
   Select the compiler you prefer. UNIX GCC works for systems with GCC installed. 
9. If this is the first time you are configuring, you will need to click configure once again.
10. The "generate" button should be available now. Click "generate"
11. Open Command Prompt and go to the <Tinkercell folder>/trunk/BUILD directory
12. run make

WIN32 using MinGW g++
--------------------------
1. Go to the Tinkercell source directory (e.g. Tinkercell/trunk) and create a new folder called BUILD.
2. Install CMake (www.cmake.org) and MinGW (www.mingw.org)
3. Run the CMake GUI, located in the CMake folder
4. Select the <Tinkercell folder>/trunk as the "source code" folder
5. Select the <Tinkercell folder>/trunk/BUILD as the "build the binaries" folder
6. Change the view to "Grouped view" -- this makes editting the options easier
7. Click "configure". 
8. If this is the first time you are configuring, you will be asked to select the compiler. Select MinGW.
9. If this is the first time you are configuring, you will need to click configure once again.
10. The "generate" button should be available now. Click "generate"
11. Open Command Prompt and go to the <Tinkercell folder>/trunk/BUILD directory
12. Type "mingw32-make" (or just "make" if the alias is setup).


Possible problems:

-- Problem: CMake is not able to find MinGW. 
   Solution: Look for a field in the CMake GUI called CMAKE_CXX_COMPILER. 
             Set the compiler manually by locating the mingw32-make.exe. 
             Click "configure" again.


WIN32 using Visual Studio 
----------------------------------------------------------------------------
NOTE: Visual Studio is giving link errors, 
      but you are welcome to try (and let me know if it works)

1. Go to the Tinkercell source directory (e.g. Tinkercell/trunk) and create a new folder called BUILD.
2. Install CMake (www.cmake.org) and Visual Studio C++ (http://www.microsoft.com/express/product/)
3. Run the CMake GUI, located in the CMake folder
4. Select the <Tinkercell folder>/trunk as the "source code" folder
5. Select the <Tinkercell folder>/trunk/BUILD as the "build the binaries" folder
6. Change the view to "Grouped view" -- this makes editting the options easier
7. Click "configure". 
8. If this is the first time you are configuring, you will be asked to select the compiler. 
   Select the Visual Studio compiler.
9. If this is the first time you are configuring, you will need to click configure once again.
10. The "generate" button should be available now. Click "generate"
11. The BUILD directory should now contain all the Visual Studio project files. 
12. Open the TINKERCELL visual studio file, which will contain all the other projects.









