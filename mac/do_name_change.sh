#!/bin/bash

#run in the bin folder

LIBFILES="*.dylib"
PLUGINFILES="Plugins/*.dylib"

for f in "$LIBFILES"
do
  echo "Processing $f ..."
  cp $f Tinkercell.app/Contents/Frameworks/
  install_name_tool \
        -id @executable_path/../Frameworks/$f \
        Tinkercell.app/Contents/Frameworks/$f
done

for f2 in "$LIBFILES"
do
    install_name_tool \
          -change $f2 \
          @executable_path/../Frameworks/$f2 \
          Tinkercell.app/Contents/MacOS/Tinkercell
done

for f1 in "$PLUGINFILES"
do
  echo "Processing $f ..."
  for f2 in "$LIBFILES"
  do
    install_name_tool \
          -change $f2 \
          @executable_path/../Frameworks/$f2 \
          Tinkercell.app/Contents/MacOS/Plugins/$f1
  done
done

