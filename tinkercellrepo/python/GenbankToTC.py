import sys
from Bio import SeqIO
from tinkercell import *
from tc2py import *

def loadGenbankFeaturesAsParts(gb_file):
    gb_record = SeqIO.read(open(gb_file,"r"), "genbank")
    vector = tc_insert("g1", "vector")
    seq = gb_record.seq.tostring()
    parts = []
    for i in gb_record.features:
        family = i.type
        if family == "misc_feature": family = "part"
        p = tc_insert("part1",family)
        parts.append(p)
    parts2 = toTC(parts)
    tc_alignPartsOnPlasmid(vector, parts2)
    tc_setTextAttribute(vector, "sequence", seq)   
    return vector
