/****************************************************************************

Copyright (c) 2010 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This tool generates multiple models by trying all available models for each process

****************************************************************************/

#ifndef TINKERCELL_MODULECOMBINATORICS_H
#define TINKERCELL_MODULECOMBINATORICS_H

#include "Tool.h"
#include "ModelFileGenerator.h"

class TINKERCELLEXPORT ModuleCombinatorics : public Tool
{
		Q_OBJECT

	public:

		ModuleCombinatorics();
	
	private:

		void getModelsFor(ItemFamily*, QList< QList<ItemHandle*> >&);
		void writeModels();
		void writeModels(int);
};

#endif

