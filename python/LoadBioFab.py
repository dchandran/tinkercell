"""
category: Database
name: BioFAB parts
description: load constructs and data from BioFAB
icon: database.png
menu: yes
specific for: Vector
tool: yes
"""
import urllib2
import simplejson as json
from Bio import SeqIO
import numpy
import csv

#get list of bioFAB parts
u = urllib2.urlopen('http://biofab.jbei.org/services/data/constructs?collectionid=1&format=csv')
FILE = open('biofab.csv','w')
FILE.write(u.read())
FILE.close()

#create a list from the csv file
displaylist = []
idlist = []
bfreader = csv.reader(open('biofab.csv', 'rb'), delimiter=' ', quotechar='|')
for row in bfreader:
    displaylist.append( '    '.join(row))
    idlist.append( row[1] ) 

#ask user to select a part from the list
k = tc_getStringFromList("Please select a BioFAB part", displaylist)
s = idlist[k]  #to do: check for -1 case

#load performance data for user selected part
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

#load genbank data for user selected part
u = urllib2.urlopen('http://biofab.jbei.org/services/data/construct/design?id=' + s + '&format=genbank')
FILE = open('genbank.txt','w')
FILE.write(u.read())
FILE.close()

FILE = open('genbank.txt', 'rU')
for record in SeqIO.parse(FILE, 'genbank') :
    print record.id
FILE.close()

