//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qExametrics                 #
//#																									                       #
//##########################################################################

#ifndef Q_EXAMETRICS_PLUGIN_HEADER
#define Q_EXAMETRICS_PLUGIN_HEADER

//qCC
#include "../ccStdPluginInterface.h"
#include <QDoubleSpinBox>
#include <ccPointCloud.h>
#include <ccPolyline.h>
#include <cc2DLabel.h>
#include <ccPlane.h>

#include "utils.h"


//#include "ccExametricsDialog.h"
class ccExametricsDialog;

//! Dummy qCC plugin
/** Replace the 'qDummyPlugin' string by your own plugin class name
	and then check 'qDummyPlugin.cpp' for more directions (you
	have to fill-in the blank methods. The most important one is the
	'getActions' method.  This method should return all actions
	(QAction objects). CloudCompare will automatically add them to an
	icon in the plugin toolbar and to an entry in the plugin menu
	(if your plugin returns several actions, CC will create a dedicated
	toolbar and sub-menu).
	You are responsible to connect these actions to custom slots of your
	plugin.
	Look at the ccStdPluginInterface::m_app attribute to get access to
	most of CC components (database, 3D views, console, etc.).
**/
class ccExametrics : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
		Q_INTERFACES(ccStdPluginInterface)
		Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.qExametrics")

public:

	//! Default constructor
	explicit ccExametrics(QObject* parent = nullptr);

	//desconstructor
	~ccExametrics();

	//inherited from ccPluginInterface
	virtual QString getName() const override { return "Exametrics"; }
	virtual QString getDescription() const override { return "Exametrics plugin (plan intersection view)"; }
	virtual QIcon getIcon() const override;

	virtual void stop() override;

	//inherited from ccStdPluginInterface
	void onNewSelection(const ccHObject::Container& selectedEntities) override;
	virtual void getActions(QActionGroup& group) override;

private:

    /* Members */

	double m_boxXWidth = 0;
	double m_boxYWidth = 0;

	CCVector3d m_vectorPoint = CCVector3d(0,0,0);

	double m_coef = 0;


	/* Display purpose variables */

	// normalized vector cloud (display purpose)
	ccPointCloud* normalizedVectorCloud = nullptr;
	// normalized vector polyline
	ccPolyline* normalizedVectorPoly = nullptr;

	// vector point cloud (display purpose)
	ccPointCloud* vectorPointCloud = nullptr;
	// vector point 2dlabel
	cc2DLabel* vectorPoint2DLabel = nullptr;

	// plan cloud (display purpose)
	ccPointCloud* planCloud = nullptr;
	// plan
	ccPlane* pPlane = nullptr;
	// plan transformation
	//ccGLMatrix* planTransformation = nullptr;


	/* Initialization methods */

	// spb limits and initial values
	void initializeParameterWidgets(ccBBox box);
	// draw vectors and plans basic settings
	void initializeDrawSettings();


	/* On events methods */

	void onNormalizedVectorChanged();
	void onVectorPointChanged(int coef);
	void onParameterChanged(QWidget* w, double value);


	/* Getters */

    double getNormX();
	double getNormY();
	double getNormZ();
	CCVector3d getNormalizedVectorPointA();
	CCVector3d getNormalizedVectorPointB();
	CCVector3d getNormalizedVector();
	CCVector3d getVectorPoint();


	/* Other methods*/

	//bool pointIsOnVector(CCVector3 vectorPointA, CCVector3 vectorPointB, CCVector3 myPoint);


protected slots:

	void doAction();

	//**************
	//GUI actions:
	//**************
	//general
	void onCompute();
	void onClose();
	//plan parameters changed
	void onSpbXAChanged(double value);
	void onSpbYAChanged(double value);
	void onSpbZAChanged(double value);
	void onSpbXBChanged(double value);
	void onSpbYBChanged(double value);
	void onSpbZBChanged(double value);
	void onCoefSliderChanged(int value);
	void onToleranceSpbChanged(double value);


protected:

	//link to application windows
	//useless?
	QMainWindow* m_main_window = nullptr;

	//ccExametrics UI
	ccExametricsDialog* m_dlg = nullptr;

	//! Default action
	/** You can add as many actions as you want in a plugin.
		All actions will correspond to an icon in the dedicated
		toolbar and an entry in the plugin menu.
	**/
	QAction* m_action;
};

#endif
