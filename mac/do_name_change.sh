#!/bin/bash

#run in the bin folder

CURPATH=$1
LIBFILES='*.dylib'
PLUGINFILES='Plugins/*.dylib'
CPLUGINFILES='Plugins/c/*.dylib'

#copy other supporting files

cp -R Plugins Tinkercell.app/Contents/MacOS/
cp -R ../../c Tinkercell.app/Contents/MacOS/
cp -R ../../Main/tinkercell.qss Tinkercell.app/Contents/MacOS/
cp -R ../../c/icons/*.png Tinkercell.app/Contents/MacOS/Plugins/c
cp -R ../../c/icons/*.PNG Tinkercell.app/Contents/MacOS/Plugins/c
cp Plugins/c/lib*.a Tinkercell.app/Contents/MacOS/c/
cp lib*.a Tinkercell.app/Contents/MacOS/c/
cp -R ../../ArrowItems Tinkercell.app/Contents/MacOS/
cp -R ../../NodeItems Tinkercell.app/Contents/MacOS/
cp -R ../../OtherItems Tinkercell.app/Contents/MacOS/
mkdir Tinkercell.app/Contents/MacOS/NodesTree/
cp ../../NodesTree/*.xml Tinkercell.app/Contents/MacOS/NodesTree/
cp -R ../../NodesTree/Icons Tinkercell.app/Contents/MacOS/NodesTree/
cp -R ../../py Tinkercell.app/Contents/MacOS/
cp ../../py/python*.txt Tinkercell.app/Contents/MacOS/
cp ../../*.txt Tinkercell.app/Contents/MacOS/


#QT frameworks install for TinkerCell.app

install_name_tool \
        -id @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
        Tinkercell.app/Contents/Frameworks/QtCore.framework/Versions/4/QtCore

install_name_tool \
        -id @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
        Tinkercell.app/Contents/Frameworks/QtGui.framework/Versions/4/QtGui

install_name_tool \
        -id @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml \
        Tinkercell.app/Contents/Frameworks/QtXml.framework/Versions/4/QtXml

install_name_tool \
        -id @executable_path/../Frameworks/QtOpenGL.framework/Versions/4/QtOpenGL \
        Tinkercell.app/Contents/Frameworks/QtOpenGL.framework/Versions/4/QtOpenGL

#QT frameworks install for NodeGraphics.app

install_name_tool \
        -id @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
        NodeGraphics.app/Contents/Frameworks/QtCore.framework/Versions/4/QtCore

install_name_tool \
        -id @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
        NodeGraphics.app/Contents/Frameworks/QtGui.framework/Versions/4/QtGui

install_name_tool \
        -id @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml \
        NodeGraphics.app/Contents/Frameworks/QtXml.framework/Versions/4/QtXml

install_name_tool \
        -id @executable_path/../Frameworks/QtOpenGL.framework/Versions/4/QtOpenGL \
        NodeGraphics.app/Contents/Frameworks/QtOpenGL.framework/Versions/4/QtOpenGL

#QtCore name change for other Qt frameworks

install_name_tool \
          -change QtCore.framework/Versions/4/QtCore \
          @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
          Tinkercell.app/Contents/Frameworks/QtGui.framework/Versions/4/QtGui

install_name_tool \
          -change QtCore.framework/Versions/4/QtCore \
          @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
          Tinkercell.app/Contents/Frameworks/QtXml.framework/Versions/4/QtXml
          
install_name_tool \
          -change QtCore.framework/Versions/4/QtCore \
          @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
          Tinkercell.app/Contents/Frameworks/QtOpenGL.framework/Versions/4/QtOpenGL

#QT framework name change for TinkerCell.app

install_name_tool \
          -change QtCore.framework/Versions/4/QtCore \
          @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
          Tinkercell.app/Contents/MacOS/Tinkercell

install_name_tool \
          -change QtGui.framework/Versions/4/QtGui \
          @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
          Tinkercell.app/Contents/MacOS/Tinkercell

install_name_tool \
          -change QtXml.framework/Versions/4/QtXml \
          @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml \
          Tinkercell.app/Contents/MacOS/Tinkercell

install_name_tool \
          -change QtOpenGL.framework/Versions/4/QtOpenGL \
          @executable_path/../Frameworks/QtOpenGL.framework/Versions/4/QtOpenGL \
          Tinkercell.app/Contents/MacOS/Tinkercell

#QT framework name change for NodeGraphcs.app

install_name_tool \
          -change QtCore.framework/Versions/4/QtCore \
          @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
          NodeGraphics.app/Contents/MacOS/NodeGraphics

install_name_tool \
          -change QtGui.framework/Versions/4/QtGui \
          @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
          NodeGraphics.app/Contents/MacOS/NodeGraphics

install_name_tool \
          -change QtXml.framework/Versions/4/QtXml \
          @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml \
          NodeGraphics.app/Contents/MacOS/NodeGraphics

install_name_tool \
          -change QtOpenGL.framework/Versions/4/QtOpenGL \
          @executable_path/../Frameworks/QtOpenGL.framework/Versions/4/QtOpenGL \
          NodeGraphics.app/Contents/MacOS/NodeGraphics

#for all libraries used in Tinkercell.app and NodeGraphics.app

for f in $LIBFILES
do
  echo "Processing $f"
  
  cp $f Tinkercell.app/Contents/Frameworks/
  
  cp $f NodeGraphics.app/Contents/Frameworks/
  
  for f2 in $LIBFILES
  do
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/../Frameworks/$f2 \
          Tinkercell.app/Contents/Frameworks/$f
  done
  
  install_name_tool \
        -id @executable_path/../Frameworks/$f \
        Tinkercell.app/Contents/Frameworks/$f
  
  install_name_tool \
        -change $CURPATH/$f \
         @executable_path/../Frameworks/$f \
         Tinkercell.app/Contents/MacOS/Tinkercell

  install_name_tool \
         -change $CURPATH/$f \
         @executable_path/../Frameworks/$f \
         NodeGraphics.app/Contents/MacOS/NodeGraphics

  install_name_tool \
          -change QtCore.framework/Versions/4/QtCore \
          @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
          Tinkercell.app/Contents/Frameworks/$f

  install_name_tool \
          -change QtGui.framework/Versions/4/QtGui \
          @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
          Tinkercell.app/Contents/Frameworks/$f

  install_name_tool \
          -change QtXml.framework/Versions/4/QtXml \
          @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml \
          Tinkercell.app/Contents/Frameworks/$f

  install_name_tool \
          -change QtOpenGL.framework/Versions/4/QtOpenGL \
          @executable_path/../Frameworks/QtOpenGL.framework/Versions/4/QtOpenGL \
          Tinkercell.app/Contents/Frameworks/$f
          
  install_name_tool \
          -change QtCore.framework/Versions/4/QtCore \
          @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
          NodeGraphics.app/Contents/Frameworks/$f

  install_name_tool \
          -change QtGui.framework/Versions/4/QtGui \
          @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
          NodeGraphics.app/Contents/Frameworks/$f

  install_name_tool \
          -change QtXml.framework/Versions/4/QtXml \
          @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml \
          NodeGraphics.app/Contents/Frameworks/$f

  install_name_tool \
          -change QtOpenGL.framework/Versions/4/QtOpenGL \
          @executable_path/../Frameworks/QtOpenGL.framework/Versions/4/QtOpenGL \
          NodeGraphics.app/Contents/Frameworks/$f

done

#name change for all plugins that can depend on other plugins and libTinkerCellCore

for f1 in $PLUGINFILES
do
  echo "Processing $f1"
  install_name_tool \
          -id \
          @executable_path/$f1 \
          Tinkercell.app/Contents/MacOS/$f1

  install_name_tool \
          -change QtCore.framework/Versions/4/QtCore \
          @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
         Tinkercell.app/Contents/MacOS/$f1

  install_name_tool \
          -change QtGui.framework/Versions/4/QtGui \
          @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
          Tinkercell.app/Contents/MacOS/$f1

  install_name_tool \
          -change QtXml.framework/Versions/4/QtXml \
          @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml \
          Tinkercell.app/Contents/MacOS/$f1

  install_name_tool \
          -change QtOpenGL.framework/Versions/4/QtOpenGL \
          @executable_path/../Frameworks/QtOpenGL.framework/Versions/4/QtOpenGL \
          Tinkercell.app/Contents/MacOS/$f1

  for f2 in $LIBFILES
  do
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/../Frameworks/$f2 \
          Tinkercell.app/Contents/MacOS/$f1
  done
  for f2 in $PLUGINFILES
  do
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/$f2 \
          Tinkercell.app/Contents/MacOS/$f1
  done
  for f2 in $CPLUGINFILES
  do
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/$f2 \
          Tinkercell.app/Contents/MacOS/$f1
  done
done

for f1 in $CPLUGINFILES
do
  echo "Processing $f1"
  install_name_tool \
          -id \
          @executable_path/$f1 \
          Tinkercell.app/Contents/MacOS/$f1  
  for f2 in $CPLUGINFILES
  do
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/$f2 \
          Tinkercell.app/Contents/MacOS/$f1
  done
done


