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
	TextGraphicsTool::TextGraphicsTool() : Tool(tr("Text Insert Tool"))
	{
		inserting = false;
		command = 0;
		targetItem = 0;
		oldText = tr("");

		toolBar = new QToolBar(tr("Text tool"),this);
		QToolButton * insertText = new QToolButton(toolBar);
		insertText->setIcon(QIcon(tr(":/images/text.png")));
		insertText->setToolTip(tr("Insert text"));
		connect(insertText,SIGNAL(pressed()),this,SLOT(insertText()));

		QMenu * fontMenu = new QMenu(toolBar);
		fontMenu->addAction(tr("Set Font"),this,SLOT(getFont()));
		insertText->setMenu(fontMenu);
		insertText->setPopupMode(QToolButton::MenuButtonPopup);
		toolBar->addWidget(insertText);

		font.setPointSize(22);

		/*


		widget.setPalette(QPalette(QColor(255,255,255,255)));
		widget.setAutoFillBackground(true);

		QSlider * scaleSlider = new QSlider;
		scaleSlider->setRange(1,100);
		scaleSlider->setValue(50);
		scaleSlider->setOrientation(Qt::Horizontal);

		QVBoxLayout * layout = new QVBoxLayout;

		QHBoxLayout * scaleLayout = new QHBoxLayout;
		QGroupBox * scaleGroup = new QGroupBox(tr("Scale"));
		scaleLayout->addWidget(scaleSlider);
		scaleGroup->setLayout(scaleLayout);
		layout->addWidget(scaleGroup);

		QTextEdit * textBox = new QTextEdit;

		layout->addWidget(textBox);

		widget.setLayout(layout);*/
	}

	bool TextGraphicsTool::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			mainWindow->addToolBar(toolBar);

			QAction * insertText = new QAction("Insert text",this);
			insertText->setIcon(QIcon(tr(":/images/text.png")));
			insertText->setToolTip(tr("Insert text"));
			connect(insertText,SIGNAL(triggered()),this,SLOT(insertTextWith()));

			mainWindow->contextItemsMenu.addAction(insertText);

			connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),this, SLOT(escapeSignal(const QWidget*)));

			connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene*,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
				this, SLOT(itemsSelected(GraphicsScene*,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(itemsInserted(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)),
				this, SLOT(itemsInserted(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)));

			connect(mainWindow,SIGNAL(itemsMoved(GraphicsScene*,const QList<QGraphicsItem*>&, const QList<QPointF>&, Qt::KeyboardModifiers)),
				this, SLOT(itemsMoved(GraphicsScene*,const QList<QGraphicsItem*>&, const QList<QPointF>&, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(mousePressed(GraphicsScene*,QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),
				this ,SLOT(mousePressed(GraphicsScene*,QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));

			//connect(this,SIGNAL(itemsRenamed(NetworkWindow*, const QList<ItemHandle*>&, const QList<QString>&, const QList<QString>&)),
			//       mainWindow,SIGNAL(itemsRenamed(NetworkWindow*, const QList<ItemHandle*>&, const QList<QString>&, const QList<QString>&)));

			connect(mainWindow,SIGNAL(itemsAboutToBeRemoved(GraphicsScene*, QList<QGraphicsItem*>&, QList<ItemHandle*>&)),
				this,SLOT(itemsRemoved(GraphicsScene*, QList<QGraphicsItem*>&, QList<ItemHandle*>&)));

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
			if (qgraphicsitem_cast<TextGraphicsItem*>(items[i]))
			{
				QPointF p0 = items[i]->scenePos();
				qreal dist = 0;
				handle = getHandle(items[i]);
				if (handle)
					for (int j=0; j < handle->graphicsItems.size(); ++j)
					{
						if (!qgraphicsitem_cast<TextGraphicsItem*>(handle->graphicsItems[j]))
						{
							QPointF p1;
							ConnectionGraphicsItem * connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(handle->graphicsItems[j]);
							if (connection && connection->centerPoint())
							{
								p1 = connection->centerLocation();

								if (dist == 0 || ((p1.rx()-p0.rx())*(p1.rx()-p0.rx()) + (p1.ry()-p0.ry())*(p1.ry()-p0.ry())) < dist)
								{
									dist = ((p1.rx()-p0.rx())*(p1.rx()-p0.rx()) + (p1.ry()-p0.ry())*(p1.ry()-p0.ry()));
									(qgraphicsitem_cast<TextGraphicsItem*>(items[i]))->relativePosition = QPair<QGraphicsItem*,QPointF>(connection,p0 - p1);
								}
							}
							else
							{
								NodeGraphicsItem * node = qgraphicsitem_cast<NodeGraphicsItem*>(handle->graphicsItems[j]);
								if (node)
								{
									for (int k=0; k < node->boundaryControlPoints.size(); ++k)
										if (node->boundaryControlPoints[k])
										{	
											p1 = node->boundaryControlPoints[k]->scenePos();
											if (dist == 0 || ((p1.rx()-p0.rx())*(p1.rx()-p0.rx()) + (p1.ry()-p0.ry())*(p1.ry()-p0.ry())) < dist)
											{
												dist = ((p1.rx()-p0.rx())*(p1.rx()-p0.rx()) + (p1.ry()-p0.ry())*(p1.ry()-p0.ry()));
												(qgraphicsitem_cast<TextGraphicsItem*>(items[i]))->relativePosition = QPair<QGraphicsItem*,QPointF>(node->boundaryControlPoints[k],p0 - p1);
											}
										}
								}
							}
						}
					}
			}
		}
	}

	void TextGraphicsTool::itemsMoved(GraphicsScene* scene,const QList<QGraphicsItem*>& items, const QList<QPointF>&, Qt::KeyboardModifiers)
	{
		static bool selfCall = false;

		if (!scene || selfCall) return;
		ItemHandle * handle = 0;

		selfCall = false;

		QList<QGraphicsItem*> nonTextItems;
		QList<ItemHandle*> visited;

		for (int i=0; i < items.size(); ++i)
		{
			if (qgraphicsitem_cast<TextGraphicsItem*>(items[i]))
			{
				QPointF p0 = items[i]->scenePos();
				qreal dist = 0;
				handle = getHandle(items[i]);
				if (handle)
				{
					visited += handle;
					for (int j=0; j < handle->graphicsItems.size(); ++j)
					{
						if (!qgraphicsitem_cast<TextGraphicsItem*>(handle->graphicsItems[j]))
						{
							QPointF p1;
							ConnectionGraphicsItem * connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(handle->graphicsItems[j]);
							if (connection && connection->centerPoint())
							{
								p1 = connection->centerLocation();
								if (dist == 0 || ((p1.rx()-p0.rx())*(p1.rx()-p0.rx()) + (p1.ry()-p0.ry())*(p1.ry()-p0.ry())) < dist)
								{
									dist = ((p1.rx()-p0.rx())*(p1.rx()-p0.rx()) + (p1.ry()-p0.ry())*(p1.ry()-p0.ry()));
									(qgraphicsitem_cast<TextGraphicsItem*>(items[i]))->relativePosition = QPair<QGraphicsItem*,QPointF>(connection,p0 - p1);
								}
							}
							else
							{
								NodeGraphicsItem * node = qgraphicsitem_cast<NodeGraphicsItem*>(handle->graphicsItems[j]);
								if (node)
								{
									for (int k=0; k < node->boundaryControlPoints.size(); ++k)
										if (node->boundaryControlPoints[k])
										{	
											p1 = node->boundaryControlPoints[k]->scenePos();
											if (dist == 0 || ((p1.rx()-p0.rx())*(p1.rx()-p0.rx()) + (p1.ry()-p0.ry())*(p1.ry()-p0.ry())) < dist)
											{
												dist = ((p1.rx()-p0.rx())*(p1.rx()-p0.rx()) + (p1.ry()-p0.ry())*(p1.ry()-p0.ry()));
												(qgraphicsitem_cast<TextGraphicsItem*>(items[i]))->relativePosition = QPair<QGraphicsItem*,QPointF>(node->boundaryControlPoints[k],p0 - p1);
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
				NodeGraphicsItem * node = qgraphicsitem_cast<NodeGraphicsItem*>(items[i]);
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
					&& ccp->connectionItem && ccp->connectionItem->itemHandle)
					handle = ccp->connectionItem->itemHandle;

				if ((pcp = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(nonTextItems[i]))
					&& pcp->nodeItem && pcp->nodeItem->itemHandle)
					handle = pcp->nodeItem->itemHandle;
			}

			if (handle && !visited.contains(handle))
				for (int j=0; j < handle->graphicsItems.size(); ++j)
				{
					if (qgraphicsitem_cast<TextGraphicsItem*>(handle->graphicsItems[j]) && !itemsToMove.contains(handle->graphicsItems[j]))
					{
						QPointF p;
						QPointF p0 = handle->graphicsItems[j]->scenePos();
						for (int k=0; k < handle->graphicsItems.size(); ++k)
							if (!qgraphicsitem_cast<TextGraphicsItem*>(handle->graphicsItems[k]) && k != j)
							{
								QPointF p1;
								QPair<QGraphicsItem*,QPointF> relativePoisition = (qgraphicsitem_cast<TextGraphicsItem*>(handle->graphicsItems[j]))->relativePosition;
								if (!relativePoisition.first) continue;

								if (qgraphicsitem_cast<ConnectionGraphicsItem*>(relativePoisition.first))
									p1 = (qgraphicsitem_cast<ConnectionGraphicsItem*>(relativePoisition.first))->centerLocation();
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
			scene->move(itemsToMove,moveBy);
		}
		selfCall = false;
	}

	void TextGraphicsTool::escapeSignal(const QWidget* )
	{
		if (inserting)
		{
			mainWindow->setCursor(Qt::ArrowCursor);
			if (mainWindow && mainWindow->currentScene())
				mainWindow->currentScene()->actionsEnabled = true;
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
			if (!name.isEmpty() && mainWindow->currentScene() && item->itemHandle && item->itemHandle->name == oldText)
			{	
				//mainWindow->currentScene()->rename(item,name);
				QList<QGraphicsItem*> items; items << item;
				QList<QString> oldNames; oldNames << oldText;
				QString newName = item->itemHandle->name = Tinkercell::RemoveDisallowedCharactersFromName(name);
				QString fullname = item->itemHandle->fullName();
				QList<QString> newNames; newNames << item->itemHandle->name;
				item->itemHandle->name = oldText;

				if (currentScene())
					currentScene()->rename(items,newNames);
			}
			else
			{
				if (name.isEmpty())
					command = new RemoveGraphicsCommand(tr("text removed"),mainWindow->currentScene(),item);
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
	}

	/*void TextGraphicsTool::handlesRenamed(NetworkWindow * , const QList<QGraphicsItem*>& items, const QList<QString>& oldnames, const QList<QString>& newnames)
	{
	QList<QGraphicsItem*> textItems;
	QList<QString> newNames;

	ItemHandle * itemHandle = 0;
	for (int i=0; i < items.size(); ++i)
	{
	itemHandle = getHandle(items[i]);

	if (itemHandle)
	for (int j=0; j < itemHandle->graphicsItems.size(); ++j)
	{
	TextGraphicsItem * textItem = qgraphicsitem_cast<TextGraphicsItem*>(itemHandle->graphicsItems[j]);
	if (textItem)
	{
	QString text1 = textItem->toPlainText(), text2 = oldnames.at(i);
	if (text1 == text2)
	{
	textItems += textItem;
	newNames += newnames.at(i);
	}
	}
	}
	}

	if (!textItems.isEmpty() && !newNames.isEmpty())
	{
	ChangeTextCommand * command = new ChangeTextCommand(tr("text changed"),textItems,newNames);
	if (mainWindow)		
	if (mainWindow->historyStack())
	mainWindow->historyStack()->push(command);
	else
	command->redo();
	}
	}*/

	void TextGraphicsTool::insertText()
	{
		if (mainWindow && mainWindow->currentScene())
		{
			mainWindow->sendEscapeSignal(this);
			mainWindow->setCursor(Qt::IBeamCursor);
			mainWindow->currentScene()->actionsEnabled = false;
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

	void TextGraphicsTool::itemsRemoved(GraphicsScene* scene,  QList<QGraphicsItem*>& items, QList<ItemHandle*>& handles)
	{
		if (!scene || items.isEmpty()) return;

		TextGraphicsItem * text;
		bool del;

		for (int i=0; i < handles.size(); ++i)
			if (handles[i])
				for (int j=0; j < handles[i]->graphicsItems.size(); ++j)
					if ((text = qgraphicsitem_cast<TextGraphicsItem*>(handles[i]->graphicsItems[j])) &&
						!items.contains(text))
					{
						del = true;
						for (int k=0; k < handles[i]->graphicsItems.size(); ++k)
							if (!qgraphicsitem_cast<TextGraphicsItem*>(handles[i]->graphicsItems[k]) &&
								!items.contains(handles[i]->graphicsItems[k]) &&
								handles[i]->graphicsItems[k]->sceneBoundingRect().adjusted(-100,-100,100,100).contains(text->scenePos()))
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
			targetItem->showBorder(false);
			targetItem->setSelected(false);
			targetItem->setTextInteractionFlags(Qt::NoTextInteraction);
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
			scene->actionsEnabled = true;
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
			TextGraphicsItem * textItem = qgraphicsitem_cast<TextGraphicsItem*>(items[0]);

			if (targetItem)
				clear();

			if (!textItem)
			{
				return;
			}

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
						if (scene->historyStack)
							scene->historyStack->push(handleChange);
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
			targetItem->setSelected(true);
			textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
			QTextCursor c = targetItem->textCursor();
			c.movePosition(QTextCursor::EndOfLine);
			c.movePosition(QTextCursor::StartOfLine,QTextCursor::KeepAnchor);
			targetItem->setTextCursor(c);
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
				TextGraphicsItem * textItem = qgraphicsitem_cast<TextGraphicsItem*>(scene->selected()[0]->topLevelItem());
				if (textItem)
				{
					oldText = textItem->toPlainText();
					targetItem = textItem;			
					targetItem->setSelected(true);
					textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
					QTextCursor c = targetItem->textCursor();
					c.movePosition(QTextCursor::EndOfLine);
					c.movePosition(QTextCursor::StartOfLine,QTextCursor::KeepAnchor);
					targetItem->setTextCursor(c);
				}
			}

			if (!targetItem) return;

			int key = keyEvent->key();
			if (key == Qt::Key_Enter || key == Qt::Key_Return || key == Qt::Key_Escape)
				clear();	
			keyEvent->accept();
		}
	}

	void TextGraphicsTool::mouseDoubleClicked (GraphicsScene * scene, QPointF , QGraphicsItem * item, Qt::MouseButton , Qt::KeyboardModifiers )
	{
		if (!item || !scene) return;
		TextGraphicsItem * textItem = qgraphicsitem_cast<TextGraphicsItem*>(item->topLevelItem());
		if (textItem)
		{
			oldText = textItem->toPlainText();
			targetItem = textItem;			
			targetItem->setSelected(true);
			textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
			QTextCursor c = targetItem->textCursor();
			c.movePosition(QTextCursor::EndOfLine);
			c.movePosition(QTextCursor::StartOfLine,QTextCursor::KeepAnchor);
			targetItem->setTextCursor(c);
		}
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
				if ((textItem = qgraphicsitem_cast<TextGraphicsItem*>(selected[i])))
				{
					texts += textItem->toPlainText();
					fonts += newFont;
					items += selected[i];
				}
			}
			QUndoCommand * command = new ChangeTextCommand(tr("font(s) changed"),items,texts,fonts);
			if (scene->historyStack)
				scene->historyStack->push(command);
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

		TextGraphicsItem * text = qgraphicsitem_cast<TextGraphicsItem*>(item);

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

			TextGraphicsItem * text = qgraphicsitem_cast<TextGraphicsItem*>(items[i]);

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

		TextGraphicsItem * text = qgraphicsitem_cast<TextGraphicsItem*>(item);

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

			TextGraphicsItem * text = qgraphicsitem_cast<TextGraphicsItem*>(items[i]);

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
				TextGraphicsItem * text = qgraphicsitem_cast<TextGraphicsItem*>(textItems[i]);

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
				TextGraphicsItem * text = qgraphicsitem_cast<TextGraphicsItem*>(textItems[i]);

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
