#include "ccExametricsDialog.h"

//Local
#include "ccGLWindow.h"

//qCC_db
#include <ccLog.h>

//Qt
#include <QEvent>
#include <QKeyEvent>
#include <QApplication>
#include <qmenu.h>
#include <qaction.h>

//system
#include <assert.h>

/* Default constructor */
ccExametricsDialog::ccExametricsDialog(QWidget* parent/*=0*/)
	: ccOverlayDialog(parent)
	, Ui::exametricsDialog()
{
	setupUi(this);

	//set background color
	QPalette p;
	p.setColor(backgroundRole(), QColor(240, 240, 240, 200));
	setPalette(p);
	setAutoFillBackground(true);
}

/* Retrieve mouse coordinates on click */
void ccExametricsDialog::mousePressEvent(QMouseEvent *event) 
{
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

/* Move the window on mouse move */
void ccExametricsDialog::mouseMoveEvent(QMouseEvent *event) 
{
    move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}

