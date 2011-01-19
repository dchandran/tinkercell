#!/bin/sh

cd @TINKERCELL_BINARY_DIR@
make package
cd @TINKERCELL_BINARY_DIR@/_CPack_Packages/Linux/TGZ
mv TinkerCell.tar.gz @TINKERCELL_EXE@@TINKERCELL_BIT@.tar.gz
echo "renamed TinkerCell.tar.gz to @TINKERCELL_EXE@@TINKERCELL_BIT@.tar.gz"

if [ @TINKERCELL_UPLOAD_SOURCE@ == ON ]
  then
	echo "\033[31m extracting source code... \033[0m"
    svn export @TINKERCELL_SOURCE_DIR@ TinkerCellSource
    echo "\033[33m compressing source code...\033[0m"
    tar czf @TINKERCELL_BINARY_DIR@/_CPack_Packages/Linux/TGZ/TinkerCellSource.tar.gz TinkerCellSource
    rm -Rf TinkerCellSource
	echo "\033[33m created @TINKERCELL_BINARY_DIR@/_CPack_Packages/Linux/TGZ/TinkerCellSource.tar.gz \033[0m"
fi 

echo "\033[31m logging in to Sourceforge as @TINKERCELL_SOURCEFORGE_USERNAME@ \033[0m"

cd @TINKERCELL_BINARY_DIR@/_CPack_Packages/Linux/TGZ

/usr/bin/expect <<EOD
set timeout -1

spawn sftp @TINKERCELL_SOURCEFORGE_USERNAME@,tinkercell@frs.sourceforge.net
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

echo "\033[32m all done \033[0m"

