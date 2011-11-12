"""
category: Database
name: BioFAB parts
description: load constructs and data from BioFAB
icon: database.png
menu: yes
specific for: Vector
tool: yes
"""
import sys
import urllib2
import simplejson as json
import numpy
import csv
from GenbankToTC import *

#get list of bioFAB parts
u = urllib2.urlopen('http://biofab.jbei.org/services/data/constructs?collectionid=1&format=csv')
FILE = open('biofab.csv','w')
FILE.write(u.read())
FILE.close()

tc_showProgress("Fetching info from BioFab", 0)
#create a list from the csv file
displaylist = []
idlist = []
bfreader = csv.reader(open('biofab.csv', 'rb'), delimiter=',', quotechar='|')
bfreader.next()
for row in bfreader:
    if len(row) > 0:
        displaylist.append( '    '.join(row))
        idlist.append( row[1] )

#ask user to select a part from the list
k = tc_getStringFromList("Please select a BioFAB part", toTC(displaylist), "")
s = idlist[k]  #to do: check for -1 case

#load performance data for user selected part
u = urllib2.urlopen('http://biofab.jbei.org/services/data/construct/performance?id=' + s + '&format=json')
performance = json.loads( u.read() )

tc_showProgress("Fetching info from BioFab", 20)
#convert performance data to numpy arrays
n1 = len(performance['performance']['reads'])
datalist = []
tc_multiplot(1,n1)
for i in range(0,n1):
    dat = performance['performance']['reads'][i]['measurements']
    n2 = len(dat)
    datamatrix = numpy.zeros([n2,2])
    for j in range(0, n2):
        datamatrix[j,0] = dat[j]['time']
        datamatrix[j, 1] = dat[j]['value']
    m = datamatrix.transpose().tolist()
    datalist.append(m)
    m2 = toTC(m, rows=[], cols=['time','value'])
    tc_plot(m2, 'reads ' + str(i))
    tc_setLogScale(1)

#load genbank data for user selected part
u = urllib2.urlopen('http://biofab.jbei.org/services/data/construct/design?id=' + s + '&format=genbank')

tc_showProgress("Fetching info from BioFab", 50)
FILE = open('genbank.txt','w')
s = u.readline()
s = s.replace('          ','     ') #bug??
while len(s)>0:
    FILE.write(s)
    s = u.readline()
FILE.close()

tc_showProgress("Fetching info from BioFab", 100)
loadGenbankFeaturesAsParts('genbank.txt')


