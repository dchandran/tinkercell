#!/usr/local/bin/perl

open FILE, "TC_py.h";

while ($line = <FILE>)
{
	if ($line =~ /{\"(\S+)\",.+,.+,\s*\"(.*)example:(.*)\"\s*}/)
	{
		$a = $1;
		$b = $2;
		$c = $3;
		
		$c =~ s/\\"/"/g;
		
		print "<b>$a</b><br>\n&nbsp;&nbsp;&nbsp;$b<br>\n&nbsp;&nbsp;&nbsp;example:<br>\n&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;$c<br><br>\n\n";
	}
}

close FILE;
 