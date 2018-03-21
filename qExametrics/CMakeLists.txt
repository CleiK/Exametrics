cmake_minimum_required(VERSION 3.0)

option( INSTALL_QEXAMETRICS_PLUGIN "Check to install qExametrics plugin" OFF )

# CloudCompare 'EXAMETRICS' plugin
if (INSTALL_QEXAMETRICS_PLUGIN)
    project( QEXAMETRICS_PLUGIN )
    
    #load necessary libraries (see qPCV for an example)
    #add_subdirectory (LIB1)

    #As we are going to re-use several components of the main qCC project
	#we define its sources path relatively to the plugin's dir
	set(CloudCompare_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../qCC)

	#include the whole CC directory (easier)
	include_directories( ${CloudCompare_SOURCE_DIR} )

	#we need ccComparison classes
	file( GLOB CC_PLUGIN_CUSTOM_HEADER_LIST ${CloudCompare_SOURCE_DIR}/ccComparisonDlg*.h )
	file( GLOB CC_PLUGIN_CUSTOM_SOURCE_LIST ${CloudCompare_SOURCE_DIR}/ccComparisonDlg*.cpp )

    list( APPEND  CC_PLUGIN_CUSTOM_HEADER_LIST ${CloudCompare_SOURCE_DIR}/ccOverlayDialog.h )
    list( APPEND  CC_PLUGIN_CUSTOM_SOURCE_LIST ${CloudCompare_SOURCE_DIR}/ccOverlayDialog.cpp )

    # customplot and cchistogramwindow used for ccComparisonDlg
    list( APPEND  ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/qcustomplot/qcustomplot.h )
    list( APPEND  ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/qcustomplot/qcustomplot.cpp )
    list( APPEND  CC_PLUGIN_CUSTOM_HEADER_LIST ${CloudCompare_SOURCE_DIR}/ccHistogramWindow.h )
    list( APPEND  CC_PLUGIN_CUSTOM_SOURCE_LIST ${CloudCompare_SOURCE_DIR}/ccHistogramWindow.cpp )
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/qcustomplot )

	set( CC_PLUGIN_CUSTOM_UI_LIST ${CMAKE_CURRENT_SOURCE_DIR}/exametricsDialog.ui )

    set( CC_PLUGIN_CUSTOM_UI_LIST ${CloudCompare_SOURCE_DIR}/ui_templates/comparisonDlg.ui )
    set( CC_PLUGIN_CUSTOM_UI_LIST ${CloudCompare_SOURCE_DIR}/ui_templates/histogramDlg.ui )

    
    #if the plugin is an 'OpenGL filter', uncomment the line below
    #set( CC_OPENGL_FILTER ON BOOL)
    include( ../CMakePluginTpl.cmake )
    
    #set dependencies to necessary libraries (see qPCV for an example)
    #target_link_libraries( ${PROJECT_NAME} LIB1 )
    #include_directories( ${LIB1_INCLUDE_DIR} )

    #add_custom_command(TARGET ../../../CloudCompare
    #   				   POST_BUILD 
    #				   COMMAND ${CMAKE_COMMAND} -E 
    #				   copy ${CMAKE_CURRENT_SOURCE_DIR}/scripts/exa.py
    #				   ${CMAKE_INSTALL_PREFIX}/lib/cloudcompare/plugins)

endif()
