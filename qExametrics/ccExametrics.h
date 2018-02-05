//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qExametrics                 #
//#																									                       #
//##########################################################################

#ifndef Q_EXAMETRICS_PLUGIN_HEADER
#define Q_EXAMETRICS_PLUGIN_HEADER

//qCC
#include "../ccStdPluginInterface.h"


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

protected slots:

	void doAction();

	//**************
	//GUI actions:
	//**************
	//general
	void onCompute();
	void onClose();

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
