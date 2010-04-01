import pytc
import sys
import re
import os

appDir = pytc.appDir();
fastaFile = appDir + '/DB/Regulon/genes.fasta';
bindingSitesFiles = [appDir + '/DB/Regulon/BindingSiteSet.txt'];
geneProductsFiles = [appDir + '/DB/Regulon/GeneProductSet.txt'];
networkFiles = [appDir + '/DB/Regulon/NetWorkSet.txt'];
promoterFiles = [appDir + '/DB/Regulon/PromoterSigma24Set.txt', appDir + '/DB/Regulon/PromoterSigma28Set.txt', appDir + '/DB/Regulon/PromoterSigma32Set.txt', appDir + '/DB/Regulon/PromoterSigma38Set.txt',appDir + '/DB/Regulon/PromoterSigma54Set.txt', appDir + '/DB/Regulon/PromoterSigma70Set.txt'];
rbsFiles = [appDir + '/DB/Regulon/RBSSet.txt'];
terminatorFiles = [appDir + '/DB/Regulon/TerminatorSet.txt'];

ECOLI_PROMOTERS = {};  #name -> [regulonID sequence]
ECOLI_RBS = {};  #name -> [regulonID sequence]
ECOLI_TERMINATORS = {};  #name -> [regulonID rho sequence]
ECOLI_GENE_SEQUENCES = {};   #name -> [sequence]
ECOLI_GENE_PRODUCT = {};   #name -> [protein]
ECOLI_GENETIC_NETWORK = {};  #double hash [tf1][tf2] -> + or -
ECOLI_BINDING_SITES_INTERACTIONS1 = {} #double hash [tf][bs] -> repressor or activator
ECOLI_BINDING_SITES_INTERACTIONS2 = {} #double hash [bs][tf] -> repressor or activator
ECOLI_BINDING_SITES_SEQUENCE = {} #bs -> [regulonID sequence]
ECOLI_TF = {} #tf -> [regulonID]

#read the promoter files and store in ECOLI_PROMOTERS hash
for fileName in promoterFiles:

	file = open(fileName,'r+');
	lines = file.readlines();
	
	for line in lines:
		words = line.split('\t');
		if (len(words) > 6):	
			id = (words[0]);
			name = (words[1]).lower();
			seq = (words[5]).lower();
			name = re.sub('[^\w]','',name);
			ECOLI_PROMOTERS[name] = [id, seq];
	file.close();

#read the rbs files and store in ECOLI_RBS hash
for fileName in rbsFiles:

	file = open(fileName,'r+');
	lines = file.readlines();
	
	for line in lines:
		words = line.split('\t');
		if (len(words) > 6):	
			id = (words[0]);
			name = (words[1]).lower();
			seq = (words[5]).lower();
			name = re.sub('[^\w]','',name);
			ECOLI_RBS[name] = [id, seq];
	file.close();

#read the terminator files and store in ECOLI_TERMINATORS hash
for fileName in terminatorFiles:

	file = open(fileName,'r+');
	lines = file.readlines();
	
	for line in lines:
		words = line.split('\t');
		if (len(words) > 6):	
			id = (words[0]);
			name = (words[5]).lower();
			seq = (words[4]).lower();
			rho = (words[6]).lower();
			name = re.sub('[^\w]','',name);
			ECOLI_TERMINATORS[name] = [id, rho, seq];
	file.close();
			
#read the fasta file and store in ECOLI_GENE_SEQUENCES hash
#file = open(fastaFile,'r+');
#lines = file.readlines();
#currentGene = '';
#currentSeq = '';
#p = re.compile('>(\S+)');

#for line in lines:
#	m = p.match(line);
#	if m:
#		if len(currentGene) > 0 and len(currentSeq) > 0:
#			ECOLI_GENE_SEQUENCES[currentGene] = currentSeq;
#		currentGene = m.group(1);
#	else:
#		if len(line) > 0:
#			currentSeq += line;
#file.close();

#read the network files and store in ECOLI_GENETIC_NETWORK double hash
for fileName in networkFiles:

	file = open(fileName,'r+');
	lines = file.readlines();

	for line in lines:
		words = line.split('\t');
		if (len(words) > 6):	
			tf1 = (words[1]).lower();
			protein = words[0];
			tf2 = (words[3]).lower();
			affect = -1;
			if (words[5].count('+') > 0):
				affect = 1;
			tf1 = re.sub('[^\w]','',tf1);
			tf2 = re.sub('[^\w]','',tf2);
			#insert value in table
			if (not ECOLI_GENETIC_NETWORK.has_key(tf2)):
				ECOLI_GENETIC_NETWORK[tf2] = {};
			ECOLI_GENETIC_NETWORK[tf2][tf1] = affect;
			ECOLI_GENE_PRODUCT[tf1] = protein;
	file.close();

#read the binding files and store in ECOLI_BINDING_SITES_INTERACTIONS1,2 double hash and ECOLI_BINDING_SITES_SEQUENCE
for fileName in bindingSitesFiles:

	file = open(fileName,'r+');
	lines = file.readlines();

	for line in lines:
		words = line.split('\t');
		if (len(words) > 6):	
			tf = (words[1]).lower();
			bs = (words[7]).lower();
			affect = (words[8]).lower();
			seq = (words[10]);
			
			tf = re.sub('[^\w]','',tf);
			#insert value in table
			if (not ECOLI_BINDING_SITES_INTERACTIONS1.has_key(tf)):
				ECOLI_BINDING_SITES_INTERACTIONS1[tf] = {};
			ECOLI_BINDING_SITES_INTERACTIONS1[tf][bs] = affect;
			
			if (not ECOLI_BINDING_SITES_INTERACTIONS2.has_key(bs)):
				ECOLI_BINDING_SITES_INTERACTIONS2[bs] = {};
			ECOLI_BINDING_SITES_INTERACTIONS2[bs][tf] = affect;
			
			
			ECOLI_BINDING_SITES_SEQUENCE[bs] = [words[2] , seq];
			ECOLI_TF[tf] = words[0];
	file.close();