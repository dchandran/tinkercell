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
@QT_LIBRARY_DIR@/../bin/macdeployqt @TINKERCELL_EXE@.app
@QT_LIBRARY_DIR@/../bin/macdeployqt NodeGraphics.app

#copy supporting files
cp -R plugins @TINKERCELL_EXE@.app/Contents/MacOS/
mkdir @TINKERCELL_EXE@.app/Contents/MacOS/c
mkdir @TINKERCELL_EXE@.app/Contents/MacOS/lib
cp @TINKERCELL_SOURCE_DIR@/API/*.h @TINKERCELL_EXE@.app/Contents/MacOS/c
cp @TINKERCELL_SOURCE_DIR@/ExternalCode/copasi/copasi_api.h @TINKERCELL_EXE@.app/Contents/MacOS/c
cp @TINKERCELL_SOURCE_DIR@/Main/tinkercell.qss @TINKERCELL_EXE@.app/Contents/MacOS/
cp -R @TINKERCELL_SOURCE_DIR@/icons @TINKERCELL_EXE@.app/Contents/MacOS/
cp @TINKERCELL_BINARY_BIN_DIR@/plugins/c/*.a @TINKERCELL_EXE@.app/Contents/MacOS/lib/
cp @TINKERCELL_BINARY_BIN_DIR@/plugins/*.a @TINKERCELL_EXE@.app/Contents/MacOS/lib/
cp @TINKERCELL_BINARY_BIN_DIR@/*.a @TINKERCELL_EXE@.app/Contents/MacOS/lib/
cp @TINKERCELL_BINARY_BIN_DIR@/*.dylib @TINKERCELL_EXE@.app/Contents/Frameworks/
cp @TINKERCELL_BINARY_BIN_DIR@/*.dylib NodeGraphics.app/Contents/Frameworks/
cp -R @TINKERCELL_SOURCE_DIR@/Graphics @TINKERCELL_EXE@.app/Contents/MacOS/
mkdir @TINKERCELL_EXE@.app/Contents/MacOS/NodesTree/
cp @TINKERCELL_SOURCE_DIR@/NodesTree/*.xml @TINKERCELL_EXE@.app/Contents/MacOS/NodesTree/
cp -R @TINKERCELL_BINARY_BIN_DIR@/python @TINKERCELL_EXE@.app/Contents/MacOS/
cp @TINKERCELL_SOURCE_DIR@/python/*.py @TINKERCELL_EXE@.app/Contents/MacOS/python
cp @TINKERCELL_SOURCE_DIR@/*.txt @TINKERCELL_EXE@.app/Contents/MacOS/
cp -R octave @TINKERCELL_EXE@.app/Contents/MacOS/
cp @TINKERCELL_SOURCE_DIR@/octave/*.m @TINKERCELL_EXE@.app/Contents/MacOS/octave/
cp -R @TINKERCELL_HOME_DIR@/Modules @TINKERCELL_EXE@.app/Contents/MacOS/
cp -R @TINKERCELL_HOME_DIR@/octave/*.* @TINKERCELL_EXE@.app/Contents/MacOS/octave/
cp -R @TINKERCELL_HOME_DIR@/python/ @TINKERCELL_EXE@.app/Contents/MacOS/python/
cp -R @TINKERCELL_HOME_DIR@/plugins/ @TINKERCELL_EXE@.app/Contents/MacOS/plugins/

#name change for all libraries used in @TINKERCELL_EXE@.app and NodeGraphics.app

for f in $LIBFILES
do
  echo "Processing $f"
  
  cp $f @TINKERCELL_EXE@.app/Contents/Frameworks/
  
  cp $f NodeGraphics.app/Contents/Frameworks/
  
  for f2 in $LIBFILES
  do
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/../Frameworks/$f2 \
          @TINKERCELL_EXE@.app/Contents/Frameworks/$f
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/../Frameworks/$f2 \
          NodeGraphics.app/Contents/Frameworks/$f
  done
  
  install_name_tool \
        -id @executable_path/../Frameworks/$f \
        @TINKERCELL_EXE@.app/Contents/Frameworks/$f
  
  install_name_tool \
        -change $CURPATH/$f \
         @executable_path/../Frameworks/$f \
         @TINKERCELL_EXE@.app/Contents/MacOS/Tinkercell

  install_name_tool \
         -change $CURPATH/$f \
         @executable_path/../Frameworks/$f \
         NodeGraphics.app/Contents/MacOS/NodeGraphics

  install_name_tool \
          -change @QT_QTCORE_LIBRARY_RELEASE@/$QTCORE \
          @executable_path/../Frameworks/QtCore.framework/$QTCORE \
          @TINKERCELL_EXE@.app/Contents/Frameworks/$f

  install_name_tool \
          -change @QT_QTGUI_LIBRARY_RELEASE@/$QTGUI \
          @executable_path/../Frameworks/QtGui.framework/$QTGUI \
          @TINKERCELL_EXE@.app/Contents/Frameworks/$f

  install_name_tool \
          -change @QT_QTXML_LIBRARY_RELEASE@/$QTXML \
          @executable_path/../Frameworks/QtXml.framework/$QTXML \
          @TINKERCELL_EXE@.app/Contents/Frameworks/$f

  install_name_tool \
          -change @QT_QTOPENGL_LIBRARY_RELEASE@/$QTOPENGL \
          @executable_path/../Frameworks/QtOpenGL.framework/$QTOPENGL \
          @TINKERCELL_EXE@.app/Contents/Frameworks/$f
          
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
          @TINKERCELL_EXE@.app/Contents/MacOS/$f1

  install_name_tool \
          -change @QT_QTCORE_LIBRARY_RELEASE@/$QTCORE \
          @executable_path/../Frameworks/QtCore.framework/$QTCORE \
         @TINKERCELL_EXE@.app/Contents/MacOS/$f1

  install_name_tool \
          -change @QT_QTGUI_LIBRARY_RELEASE@/$QTGUI \
          @executable_path/../Frameworks/QtGui.framework/$QTGUI \
          @TINKERCELL_EXE@.app/Contents/MacOS/$f1

  install_name_tool \
          -change @QT_QTXML_LIBRARY_RELEASE@/$QTXML \
          @executable_path/../Frameworks/QtXml.framework/$QTXML \
          @TINKERCELL_EXE@.app/Contents/MacOS/$f1

  install_name_tool \
          -change @QT_QTOPENGL_LIBRARY_RELEASE@/$QTOPENGL \
          @executable_path/../Frameworks/QtOpenGL.framework/$QTOPENGL \
          @TINKERCELL_EXE@.app/Contents/MacOS/$f1

  for f2 in $LIBFILES
  do
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/../Frameworks/$f2 \
          @TINKERCELL_EXE@.app/Contents/MacOS/$f1
  done
  for f2 in $PLUGINFILES
  do
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/$f2 \
          @TINKERCELL_EXE@.app/Contents/MacOS/$f1
  done
  for f2 in $CPLUGINFILES
  do
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/$f2 \
          @TINKERCELL_EXE@.app/Contents/MacOS/$f1
  done
done

for f1 in $CPLUGINFILES
do
  echo "Processing $f1"
  install_name_tool \
          -id \
          @executable_path/$f1 \
          @TINKERCELL_EXE@.app/Contents/MacOS/$f1  
  for f2 in $CPLUGINFILES
  do
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/$f2 \
          @TINKERCELL_EXE@.app/Contents/MacOS/$f1
  done
done

#create disk image
hdiutil create -megabytes 220 -fs HFS+ -volname @TINKERCELL_EXE@ ~/Desktop/@TINKERCELL_EXE@
open ~/Desktop/@TINKERCELL_EXE@.dmg
sleep 10
cd @TINKERCELL_BINARY_BIN_DIR@
cp -R @TINKERCELL_EXE@.app /Volumes/@TINKERCELL_EXE@
cp -R NodeGraphics.app /Volumes/@TINKERCELL_EXE@
ln -s /Applications /Volumes/@TINKERCELL_EXE@

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

