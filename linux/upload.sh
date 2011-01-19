#!/bin/sh

cd @TINKERCELL_BINARY_DIR@
make package
cd @TINKERCELL_BINARY_DIR@/_CPack_Packages/Linux/TGZ
mv TinkerCell.tar.gz @TINKERCELL_EXE@@TINKERCELL_BIT@.tar.gz

if [ @TINKERCELL_UPLOAD_SOURCE@ == ON ]
  then
    echo "creating source tarball..."
    cd @TINKERCELL_SOURCE_DIR@
    cd ..
   svn export TinkerCell TinkerCellSource
    tar czvf @TINKERCELL_BINARY_DIR@/_CPack_Packages/Linux/TGZ/TinkerCellSource.tar.gz TinkerCellSource
    rm -Rf TinkerCellSource
fi 

cd @TINKERCELL_BINARY_DIR@/_CPack_Packages/Linux/TGZ

/usr/bin/expect <<EOD
set timeout -1

spawn sftp dchandran1,tinkercell@frs.sourceforge.net
expect *assword:

send "@TINKERCELL_SOURCEFORGE_PASSWORD@\r"
expect sftp>

send "cd /home/frs/project/t/ti/tinkercell\r"
expect sftp>

send "put TinkerCell*.tar.gz\r"
expect sftp>

send "exit\r"
expect eof

EOD

