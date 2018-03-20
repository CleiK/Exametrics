//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qExametrics                 #
//#																									                       #
//##########################################################################

#ifndef Q_EXAMETRICS_PLUGIN_HEADER
#define Q_EXAMETRICS_PLUGIN_HEADER

//qCC
#include "../ccStdPluginInterface.h"

// Qt
#include <QThread>
#include <QMovie>
#include <QList>
#include <QProcess>
#include <QDoubleSpinBox>

// CloudCompare
#include <ccPointCloud.h>
#include <ccGenericPointCloud.h>
#include <ccPolyline.h>
#include <cc2DLabel.h>
#include <ccPlane.h>
#include <ccBox.h>
#include <ccClipBox.h>
#include <DgmOctree.h>
#include <ccOctree.h>
#include <ccOctreeProxy.h>

// Standard lib
#include <unistd.h>
#include <iostream>
#include <iomanip>

// Exametrics
#include "utils.h"
#include "ccExaLog.h"
#include "ccExaWorker.h"

/* Init DEFINES */

// Percentage distance of vector point from point A
#define COEF_INIT 50
// Box tolerance
#define TOLERANCE_INIT 1


/* Debug DEFINES */

// Plan is used to position box
#define DEBUG_PLAN false
// Box points
#define DEBUG_BOX_POINTS false

// Enable use of "ccOctree::Shared" in connect
Q_DECLARE_METATYPE(ccOctree::Shared);


class ccExametricsDialog;

/* Main class of the ccExametrics plugin used to compute 
   the intersection of a plan with tolerance and a cloud */
class ccExametrics : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
		Q_INTERFACES(ccStdPluginInterface)
		Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.qExametrics")


public:

	// Default constructor
	explicit ccExametrics(QObject* parent = nullptr);

	// Deconstructor
	~ccExametrics();

	//inherited from ccPluginInterface
	virtual QString getName() const override { return "Exametrics"; }
	virtual QString getDescription() const override { return "Exametrics plugin (plan intersection view)"; }
	/* Return the plugin icon */
	virtual QIcon getIcon() const override;
	/* Called when the plugin is being stopped */
	virtual void stop() override;

	//inherited from ccStdPluginInterface
	void onNewSelection(const ccHObject::Container& selectedEntities) override;
	virtual void getActions(QActionGroup& group) override;

	ExaLog* logger = nullptr;

private:

    /* Members */

    ccHObject* rootLasFile = nullptr;

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
	ccPointCloud* planCloud = nullptr;

	//new cloud to test the cell center
	ccPointCloud* newCloud = nullptr;
    cc2DLabel* centrePoint2DLabel = nullptr;


	// Plan
	ccPlane* pPlane = nullptr;
	// Box
	ccBox* box = nullptr;

	// Temporary ccPointCloud list
    QList<ccPointCloud*>* tmpPointCloudList = nullptr;
    // Temporary cc2DLabel list
    QList<cc2DLabel*>* tmpPointList = nullptr;
    const int N_point = 8;

	// state
	bool planeIsInDBTree = false;

	// Loading Gif
	QMovie* loadingGifMovie = nullptr;

	// Thread use to do some work in parallel
	QThread workerThread;
	// Worker class
	ExaWorker* exaWorker = nullptr;

	// Shared octree bind to our cloud file
	ccOctree::Shared octree = nullptr;
	// Octree proxy used to display the octree
    ccOctreeProxy* octreeProxy = nullptr;


	/* Initialization methods */

	/* Initialize plan parameters at random values with min and max limits */
	void initializeParameterWidgets();
	/* Initialize draw settings for normalized vector, point and plan display */
	void initializeDrawSettings();


	/* On events methods */

	/* Called when the parameters of the normalized vector are changing */
	void onNormalizedVectorChanged();
	/* Called when the parameter of the vector point is changing */
	void onVectorPointChanged(int coef);
	/* Called when a parameters of the plan is changing */
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
	void updatePlan();
	// Update box display
	void updateBox();


	/* Getters */

	// Get box tolerance
	double getTolerance();
	/* Return the norm on X axis for the normalized vector */
    double getNormX();
    /* Return the norm on Y axis for the normalized vector */
	double getNormY();
	/* Return the norm on Z axis for the normalized vector */
	double getNormZ();
	/* Return point A coordinates of the normalized vector */
	CCVector3d getNormalizedVectorPointA();
	/* Return point B coordinates of the normalized vector */
	CCVector3d getNormalizedVectorPointB();
	/* Return the normalized vector */
	CCVector3d getNormalizedVector();
	/* Return the point on the vector (compute from coef distance) */
	CCVector3d getVectorPoint();
	// Get the vector center coordinates
	CCVector3d getVectorCenter();
	// Get the vector bisector
	CCVector3d getVectorMediator();



	/* Other methods*/

	// Enable or disable the loading gif
	void setGifLoading(bool enabled);

signals:
	// Use to tell the worker to do python work
    void operatePythonWorker(QStringList, ExaLog*);
    // Use to tell the worker to do octree work
    void operateOctreeWorker(ccOctree::Shared octree, double tolerance, ExaLog*);


protected slots:

	void doAction();

	//**************
	//GUI actions:
	//**************

	//general

	/* Slot on Compute button click */
	void onCompute();

	/* Slot on dialog closed */
	void onClose();

	//plan parameters changed
	/* Called when xA spinbox changed value */
	void onSpbXAChanged(double value);
	/* Called when yA spinbox changed value */
	void onSpbYAChanged(double value);
	/* Called when zA spinbox changed value */
	void onSpbZAChanged(double value);
	/* Called when xB spinbox changed value */
	void onSpbXBChanged(double value);
	/* Called when yB spinbox changed value */
	void onSpbYBChanged(double value);
	/* Called when zB spinbox changed value */
	void onSpbZBChanged(double value);
	/* Called when distance coefficient slider changed value */
	void onCoefSliderChanged(int value);
	/* Called when distance coefficient spinbox changed value */
	void onCoefSpinBoxChanged(int value);
	/* Called when tolerance spinbox changed value */
	void onToleranceSpbChanged(double value);

	// Called when the worker is done
	void workerDone(const QString s);
	// Called when the worker has computed the desired octree level
	void octreeLevelReady(const unsigned int level);


protected:

	//link to application windows
	//useless?
	QMainWindow* m_main_window = nullptr;

	//ccExametrics UI
	ccExametricsDialog* m_dlg = nullptr;

	//icon action
	QAction* m_action;


};

#endif
