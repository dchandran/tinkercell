#ifndef MODULAR_REACTIONBLOCKS_H
#define MODULAR_REACTIONBLOCKS_H

namespace ModularNetworks
{
	class ReactionBlock
	{
		int type;
		int * internals;            //molecules that cannot interact with other modules
		int * externals;            //molecules that can interact with other modules
		double * params;            //parameters used in the rate equations of this module
		double * initValsExternals; //initial concentrations for external molecules in this module
		double * initValsInternals; 
	};

	class SystemOfBlocks
	{
	};
}

#endif

