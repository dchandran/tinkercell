"""
category: Generate kinetics
name: Hill equations
description: automatically generate the equilibrium rate equation for transcription
icon: hillequation.png
menu: yes
specific for: Coding
tool: yes
"""
import urllib2
import simplejson as json
from Bio import SeqIO
import numpy

#http://biofab.jbei.org/services/data/constructs?collectionid=1&format=csv

#s = tc_getStringDialog("Please enter a BioFAB part")
u = urllib2.urlopen('http://biofab.jbei.org/services/data/construct/performance?id=' + s + '&format=json')
performance = json.loads( u.read() )

#convert performance data to numpy arrays
n1 = len(performance['performance']['reads'])
datalist = []
for i in range(0,n1):
    dat = performance['performance']['reads'][i]['measurements']
    n2 = len(dat)
    datamatrix = numpy.zeros([n2,2])
    for j in range(0, n2):
        datamatrix[j,0] = dat[j]['time']
        datamatrix[j, 1] = dat[j]['value']
    datalist.append(datamatrix)

u = urllib2.urlopen('http://biofab.jbei.org/services/data/construct/design?id=' + s + '&format=genbank')
FILE = open('genbank.txt','w')
FILE.write(u.read())
FILE.close()

FILE = open('genbank.txt', 'rU')
for record in SeqIO.parse(FILE, 'genbank') :
    print record.id
FILE.close()

