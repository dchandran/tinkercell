#!/bin/sh

make package
cd @TINKERCELL_BINARY_DIR@/_CPack_Packages/Linux/TGZ
mv TinkerCell.tar.gz TinkerCell@TINKERCELL_BIT@.tar.gz

/usr/bin/expect <<EOD
set timeout -1

spawn sftp dchandran1,tinkercell@frs.sourceforge.net
expect *assword:

send "May112008\r"
expect sftp>

send "cp /home/frs/project/t/ti/tinkercell\r"
expect sftp>

send "put TinkerCell*.tgz\r"
expect sftp>

send "exit\r"
expect eof

EOD

