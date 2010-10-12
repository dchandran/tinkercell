#!/bin/bash

#run in the bin folder
CURPATH=@TINKERCELL_BINARY_BIN_DIR@
cd @TINKERCELL_BINARY_BIN_DIR@

#library files
LIBFILES='*.dylib'
PLUGINFILES='plugins/*.dylib'
CPLUGINFILES='plugins/c/*.dylib'

#Qt framework files
QTCORE = @QT_QTCORE@
QTGUI = @QT_QTGUI@
QTXML = @QT_QTXML@
QTOPENGL = @QT_QTOPENGL@

#copy QT framework
@QT_LIBRARY_DIR@/../bin/macdeployqt TinkerCell.app
@QT_LIBRARY_DIR@/../bin/macdeployqt NodeGraphics.app

#copy supporting files
cp -R plugins TinkerCell.app/Contents/MacOS/
mkdir TinkerCell.app/Contents/MacOS/c
mkdir TinkerCell.app/Contents/MacOS/lib
cp ../../API/*.h TinkerCell.app/Contents/MacOS/c
cp ../../c/*.h TinkerCell.app/Contents/MacOS/c/
cp ../../Main/tinkercell.qss TinkerCell.app/Contents/MacOS/
cp -R ../../icons TinkerCell.app/Contents/MacOS/
cp plugins/c/.a TinkerCell.app/Contents/MacOS/lib/
cp lib* TinkerCell.app/Contents/MacOS/
cp -R ../../Graphics TinkerCell.app/Contents/MacOS/
mkdir TinkerCell.app/Contents/MacOS/NodesTree/
cp ../../NodesTree/*.xml TinkerCell.app/Contents/MacOS/NodesTree/
cp -R python TinkerCell.app/Contents/MacOS/
cp ../../python/*.py TinkerCell.app/Contents/MacOS/python
cp ../../*.txt TinkerCell.app/Contents/MacOS/
cp -R octave TinkerCell.app/Contents/MacOS/
cp ../../octave/*.m TinkerCell.app/Contents/MacOS/octave/
cp -R ../../Modules/ TinkerCell.app/Contents/MacOS/

#name change for all libraries used in TinkerCell.app and NodeGraphics.app

for f in $LIBFILES
do
  echo "Processing $f"
  
  cp $f TinkerCell.app/Contents/Frameworks/
  
  cp $f NodeGraphics.app/Contents/Frameworks/
  
  for f2 in $LIBFILES
  do
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/../Frameworks/$f2 \
          TinkerCell.app/Contents/Frameworks/$f
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/../Frameworks/$f2 \
          NodeGraphics.app/Contents/Frameworks/$f
  done
  
  install_name_tool \
        -id @executable_path/../Frameworks/$f \
        TinkerCell.app/Contents/Frameworks/$f
  
  install_name_tool \
        -change $CURPATH/$f \
         @executable_path/../Frameworks/$f \
         TinkerCell.app/Contents/MacOS/Tinkercell

  install_name_tool \
         -change $CURPATH/$f \
         @executable_path/../Frameworks/$f \
         NodeGraphics.app/Contents/MacOS/NodeGraphics

  install_name_tool \
          -change @QT_QTCORE_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/$QTCORE \
          TinkerCell.app/Contents/Frameworks/$f

  install_name_tool \
          -change @QT_QTGUI_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/$QTGUI \
          TinkerCell.app/Contents/Frameworks/$f

  install_name_tool \
          -change @QT_QTXML_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/$QTXML \
          TinkerCell.app/Contents/Frameworks/$f

  install_name_tool \
          -change @QT_QTOPENGL_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/$QTOPENGL \
          TinkerCell.app/Contents/Frameworks/$f
          
  install_name_tool \
          -change @QT_QTCORE_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/$QTCORE \
          NodeGraphics.app/Contents/Frameworks/$f

  install_name_tool \
          -change @QT_QTGUI_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/$QTGUI \
          NodeGraphics.app/Contents/Frameworks/$f

  install_name_tool \
          -change @QT_QTXML_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/$QTXML \
          NodeGraphics.app/Contents/Frameworks/$f

  install_name_tool \
          -change @QT_QTOPENGL_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/$QTOPENGL \
          NodeGraphics.app/Contents/Frameworks/$f

done

#name change for all plugins that can depend on other plugins and libTinkerCellCore

for f1 in $PLUGINFILES
do
  echo "Processing $f1"
  install_name_tool \
          -id \
          @executable_path/$f1 \
          TinkerCell.app/Contents/MacOS/$f1

  install_name_tool \
          -change @QT_QTCORE_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/$QTCORE \
         TinkerCell.app/Contents/MacOS/$f1

  install_name_tool \
          -change @QT_QTGUI_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/$QTGUI \
          TinkerCell.app/Contents/MacOS/$f1

  install_name_tool \
          -change @QT_QTXML_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/$QTXML \
          TinkerCell.app/Contents/MacOS/$f1

  install_name_tool \
          -change @QT_QTOPENGL_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/$QTOPENGL \
          TinkerCell.app/Contents/MacOS/$f1

  for f2 in $LIBFILES
  do
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/../Frameworks/$f2 \
          TinkerCell.app/Contents/MacOS/$f1
  done
  for f2 in $PLUGINFILES
  do
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/$f2 \
          TinkerCell.app/Contents/MacOS/$f1
  done
  for f2 in $CPLUGINFILES
  do
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/$f2 \
          TinkerCell.app/Contents/MacOS/$f1
  done
done

for f1 in $CPLUGINFILES
do
  echo "Processing $f1"
  install_name_tool \
          -id \
          @executable_path/$f1 \
          TinkerCell.app/Contents/MacOS/$f1  
  for f2 in $CPLUGINFILES
  do
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/$f2 \
          TinkerCell.app/Contents/MacOS/$f1
  done
done

mkdir TinkerCell
mv TinkerCell.app TinkerCell
mv NodeGraphics.app TinkerCell
ln -s /Applications TinkerCell

