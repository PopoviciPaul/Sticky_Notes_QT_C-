include("/media/paul/D/QT_Projects/New_Series/Sticky_Notes/build/Desktop_Qt_6_9_1-Debug/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/Sticky_Notes-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE /media/paul/D/QT_Projects/New_Series/Sticky_Notes/build/Desktop_Qt_6_9_1-Debug/Sticky_Notes
    GENERATE_QT_CONF
)
