/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

load plugins as separate library
****************************************************************************/

#include "Plugins.h"

using namespace Tinkercell;

extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * mainWindow)
{
	if (!mainWindow) return;
	
	mainWindow->addTool(new CatalogWidget);
	mainWindow->addTool(new CollisionDetection);
	mainWindow->addTool(new ConnectionInsertion);
	mainWindow->addTool(new NodeInsertion);
	mainWindow->addTool(new NodeSelection);
	mainWindow->addTool(new ConnectionSelection);
	mainWindow->addTool(new TinkercellAboutBox);
	mainWindow->addTool(new GraphicsReplaceTool);
	mainWindow->addTool(new GraphicsTransformTool);
	mainWindow->addTool(new GroupHandlerTool);
	mainWindow->addTool(new NameFamilyDialog);
	mainWindow->addTool(new ConnectionMaker);
	
	mainWindow->addTool(new BasicInformationTool(QString("text")));
	mainWindow->addTool(new BasicInformationTool(QString("numerical")));
	mainWindow->addTool(new AssignmentFunctionsTool);
	mainWindow->addTool(new StoichiometryTool);
	mainWindow->addTool(new ModelSummaryTool);
	mainWindow->addTool(new ModelFileGenerator);
	mainWindow->addTool(new SimulationEventsTool);	
	mainWindow->addTool(new CompartmentTool);
	
	mainWindow->addTool(new OctaveExporter);
	mainWindow->addTool(new SBMLImportExport);
	mainWindow->addTool(new CopasiExporter);
	mainWindow->addTool(new AntimonyEditor);
	mainWindow->addTool(new EnglishExporter);
	
	mainWindow->addTool(new ModuleTool);
	mainWindow->addTool(new AutoGeneRegulatoryTool);
	mainWindow->addTool(new CLabelsTool);
	mainWindow->addTool(new DNASequenceViewer);
	mainWindow->addTool(new ViewTablesTool);
	mainWindow->addTool(new LPSolveInputWindow);
	mainWindow->addTool(new CellPositionUpdateTool);

//	mainWindow->addTool(new ModuleCombinatorics);
}

