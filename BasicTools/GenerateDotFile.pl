print "graph g {\n";
while ($line = <>)
{
	if ($line =~ /<(.*)>\s+<a>\s+<(.*)>/)
	{
		print "    ", $1,"  ->  ",$2,"\n";
	}
}
print "}\n";