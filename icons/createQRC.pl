#!/usr/bin/perl
#This perl script can be used to create a Qt QRC file for all the files in a given folder
# To generate a QRC file for all the icons in the /icons folder, 
#     first, cd into the Core folder
#     second, call "perl ../icons/createQRC.pl ../icons > Tinkercell.qrc" 
# compile again so that the new qrc file is used

$dir = $ARGV[0];
opendir(DIR,$dir );
@FILES= readdir(DIR);

print "<RCC>\n";
print "    <qresource prefix=\"/images\" >\n";

foreach my $file(@FILES)
{
	if ($file =~ /[A-Za-z0-9]\.[A-Za-z0-9]/ && $file !~ /~/ && $file !~ /\.pl/)
	{
		print "        <file alias=\"$file\">$dir$file</file>\n";
	}
}

print "    </qresource>\n";
print "</RCC>\n";

