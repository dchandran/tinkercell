#!/bin/bash

#run in the bin folder

PATH = 'pwd'
LIBFILES='*.dylib'
PLUGINFILES='Plugins/*.dylib'

for f in $LIBFILES
do
  echo "Processing $f ..."
  cp $f Tinkercell.app/Contents/Frameworks/
  install_name_tool \
        -id @executable_path/../Frameworks/$f \
        Tinkercell.app/Contents/Frameworks/$f
done

for f2 in $LIBFILES
do
    echo "install_name_tool Tinkercell ... $f2"
    install_name_tool \
          -change $PATH/$f2 \
          ../Frameworks/$f2 \
          Tinkercell.app/Contents/MacOS/Tinkercell
done

for f1 in $PLUGINFILES
do
  for f2 in $LIBFILES
  do
    echo "install_name_tool $f1 ... $f2"
    install_name_tool \
          -change $PATH/$f2 \
          ../Frameworks/$f2 \
          Tinkercell.app/Contents/MacOS/$f1
  done
done


