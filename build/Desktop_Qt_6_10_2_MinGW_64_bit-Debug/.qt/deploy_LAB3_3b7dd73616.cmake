include("C:/UsersInterfaceLabs/LAB3/build/Desktop_Qt_6_10_2_MinGW_64_bit-Debug/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/LAB3-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE "C:/UsersInterfaceLabs/LAB3/build/Desktop_Qt_6_10_2_MinGW_64_bit-Debug/LAB3.exe"
    GENERATE_QT_CONF
)
