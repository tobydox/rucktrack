CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

PROJECT(RuckTrack)

IF(COMMAND CMAKE_POLICY)
	CMAKE_POLICY(SET CMP0005 NEW)
	CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "1")
SET(VERSION_PATCH "0")
#SET(VERSION_SUFFIX "")

SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
IF(VERSION_SUFFIX)
	SET(VERSION "${VERSION}-${VERSION_SUFFIX}")
ENDIF(VERSION_SUFFIX)

# check for Qt4
SET(QT_MIN_VERSION "4.4.0")
FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_QTGUI_INCLUDE_DIR ${QT_INCLUDE_DIR}/QtGui)
SET(QT_QTNETWORK_INCLUDE_DIR ${QT_INCLUDE_DIR}/QtNetwork)
SET(QT_QTWEBKIT_INCLUDE_DIR ${QT_INCLUDE_DIR}/QtWebKit)
SET(QT_QTXML_INCLUDE_DIR ${QT_INCLUDE_DIR}/QtXml)
SET(QT_USE_QTNETWORK 1)
SET(QT_USE_QTWEBKIT 1)
SET(QT_USE_QTXML 1)
INCLUDE(${QT_USE_FILE})

# check for GDAL
IF(NOT NO_GDAL_SUPPORT)
	FIND_PACKAGE(GDAL REQUIRED)
	SET(HAVE_GDAL_SUPPORT 1)
	ADD_DEFINITIONS(-DHAVE_GDAL_SUPPORT)
ENDIF(NOT NO_GDAL_SUPPORT)

# if Doxygen is present, add a custom target to build the documentation
FIND_PACKAGE(Doxygen)
IF (DOXYGEN_FOUND STREQUAL "YES")
	ADD_CUSTOM_TARGET(doxygen 
		DEPENDS ${rucktrack_SOURCES} ${rucktrack_INCLUDES}
		COMMAND cd ${CMAKE_SOURCE_DIR}/src \; echo -e \"PROJECT_NUMBER=${VERSION}\" | cat Doxyfile - | ${DOXYGEN_EXECUTABLE} - 
		)
ENDIF()

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/rucktrack.rc.in ${CMAKE_BINARY_DIR}/rucktrack.rc)

# set compiler flags
SET(CMAKE_C_FLAGS "-g -Wall ${CMAKE_C_FLAGS}")
SET(CMAKE_CXX_FLAGS "-g -fno-exceptions -Wall ${CMAKE_CXX_FLAGS}")

SET(rucktrack_SOURCES src/main.cpp
			src/RTMainWindow.cpp
			src/PreferencesDialog.cpp
			src/GoogleMapsProvider.cpp
			src/OpenStreetMapProvider.cpp
			src/GpxFile.cpp
			src/MapProvider.cpp
			src/MapView.cpp
			src/PlotView.cpp
			src/RouteTableModel.cpp
			src/RuckTrackNetworkAccessManager.cpp
			src/StatisticsTable.cpp
			src/SrtmLayer.cpp
			src/SrtmTiff.cpp
			src/QtBzip2File.cpp
			src/3rdparty/quazip/quazip/zip.c
			src/3rdparty/quazip/quazip/unzip.c
			src/3rdparty/quazip/quazip/quazipnewinfo.cpp
			src/3rdparty/quazip/quazip/quazipfile.cpp
			src/3rdparty/quazip/quazip/quazip.cpp
			src/3rdparty/quazip/quazip/ioapi.c
			src/3rdparty/bzip2/bzlib.c
			src/3rdparty/bzip2/crctable.c
			src/3rdparty/bzip2/compress.c
			src/3rdparty/bzip2/huffman.c
			src/3rdparty/bzip2/randtable.c
			src/3rdparty/bzip2/decompress.c
			src/3rdparty/bzip2/blocksort.c
			src/3rdparty/qwt/qwt-all.cpp)

SET(rucktrack_INCLUDES src/RTMainWindow.h
			src/AboutDialog.h
			src/PreferencesDialog.h
			src/GoogleMapsProvider.h
			src/OpenStreetMapProvider.h
			src/GpxFile.h
			src/MapProvider.h
			src/MapView.h
			src/PlotView.h
			src/RuckTrackNetworkAccessManager.h
			src/RouteTableModel.h
			src/TrackPoint.h
			src/SrtmLayer.h
			src/StatisticsTable.h
			src/QtBzip2File.h
			src/3rdparty/quazip/quazip/quazipfile.h
			src/3rdparty/qwt/qwt_dyngrid_layout.h
			src/3rdparty/qwt/qwt_legend.h
			src/3rdparty/qwt/qwt_legend_item.h
			src/3rdparty/qwt/qwt_plot_canvas.h
			src/3rdparty/qwt/qwt_plot.h
			src/3rdparty/qwt/qwt_text_label.h
			src/3rdparty/qwt/qwt_scale_widget.h)

IF(WIN32)
	INSTALL(FILES ${QT_BINARY_DIR}/QtCore4.dll ${QT_BINARY_DIR}/QtGui4.dll ${QT_BINARY_DIR}/QtXml4.dll ${QT_BINARY_DIR}/QtNetwork4.dll ${QT_BINARY_DIR}/QtWebKit4.dll ${QT_BINARY_DIR}/phonon4.dll ${QT_BINARY_DIR}/libgdal-1.dll ${QT_BINARY_DIR}/libz.dll DESTINATION .)
	SET(WINRC "${CMAKE_BINARY_DIR}/rucktrackrc.obj")
	ADD_CUSTOM_COMMAND(OUTPUT ${WINRC}
			COMMAND ${WINDRES}
				-I${CMAKE_SOURCE_DIR}
				-o${CMAKE_BINARY_DIR}/rucktrackrc.obj
				-i${CMAKE_BINARY_DIR}/rucktrack.rc
			DEPENDS ${CMAKE_BINARY_DIR}/rucktrack.rc)
ENDIF(WIN32)

SET(rucktrack_UI
					forms/rtmainwindow.ui
					forms/AboutDialog.ui
					forms/PreferencesDialog.ui
					)

QT4_WRAP_CPP(rucktrack_MOC_out ${rucktrack_INCLUDES})
QT4_WRAP_UI(rucktrack_UIC_out ${rucktrack_UI})
QT4_ADD_RESOURCES(rucktrack_RCC_out resources/rucktrack.qrc)

INCLUDE_DIRECTORIES(${GDAL_INCLUDE_DIR} ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/3rdparty/quazip/quazip ${CMAKE_SOURCE_DIR}/src/3rdparty/bzip2 ${CMAKE_SOURCE_DIR}/src/3rdparty/qwt ${CMAKE_BINARY_DIR})
LINK_LIBRARIES(${QT_LIBRARIES} ${GDAL_LIBRARY} -lz)
ADD_EXECUTABLE(rucktrack ${rucktrack_SOURCES} ${rucktrack_UIC_out} ${rucktrack_MOC_out} ${rucktrack_RCC_out} ${WINRC})

IF(WIN32)
	SET_TARGET_PROPERTIES(rucktrack PROPERTIES LINK_FLAGS "${LINK_FLAGS} -mwindows")
	ADD_CUSTOM_COMMAND(TARGET rucktrack POST_BUILD COMMAND ${STRIP} ${CMAKE_BINARY_DIR}/rucktrack.exe)
	INSTALL(TARGETS rucktrack RUNTIME DESTINATION .)
ELSE(WIN32)
	INSTALL(TARGETS rucktrack RUNTIME DESTINATION bin)
ENDIF(WIN32)

#
# add dist target
#
SET(TMP "rucktrack-${VERSION}")
ADD_CUSTOM_TARGET(dist
						COMMAND make clean
						COMMAND rm -rf ${TMP}
						COMMAND mkdir -p ${TMP}
						COMMAND cp AUTHORS build_mingw32 CMakeLists.txt rucktrack.pro COPYING INSTALL rucktrack.rc.in README TODO ${TMP}
						COMMAND cp -r 3rdparty cmake resources forms src ${TMP}
						COMMAND rm -rf `find ${TMP} -name cmake_install.cmake` `find ${TMP} -name Makefile` `find ${TMP} -type d -name CMakeFiles` ${TMP}/CMakeCache.txt
						COMMAND tar cjf rucktrack-${VERSION}.tar.bz2 ${TMP}
						COMMAND rm -rf ${TMP})


INCLUDE(InstallRequiredSystemLibraries)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "RuckTrack")
SET(CPACK_PACKAGE_VENDOR "Tobias Doerffel")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")

SET(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
IF(VERSION_SUFFIX)
	SET(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}-${VERSION_SUFFIX}")
ENDIF(VERSION_SUFFIX)
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "RuckTrack ${VERSION}")
IF(WIN32)
SET(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/resources\\\\rucktrack.ico")
SET(CPACK_NSIS_INSTALLED_ICON_NAME "rucktrack.exe")
SET(CPACK_NSIS_DISPLAY_NAME "RuckTrack")
SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\rucktrack.sourceforge.net")
SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\rucktrack.sourceforge.net")
SET(CPACK_NSIS_CONTACT "tobias.doerffel@gmail.com")
SET(CPACK_PACKAGE_EXECUTABLES "rucktrack.exe;RuckTrack")
SET(CPACK_NSIS_MENU_LINKS "rucktrack.exe;RuckTrack")
ELSE(WIN32)
SET(CPACK_STRIP_FILES "bin/rucktrack")
SET(CPACK_PACKAGE_EXECUTABLES "rucktrack" "RuckTrack binary")
ENDIF(WIN32)

SET(MACOSX_BUNDLE_ICON_FILE "${CMAKE_SOURCE_DIR}/resources/logo.png")
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "RuckTrack")
SET(MACOSX_BUNDLE_LONG_VERSION_STRING "${VERSION}")
SET(MACOSX_BUNDLE_BUNDLE_NAME "RuckTrack")
SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "${VERSION}")
SET(MACOSX_BUNDLE_BUNDLE_VERSION "${VERSION}")
SET(MACOSX_BUNDLE_COPYRIGHT "Tobias Doerffel, 2009")

SET(CPACK_SOURCE_GENERATOR "TBZ2")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "rucktrack-${VERSION}")
INCLUDE(CPack)

