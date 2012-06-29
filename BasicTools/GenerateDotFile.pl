$numskip = 0;
$cmdargs = $#ARGV + 1;

if ($cmdargs < 1)
{
	print "example usage: perl GenerateDotFile.pl ConnectionsTree.nt 19 > ConnectionsTree.dot\n";
	exit;
}

open(FH, $ARGV[0]);

if ($cmdargs > 1)
{
	$numskip = $ARGV[1];
}

$edges = "";
$nodes = "";
while ($line = <FH>)
{
	if ($numskip < 0 && $line =~ /<(.*)>\s+<a>\s+<(.*)>/)
	{
		$a = "\"".$1."\"";
		$b = "\"".$2."\"";
		$edges = $edges. "    ".$a."  ->  ".$b.";\n";
		if ($nodes !~ $a)
		{
			$nodes = $nodes."    ".$a." [orientation=90];\n";
		}
		if ($nodes !~ $b)
		{
			$nodes = $nodes."    ".$b." [orientation=90];\n";
		}
	}
	$numskip = $numskip - 1;
}

print "digraph g {\n";
print $nodes;
print $edges;
print "}\n";
