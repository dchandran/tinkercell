.SUFFIXES: .cpp .exe

all: ..\..\BioCoder\BioCoder.obj ..\..\BioCoder\Graph.obj *.cpp
    @$(MAKE) $(**:.cpp=.exe)

.cpp.exe:
	cl -I..\..\BioCoder $< /link /LIBPATH:../../BioCoder BioCoder.obj Graph.obj

..\..\BioCoder\BioCoder.obj: ..\..\BioCoder\BioCoder.cpp
	cl -c -I..\..\BioCoder ..\..\BioCoder\BioCoder.cpp /Fo..\..\BioCoder\BioCoder.obj

..\..\BioCoder\Graph.obj: ..\..\BioCoder\Graph.cpp
	cl -c -I..\..\BioCoder ..\..\BioCoder\Graph.cpp /Fo..\..\BioCoder\Graph.obj

clean:
	del *.obj *.exe ..\..\BioCoder\BioCoder.obj ..\..\BioCoder\Graph.obj
