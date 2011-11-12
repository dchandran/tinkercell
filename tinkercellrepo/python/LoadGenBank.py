"""
category: Sequence
name: Load GenBank file
description: load the parts specified in a GenBank file onto a vector backbone
icon: printSequences.png
menu: yes
specific for: Vector
"""
import sys
import urllib2
import simplejson as json
import numpy
import csv
from GenbankToTC import *

filename = tc_getFilename();
if len(filename) > 0:
    loadGenbankFeaturesAsParts(filename)


