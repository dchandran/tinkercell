system("ls *.h >> temp.txt");

open(TEMP,"temp.txt");

$comments = "";
$function = "";
%groups = ();
while ($file = <TEMP>)
{
	chomp $file;
	open(FH,$file);
	while($line = <FH>)
	{
		if ($line =~ m/\/\*/)
		{
			$/ = "\*\/";
			$comments = <FH>;
			$comments = $line.$comments;
			$/ = "\n";
			if ($comments =~ m/\\ingroup\s*([^\*\\\/\n]+)/)
			{
				$s0 = $1;
				$s1 = $s0;
				$s1 =~ s/\s/_/g;
				$groups{$s1} = $s0;
				$comments =~ s/$s0/$s1/;
			}
		}
		if ($line =~ m/(\S+\s+)\(\*(.*)\)\s*(\(.*\)\s*;)/)
		{
			$function = $1.$2.$3;
			print $comments,"\n",$function,"\n";
		}
	}
	close(FH);
}
close(TEMP);
system("del temp.txt");

print "/**\n\
 * \@mainpage TinkerCell/C Interface\n\
 * \@brief \n\
This is a set of functions that allows C programs to interact directly with TinkerCell's visual interface. \n\
A few types to be familiar with are Array, OBJ, and Matrix. OBJ is (void*), and Array is (void**). \n\
Matrix is a simple struct used to store matrix information (see Matrix data structure). \n\
 * \@section Points Important points\n\
All Arrays or string arrays (char**)  MUST be null terminated.\n\
All Arrays and string arrays returned by TinkerCell to C will be null terminated.\n\
*/\n\n";

foreach $i(keys %groups)
{
	if ($i !~ /^init$/)
	{
		$groups{$i} =~ s/_/ /g;
		print "/*! \\defgroup ",$i," ",$groups{$i},"*/\n";
	}
}