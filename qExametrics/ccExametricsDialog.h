#ifndef CC_EXAMETRICS_DIALOG_HEADER
#define CC_EXAMETRICS_DIALOG_HEADER

//Qt
#include <QDialog>
#include <QList>
#include <QAction>

//CC
#include <ccGLWindow.h>
#include <ccOverlayDialog.h>

//Local
#include <ui_exametricsDialog.h>

class ccExametricsDialog : public ccOverlayDialog, public Ui::exametricsDialog
{
	Q_OBJECT

public:
	//! Default constructor
	explicit ccExametricsDialog(QWidget* parent = 0);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    int m_nMouseClick_X_Coordinate;
    int m_nMouseClick_Y_Coordinate;
};

#endif
