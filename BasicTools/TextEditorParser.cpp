#include "TextEditorParser.h"
#include "NetworkWindow.h"
#include "TextEditor.h"
#include "OutputWindow.h"

namespace Tinkercell
{

    TextEditorParser::TextEditorParser(TextEditor * editor)
        : textEditor(editor)
    {
        nodesTree = 0;
        connectionsTree = 0;
        syntaxHighlighter = 0;
        MainWindow * mainWindow = 0;
        if (editor->networkWindow &&
            (mainWindow = editor->networkWindow->mainWindow))
        {
            this->nodesTree = static_cast<NodesTree*>(mainWindow->tool(tr("Nodes Tree")));
            this->connectionsTree = static_cast<ConnectionsTree*>(mainWindow->tool(tr("Connections Tree")));
        }

        if (editor && editor->document())
            syntaxHighlighter = new TextEditorSyntaxHighlighter(this,editor->document());
        regexDeclarations.setPattern(tr("(^\\w+)\\s+([ \\w+\\s*,\\s*]*\\w+)"));
        regexConnections.setPattern(tr("[\\s*\\w+\\s*\\+]*[\\s*\\w+\\s*]\\->"));
        regexConnectionName.setPattern(tr("^([\\w]+)\\s*\\:"));
        regexOp.setPattern(tr("\\w+\\s*=\\s*\\S"));

        connect(editor,SIGNAL(textChanged(const QString&,const QString&)),this,SLOT(textChanged(const QString&,const QString&)));
        connect(editor,SIGNAL(lineChanged(int,const QString&)), this,SLOT(lineChanged(int,const QString&)));
    }

    QList<TextItem*> TextEditorParser::parseText(const QString& s)
    {
        if (textEditor && !items.contains(s))
        {
            qDebug() << "new item";
            if (regexDeclarations.indexIn(s) > -1)
            {
                qDebug() << "declaration";
                if (!nodesTree ||
                    !nodesTree->nodeFamilies.contains(regexDeclarations.cap(1)))
                    return QList<TextItem*>();
                TextItem * item = 0;
                NodeFamily * family = nodesTree->nodeFamilies.value(regexDeclarations.cap(1));
                qDebug() << "declaration " << regexDeclarations.cap(1);
                QString str = regexDeclarations.cap(2).remove(QChar(' '));
                QStringList list = str.split(QChar(','));
                QList<TextItem*> newItems;
                for (int i=0; i < list.size(); ++i)
                {
                    ItemHandle * h = new NodeHandle(family);
                    h->name = list[i];
                    item = new TextItem(h);
                    newItems << item;
                }
                return newItems;
            }

            if (regexConnections.indexIn(s) > -1)
            {
                QString name = tr("");
                if (regexConnectionName.indexIn(s))
                    name = regexConnectionName.cap(1);

                qDebug() << "connection " << name;

                QString line = s;
                line.remove(QChar(' '));
                QStringList list = line.split(tr("->"));
                QStringList lhs = list[0].split(tr("+"));
                QStringList rhs = list[1].split(tr("+"));

                ConnectionTextItem * conn = new ConnectionTextItem;
                for (int i=0; i < lhs.size(); ++i)
                {
                    if (items.contains(lhs[i]) && items.value(lhs[i]) && items.value(lhs[i])->asNode())
                        conn->lhs += items.value(lhs[i])->asNode();
                }

                for (int i=0; i < rhs.size(); ++i)
                {
                    if (items.contains(rhs[i]) && items.value(rhs[i]) && items.value(rhs[i])->asNode())
                        conn->rhs += items.value(rhs[i])->asNode();
                }

                return QList<TextItem*>() << conn;
            }

            if (regexOp.indexIn(s) > -1)
            {
                QStringList lst = s.split(tr("="));
                if (lst.size() != 2) return QList<TextItem*>();

                OpTextItem * op = new OpTextItem;
                op->lhs = lst[0].trimmed();
                op->rhs = lst[1].trimmed();
                return QList<TextItem*>() << op;
            }
        }
        else
        {
            qDebug() << "old item";
        }

        return QList<TextItem*>();
    }

    void TextEditorParser::textChanged(const QString& from, const QString& to)
    {
        QList<TextItem*> newItems = parseText(to);
        if (newItems.isEmpty()) return;

        TextItem * oldItem = 0;
        TextItem * newItem = newItems[0];
        if (items.contains(from) && (oldItem = items.value(from)))
        {
            if (!newItem)
            {
                textEditor->removeItem(oldItem);
                return;
            }
            if (oldItem && newItem->type != oldItem->type)
            {
                textEditor->removeItem(oldItem);
                textEditor->insertItem(newItem);
                return;
            }
        }
        else
        {
            if (newItem)
                textEditor->insertItem(newItem);
        }
    }

    void TextEditorParser::lineChanged(int line, const QString& text)
    {
        if (textEditor)
        {
            QString info = tr("");
            TextItem * item = 0;
            if (items.contains(text) && (item = items.value(text)))
            {
                OpTextItem * op = item->asOp();
                if (op)
                {
                    info = tr("     ") + op->description;
                }
                else
                {
                    ConnectionTextItem * conn= item->asConnection();
                    if (conn && conn->itemHandle && conn->itemHandle->family())
                    {
                        info = tr("     ") + conn->itemHandle->family()->name;
                    }
                }
            }
            else
            {
                QRegExp regexp(tr("^(\\S+)\\s+.*"));
                if (regexp.indexIn(text) > -1 && nodesTree && nodesTree->nodeFamilies.contains(regexp.cap(1)))
                {
                    info = tr("     all variables of type ") + regexp.cap(1);
                }
            }
            textEditor->setStatusBarText(tr("line: ") + QString::number(line) + info);
        }
    }

    ConnectionFamily* TextEditorParser::findBestMatch(ConnectionTextItem * conn)
    {
        if (!connectionsTree) return 0;

        int numRequiredIn;
        int numRequiredOut;
        QString typeOut;
        QString typeIn;
        QList<QString> keys = connectionsTree->connectionFamilies.keys();

        ConnectionFamily * bestPick = 0, * selectedFamily = 0;

        for (int i=0; i < keys.size(); ++i)
        {
            if ((selectedFamily = connectionsTree->connectionFamilies[ keys[i] ])
                && selectedFamily->name.toLower() != tr("connection")
                && selectedFamily->numericalAttributes.contains(tr("numin"))
                && selectedFamily->numericalAttributes.contains(tr("numout"))
                && selectedFamily->numericalAttributes.contains(tr("typein"))
                && selectedFamily->numericalAttributes.contains(tr("typeout"))
                )
                {
                numRequiredIn = (int)selectedFamily->numericalAttributes.value("numin");
                numRequiredOut = (int)selectedFamily->numericalAttributes.value("numout");
                typeIn = selectedFamily->textAttributes[tr("typein")];
                typeOut = selectedFamily->textAttributes[tr("typeout")];
                if (numRequiredIn > 0 && conn->lhs.size() != numRequiredIn)
                {
                    selectedFamily = 0;
                    continue;
                }
                if (numRequiredOut > 0 && conn->rhs.size() != numRequiredOut)
                {
                    selectedFamily = 0;
                    continue;
                }
                for (int j=0; j < conn->lhs.size(); ++j)
                {
                    ItemHandle * handle = getHandle(conn->lhs[j]);
                    if (handle && !handle->isA(typeIn))
                    {
                        selectedFamily = 0;
                        break;
                    }
                }
                for (int j=0; j < conn->rhs.size(); ++j)
                {
                    ItemHandle * handle = getHandle(conn->rhs[j]);
                    if (handle && !handle->isA(typeOut))
                    {
                        selectedFamily = 0;
                        break;
                    }
                }
                if (selectedFamily != 0)
                {
                    if (bestPick == 0 || (selectedFamily->children().size() > bestPick->children().size()))
                        bestPick = selectedFamily;
                }
            }
        }

        return bestPick;
    }
}

