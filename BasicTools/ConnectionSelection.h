/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This class performs some housekeeping operations for selecting and moving connection items.
 Connection items cannot be moved by themselves since their location is defined by the control
 points.
 
****************************************************************************/

#ifndef TINKERCELL_CONNECTIONSELECTIONTOOL_H
#define TINKERCELL_CONNECTIONSELECTIONTOOL_H

#include <stdlib.h>
#include <QtGui>
#include <QIcon>
#include <QSemaphore>
#include <QPixmap>
#include <QString>
#include <QtDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QAction>
#include <QFile>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QGraphicsRectItem>
#include <QTimeLine>

#include "Core/ConnectionGraphicsItem.h"
#include "Core/ItemHandle.h"
#include "Core/Tool.h"
#include "Core/MainWindow.h"

namespace Tinkercell
{

    class ConnectionSelection_FToS : public QObject
    {
        Q_OBJECT

    signals:
        void getControlPointX(QSemaphore*,qreal*,ItemHandle*,ItemHandle*,int);
        void getControlPointY(QSemaphore*,qreal*,ItemHandle*,ItemHandle*,int);
        void setControlPoint(QSemaphore*,ItemHandle*,ItemHandle*,int,qreal,qreal);
        void getCenterPointX(QSemaphore*,qreal*,ItemHandle*);
        void getCenterPointY(QSemaphore*,qreal*,ItemHandle*);
        void setCenterPoint(QSemaphore*,ItemHandle*,qreal,qreal);
        void setLineWidth(QSemaphore*,ItemHandle*,qreal,int);
        void setStraight(QSemaphore*,ItemHandle*,int);
        void setAllStraight(QSemaphore*,int);

    public slots:
        double getControlPointX(OBJ a0,OBJ a1,int a2);
        double getControlPointY(OBJ a0,OBJ a1,int a2);
        void setControlPoint(OBJ a0,OBJ a1,int i,double a2,double a3);
        void setCenterPoint(OBJ a0,double a1,double a2);
        double getCenterPointX(OBJ);
        double getCenterPointY(OBJ);
        void setStraight(OBJ,int);
        void setAllStraight(int);
        void setLineWidth(OBJ,double,int);
    };

    /*! \brief This class listens to selection events from the scene and responds by changing colors
of the selected objects and displaying tools and widgets that point to that object.
\ingroup plugins
*/
    class ConnectionSelection : public Tool
    {
        Q_OBJECT

    public:
        /*!\brief Constructor - does nothing
        */
        ConnectionSelection();
        /*!\brief set main window. connects all the event functions.
        \param the main window
        \return success or failure
        */
        bool setMainWindow(MainWindow * main);
    signals:
        /*! \brief signal sent to main window when a center box is inserted*/
        void itemsInserted(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<ItemHandle*>& handles);

    public slots:
        /*! \brief event that reponds to scene double-clicks. It an item is selected, then scene is centered on the item
        \param the scene that was clicked on
        \param point clicked on
        \param graphics item clicked on (can be null)
        \param the mouse button used for the click
        \param the keyboard modifiers used
        \return nothing
        */
        void sceneDoubleClicked (GraphicsScene * scene, QPointF point, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
        /*! \brief event that reponds to items selected. The selected items are highlighed and corresponding widgets are displayed.
        \param the scene that was clicked on
        \param the list of items selected
        \param point that was selected (not used)
        \param the mouse button used for the click
        \param the keyboard modifiers used
        \return nothing
        */
        void itemsSelected(GraphicsScene *scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers);
        /*! \brief event that reponds to key pressed. If excape, items are deselected
        \param the scene where key was pressed
        \param the keyboard modifiers used
        \return nothing
        */
        void sceneKeyPressed(GraphicsScene *scene, QKeyEvent*);

        void sceneKeyReleased(GraphicsScene *scene, QKeyEvent*);
        /*! \brief event that reponds to items moved.
        \param the scene that was clicked on
        \param the list of items underneath mouse
        \param point moved from
        \param point moved to
        \param modifier keys used
        \return nothing
        */
        void itemsMoved(GraphicsScene*,const QList<QGraphicsItem*>&, const QList<QPointF>&, Qt::KeyboardModifiers);

        void mouseMoved(GraphicsScene * scene, QGraphicsItem* item, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers, QList<QGraphicsItem*>&);

        void nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , const QList<QPointF>& , Qt::KeyboardModifiers );

        void connectionCollided(const QList<QGraphicsItem*>& , ConnectionGraphicsItem * , QPointF , Qt::KeyboardModifiers );

        void itemsRemoved(GraphicsScene * , QList<QGraphicsItem*>& , QList<ItemHandle*>& );

        void toolLoaded(Tool*);

        void setupFunctionPointers( QLibrary * );

        void setLineType(int);
        void showMiddleBox(int, const QString& filename = QString());
        void showMiddleBox();
        void hideMiddleBox();
        void arrowHeadDistance(double value);
        void newControlPoint();
        void setLineTypeStraight();
        void setLineTypeCurved();

    private slots:
        void getControlPointX(QSemaphore*,qreal*,ItemHandle*,ItemHandle*,int);
        void getControlPointY(QSemaphore*,qreal*,ItemHandle*,ItemHandle*,int);
        void setControlPoint(QSemaphore*,ItemHandle*,ItemHandle*,int,qreal,qreal);
        void setCenterPoint(QSemaphore*,ItemHandle*,qreal,qreal);
        void getCenterPointX(QSemaphore*,qreal*, ItemHandle*);
        void getCenterPointY(QSemaphore*,qreal*, ItemHandle*);
        void setStraight(QSemaphore*,ItemHandle*,int);
        void setAllStraight(QSemaphore*,int);
        void setLineWidth(QSemaphore* sem,ItemHandle* h,qreal value,int permanent);
        void escapeSignal(const QWidget*);
    protected:
        /*!\brief change control points of moved connection items (may not be needed for most connectors)*/
        void adjustConnectorPoints(const QList<QGraphicsItem*>& movingItems);
        void connectCollisionDetector();
        bool controlHeld;
        qreal gridDist;
        QAction * separator;
        QAction addControlPointAction;
        QAction useStraightLinesAction;
        QAction useCurvesAction;
        QAction showMiddleRect;
        QAction hideMiddleRect;
        //QMenu connectorsMenu;
        void connectTCFunctions();
        static ConnectionSelection_FToS fToS;
        static double _getControlPointX(OBJ ,OBJ ,int );
        static double _getControlPointY(OBJ ,OBJ ,int );
        static void _setControlPoint(OBJ ,OBJ ,int i, double ,double );
        static double _getCenterPointX(OBJ);
        static double _getCenterPointY(OBJ);
        static void _setCenterPoint(OBJ,double,double);
        static void _setStraight(OBJ,int);
        static void _setAllStraight(int);
        static void _setLineWidth(OBJ,double,int);

        class LineTypeChanged : public QUndoCommand
        {
        public:
            QList<ConnectionGraphicsItem*> list;
            bool straight;
            void undo();
            void redo();
        };
        /*
        class ShowHideMiddleRegion : public QUndoCommand
        {
        public:
                QList<ConnectionGraphicsItem*> list;
                bool show;
                QString filename;
                void undo();
                void redo();
        };
        */
        class ChangeArrowHeadDistance : public QUndoCommand
        {
        public:
            QList<ConnectionGraphicsItem*> list;
            QList<qreal> dists;
            void undo();
            void redo();
        };

    private:
        QList<ConnectionGraphicsItem*> temporarilyChangedConnections;

    };

}

#endif
