"""
PySCeS - Python Simulator for Cellular Systems (http://pysces.sourceforge.net)

Copyright (C) 2004-2009 B.G. Olivier, J.M. Rohwer, J.-H.S Hofmeyr all rights reserved,

Brett G. Olivier (bgoli@users.sourceforge.net)
Triple-J Group for Molecular Cell Physiology
Stellenbosch University, South Africa.

Permission to use, modify, and distribute this software is given under the
terms of the PySceS (BSD style) license. See LICENSE.txt that came with
this distribution for specifics.

NO WARRANTY IS EXPRESSED OR IMPLIED.  USE AT YOUR OWN RISK.
Brett G. Olivier
"""

from version import __version__

# Structural Analysis module
from pysces.PyscesStoich import Stoich
from PyscesCore2 import StructMatrix

class PyscesEnhancedStoich(Stoich):
    """PySCeS stoichiometry class for use with core2"""
    N = None
    Nr = None
    K = None
    K0 = None
    L = None
    L0 = None
    Gamma = None

    def __init__(self, core):
        Stoich.__init__(self, core.stoichiometric_matrix.array)
        self.species = core.stoichiometric_matrix.row
        self.reactions = core.stoichiometric_matrix.col

    def getNullSpaces(self):
        self.AnalyseK()
        self.AnalyseL()

    def testNullSpaces(self):
        #TODO: build in nullspace validity checks from PyscesModel
        pass

    def setStructMatrices(self):
        self.N = StructMatrix(self.nmatrix, self.nmatrix_row, self.nmatrix_col)
        self.N.setRow(self.species)
        self.N.setCol(self.reactions)

        self.Nr = StructMatrix(self.nrmatrix, self.nrmatrix_row, self.nrmatrix_col)
        self.Nr.setRow(self.species)
        self.Nr.setCol(self.reactions)

        self.K = StructMatrix(self.kmatrix, self.kmatrix_row, self.kmatrix_col)
        self.K.setRow(self.reactions)
        self.K.setCol(self.reactions)

        self.K0 = StructMatrix(self.kzeromatrix, self.kzeromatrix_row, self.kzeromatrix_col)
        self.K0.setRow(self.reactions)
        self.K0.setCol(self.reactions)

        self.L = StructMatrix(self.lmatrix, self.lmatrix_row, self.lmatrix_col)
        self.L.setRow(self.species)
        self.L.setCol(self.species)

        self.L0 = StructMatrix(self.lzeromatrix, self.lzeromatrix_row, self.lzeromatrix_col)
        self.L0.setRow(self.species)
        self.L0.setCol(self.species)

        if self.info_moiety_conserve:
            self.Gamma = StructMatrix(self.conservation_matrix, self.conservation_matrix_row, self.conservation_matrix_col)
            self.Gamma.setRow(self.species)
            self.Gamma.setCol(self.species)

class StructuralModule(object):
    core = None
    struct = None

    def setCore(self, core):
        self.core = core
        self.struct = None
        if self.core.stoichiometric_matrix == None:
            print "StructuralModule building stoichiometric matrix ..."
            self.core.setStoichiometricMatrix()

    def getCore(self):
        self.core.struct = self.struct
        return self.core

    def analyseStoichiometry(self):
        self.struct = PyscesEnhancedStoich(self.core)
        self.struct.getNullSpaces()
        self.struct.setStructMatrices()

# Integration Module
import numpy

class StateDataObj(object):
    flux = None
    flux_labels = None
    species = None
    species_labels = None
    valid = True
    _suffix = None
    _prefix = None

    def __init__(self):
        self.species_labels = []
        self.flux_labels = []

    def setSpecies(self, name, value, suffix=None):
        if suffix != None:
            name = name + suffix
        if name not in self.species_labels:
            self.species_labels.append(name)
        self._suffix = suffix
        setattr(self, name, value)

    def setFlux(self, name, value, prefix=None):
        if prefix != None:
            name = prefix + name
        if name not in self.flux_labels:
            self.flux_labels.append(name)
        self._prefix = prefix
        setattr(self, name, value)

    def setAllSpecies(self, species_labels, species, suffix=None):
        assert len(species_labels) == len(species), '\nThis aint gonna work1'
        self.species_labels = []
        ##  self.species_labels = tuple(species_labels)
        self.species = species.copy()
        for S in range(len(species_labels)):
            self.setSpecies(species_labels[S], species[S], suffix)

    def setAllFluxes(self, flux_labels, flux, prefix=None):
        assert len(flux_labels) == len(flux), '\nThis aint gonna work2'
        self.flux_labels = []
        ##  self.flux_labels = tuple(flux_labels)
        self.flux = flux.copy()
        for J in range(len(flux_labels)):
            self.setFlux(flux_labels[J], flux[J], prefix)

    def getFlux(self, name):
        if prefix != None:
            name = prefix + name
        return getattr(self, name)

    def getSpecies(self, name):
        if suffix != None:
            name = name + suffix
        return getattr(self, name)


class IntegrationDataObj(object):
    """
    This class is specifically designed to store the results of a time simulation
    It has methods for setting the Time, Labels, Species and Rate data and
    getting Time, Species and Rate (including time) arrays. However, of more use:
    - getOutput(*arg) feed this method species/rate labels and it will return
      an array of [time, sp1, r1, ....]
    - getDataAtTime(time) the data generated at time point "time".
    - getDataInTimeInterval(time, bounds=None) more intelligent version of the above
      returns an array of all data points where: time-bounds <= time <= time+bounds
      where bounds defaults to stepsize.
    """
    time = None
    rates = None
    species = None
    rate_labels = None
    species_labels = None

    def setLabels(self, species, rates):
        """set the species and rate label lists"""
        self.species_labels = species
        self.rate_labels = rates

    def setTime(self, time):
        """Set the time vector"""
        self.time = time.reshape(len(time), 1)

    def setSpecies(self, species):
        """Set the species array"""
        self.species = species

    def setRates(self, rates):
        """set the rate array"""
        self.rates = rates

    def getTime(self):
        """return the time vector"""
        assert self.time != None, "\nNo time"
        return self.time.reshape(len(self.time),)

    def getSpecies(self):
        """return time+species array"""
        assert self.species != None, "\nNo species"
        return numpy.hstack((self.time, self.species))

    def getRates(self):
        """return time+rate array"""
        assert self.rates != None, "\nNo rates"
        return numpy.hstack((self.time, self.rates))

    def getDataAtTime(self, time):
        """Return all data generated at "time" """
        t = None
        sp = None
        ra = None
        temp_t = self.time.reshape(len(self.time),)
        for tt in range(len(temp_t)):
            if temp_t[tt] == time:
                t = tt
                if self.species is not None:
                    sp = self.species.take([tt], axis=0)
                if self.rates is not None:
                    ra = self.rates.take([tt], axis=0)
                break
        output = None
        if t is not None:
            output = numpy.array([[temp_t[t]]])
            if sp is not None:
                output = numpy.hstack((output,sp))
            if ra is not None:
                output = numpy.hstack((output,ra))
        return output

    def getDataInTimeInterval(self, time, bounds=None):
        """
         getDataInTimeInterval(time, bounds=None) returns an array of all
         data points where: time-bounds <= time <= time+bounds
         where bound defaults to stepsize
        """
        temp_t = self.time.reshape(len(self.time),)
        if bounds == None:
            bounds = temp_t[1] - temp_t[0]
        c1 = (temp_t >= time-bounds)
        c2 = (temp_t <= time+bounds)
        print 'Searching (%s:%s:%s)' % (time-bounds, time, time+bounds)

        t = []
        sp = None
        ra = None

        for tt in range(len(c1)):
            if c1[tt] and c2[tt]:
                t.append(tt)
        output = None
        if len(t) > 0:
            output = self.time.take(t)
            output = output.reshape(len(output),1)
            if self.species is not None:
                output = numpy.hstack((output, self.species.take(t, axis=0)))
            if self.rates is not None:
                output = numpy.hstack((output, self.rates.take(t, axis=0)))
        return output

    def getOutput(self, *args):
        """getOutput(*arg) feed this method species/rate labels and it
        will return an array of [time, sp1, r1, ....]
        """
        output = self.time
        for roc in args:
            if roc in self.species_labels:
                assert self.species != None, "\nNo species"
                output = numpy.hstack((output, self.species.take([self.species_labels.index(roc)], axis=-1)))
            if roc in self.rate_labels:
                assert self.rates != None, "\nNo rates"
                output = numpy.hstack((output, self.rates.take([self.rate_labels.index(roc)], axis=-1)))
        return output

#TODO:
class IntegrationBase(object):
    name = None
    core = None
    data = None

    sim_start = None
    sim_end = None
    sim_point = None
    initial_value_vector = None


    def setName(self, name):
        self.name = name

    def getName(self):
        return self.name

    def setCore(self, core):
        self.core = core
        self.data = IntegrationData()
        self.data.setLabels(self.core.hasVariableSpecies(), self.core.hasReactions())

    def getCore(self):
        return self.core

    def getData(self):
        return self.data






