#!/bin/bash

#run in the bin folder

CURPATH=$1
LIBFILES='*.dylib'
PLUGINFILES='Plugins/*.dylib'
CPLUGINFILES='Plugins/c/*.dylib'

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

for f in $LIBFILES
do
  echo "Processing $f ..."
  cp $f Tinkercell.app/Contents/Frameworks/
  cp $f NodeGraphics.app/Contents/Frameworks/
  install_name_tool \
        -id @executable_path/../Frameworks/$f \
        Tinkercell.app/Contents/Frameworks/$f
done

for f2 in $LIBFILES
do
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/../Frameworks/$f2 \
          Tinkercell.app/Contents/MacOS/Tinkercell
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/../Frameworks/$f2 \
          NodeGraphics.app/Contents/MacOS/NodeGraphics
done

install_name_tool \
          -change $CURPATH/libmuparser.dylib \
          @executable_path/../Frameworks/libmuparser.dylib \
          Tinkercell.app/Contents/Frameworks/libTinkerCellCore.dylib

install_name_tool \
          -change $CURPATH/libmuparser.dylib \
          @executable_path/../Frameworks/libmuparser.dylib \
          NodeGraphics.app/Contents/Frameworks/libTinkerCellCore.dylib

for f1 in $PLUGINFILES
do
  install_name_tool \
          -id \
          @executable_path/$f1 \
          Tinkercell.app/Contents/MacOS/$f1
  for f2 in $LIBFILES
  do
    echo "install_name_tool $CURPATH/$f2 ... Tinkercell.app/Contents/MacOS/$f1"
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/../Frameworks/$f2 \
          Tinkercell.app/Contents/MacOS/$f1
  done
  for f2 in $PLUGINFILES
  do
    echo "install_name_tool $CURPATH/$f2 ... Tinkercell.app/Contents/MacOS/$f1"
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/$f2 \
          Tinkercell.app/Contents/MacOS/$f1
  done
  for f2 in $CPLUGINFILES
  do
    echo "install_name_tool $CURPATH/$f2 ... Tinkercell.app/Contents/MacOS/$f1"
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/$f2 \
          Tinkercell.app/Contents/MacOS/$f1
  done
done

for f1 in $CPLUGINFILES
do
  install_name_tool \
          -id \
          @executable_path/$f1 \
          Tinkercell.app/Contents/MacOS/$f1  
  for f2 in $CPLUGINFILES
  do
    echo "install_name_tool $CURPATH/$f2 ... Tinkercell.app/Contents/MacOS/$f1"
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/$f2 \
          Tinkercell.app/Contents/MacOS/$f1
  done
done
