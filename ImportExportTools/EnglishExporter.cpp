/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool exports English description of the model

****************************************************************************/
#include <QMessageBox>
#include <QDesktopServices>
#include "StoichiometryTool.h"
#include "BasicInformationTool.h"
#include "EnglishExporter.h"

namespace Tinkercell
{	
	EnglishExporter::EnglishExporter(): Tool(tr("English Export Tool"),tr("Export"))
	{
	}
	
	bool EnglishExporter::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);	
		if (!mainWindow) return false;

		if (mainWindow->fileMenu)
		{
			QList<QAction*> actions = mainWindow->fileMenu->actions();

			QAction * targetAction = 0;
			QMenu * exportmenu = 0;//, * importmenu = 0;
		
			for (int i=0; i < actions.size(); ++i)
				if (actions[i] && actions[i]->menu())
				{
					if (actions[i]->text() == tr("&Export"))
					{
						exportmenu = actions[i]->menu();
						targetAction = actions[i];
					}
					else
						if (actions[i]->text() == tr("&Import"))
						{
							targetAction = actions[i];
						}
				}
		
			if (!exportmenu)
			{
				for (int i=0; i < actions.size(); ++i)
					if (actions[i] && actions[i]->text() == tr("&Close page"))
					{
						exportmenu = new QMenu(tr("&Export"));
						mainWindow->fileMenu->insertMenu(actions[i],exportmenu);
					}
			}

			if (!exportmenu)
			{
				exportmenu = new QMenu(tr("&Export"));
				mainWindow->fileMenu->insertMenu(targetAction,exportmenu);
			}
		
			if (exportmenu)
			{
				exportmenu->addAction(tr("English description"),this,SLOT(exportEnglish()));
			}
		}
	
		return true;
	}

	
	void EnglishExporter::exportEnglish()
	{
		NetworkHandle * network = currentNetwork();
		if (!network)
		{
			QMessageBox::information(this,tr("No model"),tr("No model to export"));
			return;
		}

		QString filename = tempDir() + tr("/tinkercellmodel.txt");
		QFile file(filename);
		
		if (!file.open(QFile::WriteOnly | QFile::Text))
		{
			QMessageBox::information(this,tr("Write error"),tr("Cannot write to ") + filename);
			return;
		}
		
		QString txt;
		
		QList<ItemHandle*> handles = network->handles();
		QList<ConnectionHandle*> connectionHandles;
		
		for (int i=0; i < handles.size(); ++i)
			if (handles[i] &&
				!handles[i]->name.isEmpty() && 
				!handles[i]->parent && 
				handles[i]->family() &&
				ConnectionHandle::cast(handles[i]) &&
				handles[i]->hasTextData("Participants")
				)
				connectionHandles << ConnectionHandle::cast(handles[i]);
		
		for (int i=0; i <  connectionHandles.size(); ++i)
		{
			QString family = connectionHandles[i]->family()->name();
			TextDataTable & participants = connectionHandles[i]->textDataTable("participants");
			QStringList words = family.split(" ");
			QString verb = words.last();
			if (verb.endsWith("ation"))
				verb.replace("ation","es to");
			if (verb.endsWith("sion"))
				verb.replace("sion","ses");
			if (verb.endsWith("ction"))
				verb.replace("ction","ces");
			if (verb.endsWith("tion"))
				verb.replace("tion","ts");
			if (verb.endsWith("sis"))
				verb.replace("sis","ses");
			
			QList<NodeHandle*> nodesIn = connectionHandles[i]->nodesIn(),
											nodesOut = connectionHandles[i]->nodesOut();
			
			QStringList itemNames;
			for (int i=0; i < participants.rows(); ++i)
				itemNames += participants(i,0);
			
			QString in,out;
			for (int i=0; i < nodesIn.size(); ++i)
				if (itemNames.contains(nodesIn[i]->fullName()))
				{
					if (!nodesIn[i]->family() || nodesIn[i]->family()->isA("empty"))
						continue;
					if (i != 0)
						in += " and ";
					in += nodesIn[i]->family()->name() + tr(" ");
					int k = itemNames.indexOf(nodesIn[i]->fullName());
					in += tr("(") + participants.rowName(k) + tr(") ");
					in += nodesIn[i]->name;
				}
			
			for (int i=0; i < nodesOut.size(); ++i)
				if (itemNames.contains(nodesOut[i]->fullName()))
				{
					if (!nodesOut[i]->family() || nodesOut[i]->family()->isA("empty"))
						continue;
					if (i != 0)
						out += " and ";
					out += nodesOut[i]->family()->name() + tr(" ");
					int k = itemNames.indexOf(nodesOut[i]->fullName());
					out += tr("(") + participants.rowName(k) + tr(") ");
					out += nodesOut[i]->name;
				}
			if (!in.isEmpty() || !out.isEmpty())
				txt += in + tr("   ") + verb + tr("   ") + out + tr("\n");
		}
		
		file.write(txt.toUtf8());
		file.close();
		QDesktopServices::openUrl(QUrl(filename));
	}
}

