"""
category: Generate kinetics
name: Hill equations
description: automatically generate the equilibrium rate equation for transcription
icon: hillequation.png
menu: yes
specific for: Coding
tool: yes
"""

import simplejson as json
import biopython as bio

s = tc_getStringDialog("Please enter a BioFAB part")
u = urllib2.urlopen('http://biofab.jbei.org/services/data/construct/performance?id=' + id + 'format=json')
s = u.read()
j = json.loads(s)