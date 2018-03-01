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
#include <ccBox.h>
#include <ccClipBox.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>

#include "utils.h"

#define COEF_INIT 50
#define TOLERANCE_INIT 0.05


class ccExametricsDialog;


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

    // Cloud limits
	double m_boxXWidth = 0;
	double m_boxYWidth = 0;

	// Coefficient for vector point placement
	double m_coef = 0;

	// Vector point
	CCVector3d m_vectorPoint = CCVector3d(0,0,0);

	// DB Tree Exametrics group
	ccHObject* m_exametricsGroup = nullptr;


	/* Display purpose variables */
	bool canUpdateGraphics = false;

	// normalized vector cloud (display purpose)
	ccPointCloud* normalizedVectorCloud = nullptr;
	// normalized vector polyline
	ccPolyline* normalizedVectorPoly = nullptr;

	// vector point cloud (display purpose)
	ccPointCloud* vectorPointCloud = nullptr;
	// vector point 2dlabel
	cc2DLabel* vectorPoint2DLabel = nullptr;

	// plan cloud (display purpose)
	//ccPointCloud* planCloud = nullptr;
	// plan
	//ccPlane* pPlane = nullptr;

	ccClipBox* associatedBox= nullptr;
	ccBox* box = nullptr;

	// state
	bool planeIsInDBTree = false;


	/* Initialization methods */

	// spb limits and initial values
	void initializeParameterWidgets(ccHObject* lasFile);
	// draw vectors and plans basic settings
	void initializeDrawSettings();


	/* On events methods */

	void onNormalizedVectorChanged();
	void onVectorPointChanged(int coef);
	void onParameterChanged(QWidget* w, double value);


    /* Display methods */

    // Init vector display
    void initVector();
    // Update vector display
    void updateVector();
    // Init vector point display
    void initPoint();
    // Update vector point display
    void updatePoint();

    // Update plan display
	//void updatePlan();

	// Update box display
	void updateBox();


	/* Getters */

	double getTolerance();
    double getNormX();
	double getNormY();
	double getNormZ();
	CCVector3d getNormalizedVectorPointA();
	CCVector3d getNormalizedVectorPointB();
	CCVector3d getNormalizedVector();
	CCVector3d getVectorPoint();
	CCVector3d getVectorMediator();
	CCVector3d getVectorCenter();


	/* Other methods*/

	//bool pointIsOnVector(CCVector3 vectorPointA, CCVector3 vectorPointB, CCVector3 myPoint);
	void logInfo(QString s);
	void logWarn(QString s);
	void logError(QString s);


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
