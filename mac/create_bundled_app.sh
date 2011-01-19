#!/bin/sh

#make
cd @TINKERCELL_BINARY_DIR@
make

#run in the bin folder
CURPATH=@TINKERCELL_BINARY_BIN_DIR@
cd @TINKERCELL_BINARY_BIN_DIR@

#library files
LIBFILES='*.dylib'
PLUGINFILES='plugins/*.dylib'
CPLUGINFILES='plugins/c/*.dylib python/*.dylib octave/*.oct'

#Qt framework files
QTCORE=@QT_QTCORE@
QTGUI=@QT_QTGUI@
QTXML=@QT_QTXML@
QTOPENGL=@QT_QTOPENGL@

#copy QT framework
@QT_LIBRARY_DIR@/../bin/macdeployqt TinkerCell.app
@QT_LIBRARY_DIR@/../bin/macdeployqt NodeGraphics.app

#copy supporting files
cp -R plugins TinkerCell.app/Contents/MacOS/
mkdir TinkerCell.app/Contents/MacOS/c
mkdir TinkerCell.app/Contents/MacOS/lib
cp ../../API/*.h TinkerCell.app/Contents/MacOS/c
cp ../../c/*.h TinkerCell.app/Contents/MacOS/c
cp ../../c/*.c TinkerCell.app/Contents/MacOS/c
cp ../../Main/tinkercell.qss TinkerCell.app/Contents/MacOS/
cp -R ../../icons TinkerCell.app/Contents/MacOS/
cp plugins/c/*.a TinkerCell.app/Contents/MacOS/lib/
cp plugins/*.a TinkerCell.app/Contents/MacOS/lib/
cp *.a TinkerCell.app/Contents/MacOS/lib/
cp *.dylib TinkerCell.app/Contents/Frameworks/
cp *.dylib NodeGraphics.app/Contents/Frameworks/
cp -R ../../Graphics TinkerCell.app/Contents/MacOS/
mkdir TinkerCell.app/Contents/MacOS/NodesTree/
cp ../../NodesTree/*.xml TinkerCell.app/Contents/MacOS/NodesTree/
cp -R python TinkerCell.app/Contents/MacOS/
cp ../../python/*.py TinkerCell.app/Contents/MacOS/python
cp ../../*.txt TinkerCell.app/Contents/MacOS/
cp -R octave TinkerCell.app/Contents/MacOS/
cp ../../octave/*.m TinkerCell.app/Contents/MacOS/octave/
cp -R ../../Modules TinkerCell.app/Contents/MacOS/

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
          -change @QT_QTCORE_LIBRARY_RELEASE@/$QTCORE \
          @executable_path/../Frameworks/QtCore.framework/$QTCORE \
          TinkerCell.app/Contents/Frameworks/$f

  install_name_tool \
          -change @QT_QTGUI_LIBRARY_RELEASE@/$QTGUI \
          @executable_path/../Frameworks/QtGui.framework/$QTGUI \
          TinkerCell.app/Contents/Frameworks/$f

  install_name_tool \
          -change @QT_QTXML_LIBRARY_RELEASE@/$QTXML \
          @executable_path/../Frameworks/QtXml.framework/$QTXML \
          TinkerCell.app/Contents/Frameworks/$f

  install_name_tool \
          -change @QT_QTOPENGL_LIBRARY_RELEASE@/$QTOPENGL \
          @executable_path/../Frameworks/QtOpenGL.framework/$QTOPENGL \
          TinkerCell.app/Contents/Frameworks/$f
          
  install_name_tool \
          -change @QT_QTCORE_LIBRARY_RELEASE@/$QTCORE \
          @executable_path/../Frameworks/QtCore.framework/$QTCORE \
          NodeGraphics.app/Contents/Frameworks/$f

  install_name_tool \
          -change @QT_QTGUI_LIBRARY_RELEASE@/$QTGUI \
          @executable_path/../Frameworks/QtGui.framework/$QTGUI \
          NodeGraphics.app/Contents/Frameworks/$f

  install_name_tool \
          -change @QT_QTXML_LIBRARY_RELEASE@$QTXML \
          @executable_path/../Frameworks/QtXml.framework/$QTXML \
          NodeGraphics.app/Contents/Frameworks/$f

  install_name_tool \
          -change @QT_QTOPENGL_LIBRARY_RELEASE@/$QTOPENGL \
          @executable_path/../Frameworks/QtOpenGL.framework/$QTOPENGL \
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
          -change @QT_QTCORE_LIBRARY_RELEASE@/$QTCORE \
          @executable_path/../Frameworks/QtCore.framework/$QTCORE \
         TinkerCell.app/Contents/MacOS/$f1

  install_name_tool \
          -change @QT_QTGUI_LIBRARY_RELEASE@/$QTGUI \
          @executable_path/../Frameworks/QtGui.framework/$QTGUI \
          TinkerCell.app/Contents/MacOS/$f1

  install_name_tool \
          -change @QT_QTXML_LIBRARY_RELEASE@/$QTXML \
          @executable_path/../Frameworks/QtXml.framework/$QTXML \
          TinkerCell.app/Contents/MacOS/$f1

  install_name_tool \
          -change @QT_QTOPENGL_LIBRARY_RELEASE@/$QTOPENGL \
          @executable_path/../Frameworks/QtOpenGL.framework/$QTOPENGL \
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

#create disk image
hdiutil create -megabytes 220 -fs HFS+ -volname @TINKERCELL_EXE@ ~/Desktop/@TINKERCELL_EXE@
open ~/Desktop/@TINKERCELL_EXE@.dmg
sleep 10
cd @TINKERCELL_BINARY_BIN_DIR@
cp -R @TINKERCELL_EXE@.app /Volumes/@TINKERCELL_EXE@
cp -R NodeGraphics.app /Volumes/@TINKERCELL_EXE@
ln -s /Applications /Volumes/@TINKERCELL_EXE@/Applications

#upload to sourceforge

cd ~/Desktop

/usr/bin/expect <<EOD
set timeout -1

spawn sftp @TINKERCELL_SOURCEFORGE_USERNAME@,tinkercell@frs.sourceforge.net
expect *assword:

send "@TINKERCELL_SOURCEFORGE_PASSWORD@\r"
expect sftp>

send "cd /home/frs/project/t/ti/tinkercell\r"
expect sftp>

send "put TinkerCell*.dmg\r"
expect sftp>

send "exit\r"
expect eof

EOD

echo "cleaning up.."
sudo umount -f /Volumes/@TINKERCELL_EXE@
rm -f ~/Desktop/@TINKERCELL_EXE@.dmg

cd @TINKERCELL_BINARY_DIR@

