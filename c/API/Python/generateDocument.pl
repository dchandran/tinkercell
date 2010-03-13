#!/usr/local/bin/perl

open FILE, "TC_py.h";

while ($line = <FILE>)
{
	if ($line =~ /{\"(\S+)\",.+,.+,\s*\"(.*)example:(.*)\"\s*}/)
	{
		print "$1\n    $2\n    example:\n      $3\n\n";
	}
}

close FILE;
 