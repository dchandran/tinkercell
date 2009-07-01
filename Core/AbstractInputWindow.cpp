/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This file defines an abstract class that is used to create interfaces for C functions.
 LPSolveInputWindow is a good example.

****************************************************************************/
#include "AbstractInputWindow.h"
#include "CThread.h"

namespace Tinkercell
{

     /*! \brief constructor
     * \param QString name of this tool, e.g. "Flux Balance Analysis Input Window"
     * \param QString name of the C library file (WITHOUT the suffix .dll, .so, .dylib), e.g. "dlls/lpsolve"
     * \param QString name of the function to call inside the C library file, e.g. "run"
     */
     AbstractInputWindow::AbstractInputWindow(const QString& name, const QString& fileName, const QString& functionName) :
               Tool(name),
               fileName(fileName),
               functionName(functionName)
     {
          dockWidget = 0;
     }

     /*! \brief Sets the main window. This function will set this tool as a docked widget by default and registed the escapeSignal event.
          Overwrite this function to prevent that default behavior.
     */
     bool AbstractInputWindow::setMainWindow(MainWindow * main)
     {
          Tool::setMainWindow(main);
          if (mainWindow)
          {
               connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),this,SLOT(escapeSignal(const QWidget*)));
               dockWidget = mainWindow->addDockingWindow(name,this,Qt::RightDockWidgetArea,Qt::BottomDockWidgetArea,false);
               dockWidget->setAttribute(Qt::WA_ContentsPropagated);
               dockWidget->setFloating(true);
               dockWidget->setWindowOpacity(0.8);
          }
          return true;
     }

     /*!
          \brief Escape signal is a request to stop the current process. This class will hide itself as a response.
     */
     void AbstractInputWindow::escapeSignal(const QWidget*)
     {
          if (dockWidget)
               dockWidget->hide();
          else
               this->hide();
     }

     /*!
          \brief Executes the library file as a new thread (LibraryThread)
          \sa LibraryThread
     */
     void AbstractInputWindow::exec()
     {
          Matrix m = ConvertValue(dataTable);
          LibraryThread * newThread = new LibraryThread(fileName,functionName,mainWindow,m);
          LibraryThread::ThreadDialog(mainWindow,newThread,fileName + tr(": ") + functionName);
          newThread->start(); //go
     }

     static double d = 1.0;
     static double* AddVariable(const char*, void*)
     {
          return &d;
     }

     bool AbstractInputWindow::parseMath(NetworkWindow * scene,QString& s)
     {
          if (!scene) return false;

          static QStringList reservedWords;
          if (reservedWords.isEmpty())
               reservedWords << "time";

          mu::Parser parser;

          s.replace(QRegExp(tr("\\.(?!\\d)")),tr("_qqq_"));
          parser.SetExpr(s.toAscii().data());
          s.replace(tr("_qqq_"),tr("."));
          parser.SetVarFactory(AddVariable, 0);
          QString str;

          try
          {
               parser.Eval();

               // Get the map with the variables
               mu::varmap_type variables = parser.GetVar();

               // Get the number of variables
               mu::varmap_type::const_iterator item = variables.begin();

               // Query the variables
               for (; item!=variables.end(); ++item)
               {
                    SymbolsTable & symbolsTable = scene->symbolsTable;
                    str = tr(item->first.data());
                    str.replace(QRegExp(tr("[^A-Za-z0-9_]")),tr(""));
                    str.replace(tr("_qqq_"),tr("."));
                    QString str2 = str;
                    str2.replace(tr("_"),tr("."));
                    if (!reservedWords.contains(str) &&
                        !symbolsTable.handlesFullName.contains(str)) //maybe new symbol in the formula
                    {
                         if (symbolsTable.dataRowsAndCols.contains(str) && symbolsTable.dataRowsAndCols[str].first)
                         {
                              if (! str.contains(QRegExp(tr("^")+symbolsTable.dataRowsAndCols[str].first->fullName())) )
                              {
                                   ItemHandle * handle = symbolsTable.dataRowsAndCols[str].first;
                                   s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),handle->fullName() + tr(".") + str + tr("\\1"));
                                   s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + handle->fullName() + tr(".") + str + tr("\\2"));
                                   s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("$")),tr("\\1") + handle->fullName() + tr(".")  + str);
                              }
                         }
                         else
                              if (symbolsTable.dataRowsAndCols.contains(str2) && symbolsTable.dataRowsAndCols[str2].first)
                              {
                              if (! str2.contains(QRegExp(tr("^")+symbolsTable.dataRowsAndCols[str2].first->fullName())) )
                              {
                                   ItemHandle * handle = symbolsTable.dataRowsAndCols[str2].first;
                                   s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),handle->fullName() + tr(".") + str2 + tr("\\1"));
                                   s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + handle->fullName() + tr(".") + str2 + tr("\\2"));
                                   s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("$")),tr("\\1") + handle->fullName() + tr(".")  + str2);
                              }
                              else
                              {
                                   s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),str2 + tr("\\1"));
                                   s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + str + tr("\\2"));
                                   s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("$")),tr("\\1") + str);
                              }
                         }
                         else
                         {
                              if (symbolsTable.handlesFirstName.contains(str) && symbolsTable.handlesFirstName[str])
                              {
                                   s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),symbolsTable.handlesFirstName[str]->fullName() + tr("\\1"));
                                   s.replace(QRegExp(tr("([^a-zA-Z0-9_])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + symbolsTable.handlesFirstName[str]->fullName() + tr("\\2"));
                                   s.replace(QRegExp(tr("([^a-zA-Z0-9_])")+str+tr("$")),tr("\\1") + symbolsTable.handlesFirstName[str]->fullName());
                              }
                              else
                                   if (symbolsTable.handlesFirstName.contains(str2) && symbolsTable.handlesFirstName[str2])
                                   {
                                   s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),symbolsTable.handlesFirstName[str2]->fullName() + tr("\\1"));
                                   s.replace(QRegExp(tr("([^a-zA-Z0-9_])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + symbolsTable.handlesFirstName[str2]->fullName() + tr("\\2"));
                                   s.replace(QRegExp(tr("([^a-zA-Z0-9_])")+str+tr("$")),tr("\\1") + symbolsTable.handlesFirstName[str2]->fullName());
                              }
                              else
                              {
                                   //qDebug() << str << "not in symbol table";
                                   OutputWindow::error(str + tr(" is not defined"));
                                   return false;
                              }
                         }
                    }
               }
          }
          catch(mu::Parser::exception_type &e)
          {
               OutputWindow::error(tr(e.GetMsg().data()));
               return false;
          }
          return true;
     }

     void AbstractInputWindow::enterEvent ( QEvent * event )
     {
          //if (dockWidget)
          //dockWidget->setWindowOpacity(1.0);
     }

     void AbstractInputWindow::leaveEvent ( QEvent * event )
     {
          //if (dockWidget)
          //dockWidget->setWindowOpacity(0.7);
     }
};


