# names given to items in tinkercell by default
# (there will also be a number appended, eg "ter1")
TC_DEFAULT_NAMES = (
"vec",
"cod",
"rbs",
"ter",
"pro",
"as",
"rs"
)

# maps TinkerCell part families to sbpkb categories
TC_2_SBPKB = {
"coding"                 : "cds",
"rbs"                    : "rbs",
"terminator"             : "terminator",
"vector"                 : "plasmid",
"promoter"               : "promoter",
"constitutive promoter"  : "promoter",
"repressible promoter"   : "promoter",
"operator"               : "operator",
"inducible promoter"     : "promoter",
"activator binding site" : "operator",
"repressor binding site" : "operator"
}

# nodes to collect from registry XML files
# and add to PartInfo objects
USEFUL_REGISTRY_NODES = (
"part_name",
"part_short_name",
"part_short_desc",
"part_type",
"part_status",
"part_results",
"part_nickname",
"part_url",
"part_entered",
"part_author",
"part_rating",
"seq_data"
)

# maps information in PartInfo objects
# to the corresponding TinkerCell annotation
REGISTRY_2_TC = {
"part_short_desc" : "description",
"part_nickname"   : "name",
"part_url"        : "uri",
"part_entered"    : "date",
"part_author"     : "author"
}

