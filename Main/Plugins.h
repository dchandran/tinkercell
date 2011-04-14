/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

load plugins as separate library

****************************************************************************/
#include "BasicInformationTool.h"
#include "StoichiometryTool.h"
#include "FunctionDeclarationsTool.h"
#include "ModelSummaryTool.h"
#include "ContainerTool.h"
#include "ModelFileGenerator.h"
#include "SimulationEventTool.h"
#include "ModuleTool.h"
#include "AutoGeneRegulatoryTool.h"
#include "CLabelsTool.h"
#include "DNASequenceTool.h"
#include "ViewTablesTool.h"
#include "LPSolveInput.h"
#include "CellPositionUpdatingTool.h"
#include "OctaveExporter.h"
#include "EnglishExporter.h"
#include "AntimonyEditor.h"
#include "SBMLImportExport.h"
#include "CopasiExporter.h"
#include "TinkerCellAboutBox.h"
#include "CollisionDetection.h"
#include "ConnectionInsertion.h"
#include "ConnectionMaker.h"
#include "NodeInsertion.h"
#include "GraphicsReplaceTool.h"
#include "GraphicsTransformTool.h"
#include "GroupHandlerTool.h"
#include "NameFamilyDialog.h"
#include "ConnectionSelection.h"
#include "NodeSelection.h"
#include <QColor>
#include <QBrush>

extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * mainWindow);

