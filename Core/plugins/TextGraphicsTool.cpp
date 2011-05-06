/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This tool allows users to edit a text graphics item on the sceen.
The tool also updates the name of a handle when the text item is changed (and vise versa)

****************************************************************************/

#include <QKeyEvent>
#include "MainWindow.h"
#include "GraphicsScene.h"
#include "UndoCommands.h"
#include "ConnectionGraphicsItem.h"
#include "NodeGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "TextGraphicsTool.h"

namespace Tinkercell
{
	TextGraphicsTool::TextGraphicsTool(QToolBar * toolBar) : Tool(tr("Text Insert Tool"),tr("Basic GUI")), toolBar(toolBar)
	{
		inserting = false;
		command = 0;
		targetItem = 0;
		fontAction = new QAction(tr("S&et font"),this);
		connect(fontAction,SIGNAL(triggered()),this,SLOT(getFont()));
		oldText = tr("");
		font.setPointSize(22);
	}

	bool TextGraphicsTool::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			if (toolBar)
			{
				QToolButton * insertText = new QToolButton(toolBar);
				insertText->setIcon(QIcon(tr(":/images/text.png")));
				insertText->setToolTip(tr("Insert text"));
				connect(insertText,SIGNAL(pressed()),this,SLOT(insertText()));

				QMenu * fontMenu = new QMenu(toolBar);
				fontMenu->addAction(tr("&Set font"),this,SLOT(getFont()));
				insertText->setMenu(fontMenu);
				insertText->setPopupMode(QToolButton::MenuButtonPopup);
				toolBar->addWidget(insertText);
			}

			QAction * insertText = new QAction("Insert text",this);
			insertText->setIcon(QIcon(tr(":/images/text.png")));
			insertText->setToolTip(tr("Insert text"));
			connect(insertText,SIGNAL(triggered()),this,SLOT(insertTextWith()));

			mainWindow->contextItemsMenu.addAction(insertText);
			
			if (mainWindow->editMenu)
			{
				mainWindow->editMenu->addAction(insertText);
				mainWindow->editMenu->addAction(tr("S&et font"),this,SLOT(getFont()));
			}

			connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),this, SLOT(escapeSignal(const QWidget*)));

			connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene*,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
				this, SLOT(itemsSelected(GraphicsScene*,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(mousePressed(GraphicsScene*,QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),
				this ,SLOT(mousePressed(GraphicsScene*,QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(itemsAboutToBeRemoved(GraphicsScene*, QList<QGraphicsItem*>&, QList<ItemHandle*>&, QList<QUndoCommand*>&)),
				this,SLOT(itemsRemoved(GraphicsScene*, QList<QGraphicsItem*>&, QList<ItemHandle*>&, QList<QUndoCommand*>&)));

			//connect(mainWindow,SIGNAL(mouseDoubleClicked(GraphicsScene *, QPointF, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers)),
			//this,SLOT(mouseDoubleClicked(GraphicsScene *, QPointF, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(keyPressed(GraphicsScene*,QKeyEvent*)),
				this ,SLOT(keyPressed(GraphicsScene*,QKeyEvent*)));


		}
		return (mainWindow != 0);
	}

	void TextGraphicsTool::itemsInserted(GraphicsScene*, const QList<QGraphicsItem*>& items, const QList<ItemHandle*>&)
	{
		ItemHandle * handle = 0;
		for (int i=0; i < items.size(); ++i)
		{
			if (TextGraphicsItem::cast(items[i]))
			{
				QPointF p0 = items[i]->scenePos();
				qreal dist = 0;
				handle = getHandle(items[i]);
				if (handle)
					for (int j=0; j < handle->graphicsItems.size(); ++j)
					{
						if (!TextGraphicsItem::cast(handle->graphicsItems[j]))
						{
							QPointF p1;
							ConnectionGraphicsItem * connection = ConnectionGraphicsItem::cast(handle->graphicsItems[j]);
							if (connection && connection->centerPoint())
							{
								p1 = connection->centerLocation();

								if (dist == 0 || ((p1.rx()-p0.rx())*(p1.rx()-p0.rx()) + (p1.ry()-p0.ry())*(p1.ry()-p0.ry())) < dist)
								{
									dist = ((p1.rx()-p0.rx())*(p1.rx()-p0.rx()) + (p1.ry()-p0.ry())*(p1.ry()-p0.ry()));
									(TextGraphicsItem::cast(items[i]))->relativePosition = QPair<QGraphicsItem*,QPointF>(connection,p0 - p1);
								}
							}
							else
							{
								NodeGraphicsItem * node = NodeGraphicsItem::cast(handle->graphicsItems[j]);
								if (node)
								{
									if (node->boundaryControlPoints.isEmpty())
									{
										p1 = node->scenePos();
										if (dist == 0 || ((p1.rx()-p0.rx())*(p1.rx()-p0.rx()) + (p1.ry()-p0.ry())*(p1.ry()-p0.ry())) < dist)
										{
											dist = ((p1.rx()-p0.rx())*(p1.rx()-p0.rx()) + (p1.ry()-p0.ry())*(p1.ry()-p0.ry()));
											(TextGraphicsItem::cast(items[i]))->relativePosition = QPair<QGraphicsItem*,QPointF>(node,p0 - p1);
										}
									}
									else
									{
										for (int k=0; k < node->boundaryControlPoints.size(); ++k)
											if (node->boundaryControlPoints[k])
											{	
												p1 = node->boundaryControlPoints[k]->scenePos();
												if (dist == 0 || ((p1.rx()-p0.rx())*(p1.rx()-p0.rx()) + (p1.ry()-p0.ry())*(p1.ry()-p0.ry())) < dist)
												{
													dist = ((p1.rx()-p0.rx())*(p1.rx()-p0.rx()) + (p1.ry()-p0.ry())*(p1.ry()-p0.ry()));
													(TextGraphicsItem::cast(items[i]))->relativePosition = QPair<QGraphicsItem*,QPointF>(node->boundaryControlPoints[k],p0 - p1);
												}
											}
									}
								}
							}
						}
					}
			}
		}
	}

	void TextGraphicsTool::itemsAboutToBeMoved(GraphicsScene * scene, QList<QGraphicsItem*>& items, QList<QPointF>& dists, QList<QUndoCommand*>&)
	{
		static bool selfCall = false;

		if (!scene || selfCall) return;
		ItemHandle * handle = 0;

		selfCall = false;

		QList<QGraphicsItem*> nonTextItems;
		QList<ItemHandle*> visited;

		for (int i=0; i < items.size(); ++i)
			if (items[i])
			{
				if (TextGraphicsItem::cast(items[i]))
				{
					QPointF p0 = items[i]->scenePos();
					qreal dist = 0;
					handle = getHandle(items[i]);
					if (handle)
					{
						visited += handle;
						for (int j=0; j < handle->graphicsItems.size(); ++j)
						{
							if (handle->graphicsItems[j] && !TextGraphicsItem::cast(handle->graphicsItems[j]))
							{
								QPointF p1;
								ConnectionGraphicsItem * connection = ConnectionGraphicsItem::cast(handle->graphicsItems[j]);
								if (connection && connection->centerPoint())
								{
									p1 = connection->centerLocation();
									if (dist == 0 || ((p1.rx()-p0.rx())*(p1.rx()-p0.rx()) + (p1.ry()-p0.ry())*(p1.ry()-p0.ry())) < dist)
									{
										dist = ((p1.rx()-p0.rx())*(p1.rx()-p0.rx()) + (p1.ry()-p0.ry())*(p1.ry()-p0.ry()));
										(TextGraphicsItem::cast(items[i]))->relativePosition = QPair<QGraphicsItem*,QPointF>(connection,p0 - p1);
									}
								}
								else
								{
									NodeGraphicsItem * node = NodeGraphicsItem::cast(handle->graphicsItems[j]);
									if (node)
									{
										if (node->boundaryControlPoints.isEmpty())
										{
											p1 = node->scenePos();
											if (dist == 0 || ((p1.rx()-p0.rx())*(p1.rx()-p0.rx()) + (p1.ry()-p0.ry())*(p1.ry()-p0.ry())) < dist)
											{
												dist = ((p1.rx()-p0.rx())*(p1.rx()-p0.rx()) + (p1.ry()-p0.ry())*(p1.ry()-p0.ry()));
												(TextGraphicsItem::cast(items[i]))->relativePosition = QPair<QGraphicsItem*,QPointF>(node,p0 - p1);
											}
										}
										else
										{
											for (int k=0; k < node->boundaryControlPoints.size(); ++k)
												if (node->boundaryControlPoints[k])
												{	
													p1 = node->boundaryControlPoints[k]->scenePos();
													if (dist == 0 || ((p1.rx()-p0.rx())*(p1.rx()-p0.rx()) + (p1.ry()-p0.ry())*(p1.ry()-p0.ry())) < dist)
													{
														dist = ((p1.rx()-p0.rx())*(p1.rx()-p0.rx()) + (p1.ry()-p0.ry())*(p1.ry()-p0.ry()));
														(TextGraphicsItem::cast(items[i]))->relativePosition = QPair<QGraphicsItem*,QPointF>(node->boundaryControlPoints[k],p0 - p1);
													}
												}
										}
									}
								}
							}
						}
					}
				}
				else
				{
					nonTextItems += items[i];
					NodeGraphicsItem * node = NodeGraphicsItem::cast(items[i]);
					if (node) nonTextItems << node->connectionsAsGraphicsItems();
				}
			}

		ConnectionGraphicsItem::ControlPoint * ccp = 0;
		NodeGraphicsItem::ControlPoint * pcp = 0;


		QList<QGraphicsItem*> itemsToMove;
		QList<QPointF> moveBy;
		for (int i=0; i < nonTextItems.size(); ++i)
		{
			handle = getHandle(nonTextItems[i]);

			if (!handle)
			{
				if ((ccp = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(nonTextItems[i]))
					&& ccp->connectionItem && ccp->connectionItem->handle())
					handle = ccp->connectionItem->handle();

				if ((pcp = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(nonTextItems[i]))
					&& pcp->nodeItem && pcp->nodeItem->handle())
					handle = pcp->nodeItem->handle();
			}

			if (handle && !visited.contains(handle))
				for (int j=0; j < handle->graphicsItems.size(); ++j)
				{
					if (TextGraphicsItem::cast(handle->graphicsItems[j]) && 
						!items.contains(handle->graphicsItems[j]) &&
						!itemsToMove.contains(handle->graphicsItems[j]))
					{
						QPointF p;
						QPointF p0 = handle->graphicsItems[j]->scenePos();
						for (int k=0; k < handle->graphicsItems.size(); ++k)
							if (!TextGraphicsItem::cast(handle->graphicsItems[k]) && k != j)
							{
								QPointF p1;
								QPair<QGraphicsItem*,QPointF> relativePoisition = (TextGraphicsItem::cast(handle->graphicsItems[j]))->relativePosition;
								if (!relativePoisition.first) continue;

								if (ConnectionGraphicsItem::cast(relativePoisition.first))
									p1 = (ConnectionGraphicsItem::cast(relativePoisition.first))->centerLocation();
								else
									p1 = relativePoisition.first->scenePos();

								p = relativePoisition.second + p1 - p0;
							}
							if (p.rx() != 0 || p.ry() != 0)
							{
								itemsToMove += handle->graphicsItems[j];
								moveBy += p;
							}
					}
				}
		}
		if (!itemsToMove.isEmpty())
		{
			selfCall = true;
			items += itemsToMove;
			dists += moveBy;
			//scene->move(itemsToMove,moveBy);
		}
		selfCall = false;
	}

	void TextGraphicsTool::escapeSignal(const QWidget* )
	{
		if (inserting)
		{
			mainWindow->setCursor(Qt::ArrowCursor);
			if (mainWindow && mainWindow->currentScene())
				mainWindow->currentScene()->useDefaultBehavior(true);
			inserting = false;
		}
	}

	void TextGraphicsTool::setText(TextGraphicsItem * item, const QString& name0)
	{
		if (item == 0) return;

		QString oldText = item->toPlainText();

		QString name = name0.trimmed();

		if (oldText.trimmed() == name) return;

		if (mainWindow)
		{
			QUndoCommand * command = 0;
			if (!name.isEmpty() && mainWindow->currentNetwork() && item->handle() && item->handle()->name == oldText)
			{
				QString newName = Tinkercell::RemoveDisallowedCharactersFromName(name);
				
				if (newName == name)
				{
					//item->handle()->name = newName;
					if (currentNetwork())
						currentNetwork()->rename(getHandle(item),newName);
					return;
				}
			}
			
			if (name.isEmpty())
				command = new RemoveGraphicsCommand(tr("text removed"),item);
			else
				command = new ChangeTextCommand(tr("text changed"),item,name);
			if (mainWindow->historyStack())
				mainWindow->historyStack()->push(command);
			else
			{
				command->redo();
				delete command;
			}
		}
	}

	void TextGraphicsTool::insertText()
	{
		if (mainWindow && mainWindow->currentScene())
		{
			mainWindow->sendEscapeSignal(this);
			mainWindow->setCursor(Qt::IBeamCursor);
			mainWindow->currentScene()->useDefaultBehavior(false);
			inserting = true;
		}
	}

	void TextGraphicsTool::insertTextWith()
	{
		if (mainWindow && mainWindow->currentScene())
		{
			QList<QGraphicsItem*>& selected = mainWindow->currentScene()->selected();
			ItemHandle * handle = 0;
			QList<QGraphicsItem*> textItems;
			for (int i=0; i < selected.size(); ++i)
			{
				if (selected[i] && selected[i]->isVisible() && (handle = getHandle(selected[i])))
				{
					TextGraphicsItem * newText = new TextGraphicsItem;
					newText->setPlainText("new text");
					newText->setFont(font);
					newText->setPos(selected[i]->sceneBoundingRect().center().x() - 5.0, selected[i]->sceneBoundingRect().bottom() + 5.0); 
					setHandle(newText,handle);
					textItems += newText;
				}
			}
			if (!textItems.isEmpty())
				mainWindow->currentScene()->insert("text inserted",textItems);
		}
	}

	void TextGraphicsTool::itemsRemoved(GraphicsScene* scene,  QList<QGraphicsItem*>& items, QList<ItemHandle*>& handles, QList<QUndoCommand*>&)
	{
		if (!scene || items.isEmpty()) return;

		TextGraphicsItem * text;
		bool del;

		for (int i=0; i < handles.size(); ++i)
			if (handles[i])
				for (int j=0; j < handles[i]->graphicsItems.size(); ++j)
					if ((text = TextGraphicsItem::cast(handles[i]->graphicsItems[j])) &&
						!items.contains(text))
					{
						del = true;
						for (int k=0; k < handles[i]->graphicsItems.size(); ++k)
							if (!TextGraphicsItem::cast(handles[i]->graphicsItems[k]) &&
								!items.contains(handles[i]->graphicsItems[k]) &&
								handles[i]->graphicsItems[k]->sceneBoundingRect().adjusted(-100,-100,200,200).intersects(text->sceneBoundingRect()))
							{
								del = false;
								break;
							}
							if (del)
								items += text;
					}

					if (!targetItem) return;
					for (int i=0; i < items.size(); ++i)		
						if (targetItem == items[i])
						{
							if (command)
							{
								delete command;
								command = 0;
							}
							break;
						}
	}

	void TextGraphicsTool::clear()
	{
		if (targetItem)
		{
			QString text = targetItem->toPlainText();
			targetItem->setPlainText(oldText);
			setText(targetItem,text);
			targetItem = 0;
		}
		if (command)
		{
			command->undo();
			delete command;
			command = 0;				
		}
	}
	void TextGraphicsTool::mousePressed(GraphicsScene * scene,QPointF point, Qt::MouseButton , Qt::KeyboardModifiers )
	{
		if (mainWindow && scene && inserting)
		{
			TextGraphicsItem * textItem = new TextGraphicsItem(tr("new text"));
			textItem->setFont(font);
			textItem->setPos(point);

			scene->insert(tr("text inserted"),textItem);

			targetItem = textItem;

			targetItem->setSelected(true);
			textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
			QTextCursor c = targetItem->textCursor();
			c.movePosition(QTextCursor::EndOfLine);
			c.movePosition(QTextCursor::StartOfLine,QTextCursor::KeepAnchor);
			targetItem->setTextCursor(c);

			mainWindow->setCursor(Qt::ArrowCursor);
			scene->useDefaultBehavior(true);
			inserting = false;
		}
		else
		{
			clear();
		}
	}
	void TextGraphicsTool::itemsSelected(GraphicsScene * scene,const QList<QGraphicsItem*>& items, QPointF , Qt::KeyboardModifiers )
	{
		if (scene == 0) return;

		if (items.size() == 1)
		{
			TextGraphicsItem * textItem = TextGraphicsItem::cast(items[0]);

			if (targetItem)
				clear();

			if (!textItem && scene->contextItemsMenu)
			{
				scene->contextItemsMenu->removeAction(fontAction);
				return;
			}
			
			if (scene->contextItemsMenu)
				scene->contextItemsMenu->addAction(fontAction);

			ItemHandle * handle = getHandle(items[0]);
			ItemHandle * handle2 = 0;

			QList<QGraphicsItem*> itemsAt = scene->items(items[0]->sceneBoundingRect().adjusted(-50.0,-50.0,50.0,50.0));
			if (handle)
			{
				for (int i=0; i < itemsAt.size(); ++i)
					if (handle == getHandle(itemsAt[i]) && handle->name == textItem->toPlainText())
						break;
			}
			else
			{
				for (int i=0; i < itemsAt.size(); ++i)
					if ((handle2 = getHandle(itemsAt[i])) && handle2->name == textItem->toPlainText())
					{
						QUndoCommand * handleChange = new AssignHandleCommand(tr("text assigned"),textItem,handle2);
						if (scene->network)
							scene->network->push(handleChange);
						else
						{
							handleChange->redo();
							delete handleChange;
						}
						break;
					}
			}

			oldText = textItem->toPlainText();
			targetItem = textItem;
		}
		else
			if (targetItem)
				clear();
	}

	void TextGraphicsTool::keyPressed (GraphicsScene* scene,QKeyEvent * keyEvent)
	{
		if (scene && keyEvent)
		{
			if (!targetItem && scene->selected().size() == 1 && scene->selected()[0])
			{
				TextGraphicsItem * textItem = TextGraphicsItem::cast(scene->selected()[0]->topLevelItem());
				if (textItem)
				{
					oldText = textItem->toPlainText();
					targetItem = textItem;
				}
			}
		}
	}

	void TextGraphicsTool::mouseDoubleClicked (GraphicsScene * scene, QPointF , QGraphicsItem * item, Qt::MouseButton , Qt::KeyboardModifiers )
	{
	}

	void TextGraphicsTool::getFont()
	{
		GraphicsScene * scene = currentScene(); 
		if (!scene) return;

		bool ok;
		QFont newFont = QFontDialog::getFont(&ok, font, mainWindow, tr("Select Font"));
		if (ok)
		{
			font = newFont;
			QList<QGraphicsItem*> items;
			QList<QString> texts;
			QList<QFont> fonts;

			QList<QGraphicsItem*>& selected = scene->selected();
			TextGraphicsItem * textItem;
			for (int i=0; i < selected.size(); ++i) 
			{
				if ((textItem = TextGraphicsItem::cast(selected[i])))
				{
					texts += textItem->toPlainText();
					fonts += newFont;
					items += selected[i];
				}
			}
			QUndoCommand * command = new ChangeTextCommand(tr("font(s) changed"),items,texts,fonts);
			if (scene->network)
				scene->network->push(command);
			else
			{
				command->redo();
				delete command;
			}
		}
	}

	/*****************
	TEXT UNDO COMMAND
	******************/

	ChangeTextCommand::ChangeTextCommand(const QString& name, QGraphicsItem * item, const QString& newname)
		: QUndoCommand(name)
	{
		textItems.clear();
		oldText.clear();
		newText.clear();
		newFont.clear();
		oldFont.clear();

		TextGraphicsItem * text = TextGraphicsItem::cast(item);

		if (text)
		{
			textItems.append(text);
			oldText.append(text->toPlainText());
			newText.append(newname);
		}
	}

	ChangeTextCommand::ChangeTextCommand(const QString& name, const QList<QGraphicsItem*>& items, const QList<QString>& newnames)
		: QUndoCommand(name)
	{
		textItems.clear();
		oldText.clear();
		newText.clear();
		newFont.clear();
		oldFont.clear();

		for (int i=0; i < items.size() && i < newnames.size() ; ++i)
		{

			TextGraphicsItem * text = TextGraphicsItem::cast(items[i]);

			if (text)
			{
				textItems.append(text);
				oldText.append(text->toPlainText());
				newText.append(newnames[i]);
			}
		}
	}

	ChangeTextCommand::ChangeTextCommand(const QString& name, QGraphicsItem * item, const QString& newname, const QFont& font)
		: QUndoCommand(name)
	{
		textItems.clear();
		oldText.clear();
		newText.clear();
		newFont.clear();
		oldFont.clear();

		TextGraphicsItem * text = TextGraphicsItem::cast(item);

		if (text)
		{
			textItems.append(text);
			oldText.append(text->toPlainText());
			oldFont.append(text->font());
			newText.append(newname);
			newFont.append(font);
		}
	}

	ChangeTextCommand::ChangeTextCommand(const QString& name,const QList<QGraphicsItem*>& items, const QList<QString>& newnames, const QList<QFont>& fonts)
		: QUndoCommand(name)
	{
		textItems.clear();
		oldText.clear();
		newText.clear();
		newFont.clear();
		oldFont.clear();

		for (int i=0; i < items.size() && i < newnames.size() ; ++i)
		{

			TextGraphicsItem * text = TextGraphicsItem::cast(items[i]);

			if (text)
			{
				textItems.append(text);
				oldText.append(text->toPlainText());
				newText.append(newnames[i]);								
				oldFont.append(text->font());
				newFont.append(fonts[i]);
			}
		}
	}	

	void ChangeTextCommand::redo()
	{
		for (int i=0; i < textItems.size() && i < newText.size() && i < oldText.size(); ++i)
		{
			if (textItems[i] != 0)
			{
				TextGraphicsItem * text = TextGraphicsItem::cast(textItems[i]);

				if (text)
				{
					text->setPlainText(newText.at(i));
					if (newFont.size() > i)
						text->setFont(newFont[i]);
				}
			}
		}
	}

	void ChangeTextCommand::undo()
	{
		for (int i=0; i < textItems.size() && i < oldText.size(); ++i)
		{
			if (textItems[i] != 0)
			{
				TextGraphicsItem * text = TextGraphicsItem::cast(textItems[i]);

				if (text)
				{
					text->setPlainText(oldText.at(i));
					if (oldFont.size() > i)
						text->setFont(oldFont[i]);
				}
			}
		}
	}

}
