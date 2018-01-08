QT +=     network \
    xml
#webkit \
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 4): greaterThan(QT_MINOR_VERSION, 8): QT += webenginewidgets

#CONFIG += release

TARGET = rucktrack
TEMPLATE = app
SOURCES += src/main.cpp \
    src/RTMainWindow.cpp \
	src/PreferencesDialog.cpp		\
	src/GoogleMapsProvider.cpp		\
	src/OpenStreetMapProvider.cpp	\
	src/WmsMapProvider.cpp			\
    src/GpxFile.cpp \
	src/MapProvider.cpp				\
    src/MapView.cpp \
    src/PlotView.cpp \
    src/RouteTableModel.cpp \
	src/RuckTrackNetworkAccessManager.cpp	\
    src/Segmentiser.cpp \
    src/SegmentiserThread.cpp \
    src/StatisticsTable.cpp \
    src/SrtmLayer.cpp \
    src/SrtmTiff.cpp \
    src/QtBzip2File.cpp \
    src/3rdparty/quazip/quazip/zip.c \
    src/3rdparty/quazip/quazip/unzip.c \
    src/3rdparty/quazip/quazip/quazipnewinfo.cpp \
    src/3rdparty/quazip/quazip/quazipfile.cpp \
    src/3rdparty/quazip/quazip/quazip.cpp \
    src/3rdparty/quazip/quazip/ioapi.c \
    src/3rdparty/bzip2/bzlib.c \
    src/3rdparty/bzip2/crctable.c \
    src/3rdparty/bzip2/compress.c \
    src/3rdparty/bzip2/huffman.c \
    src/3rdparty/bzip2/randtable.c \
    src/3rdparty/bzip2/decompress.c \
    src/3rdparty/bzip2/blocksort.c \
#    src/3rdparty/qwt/qwt_dyngrid_layout.cpp \
#    src/3rdparty/qwt/qwt_legend.cpp \
##    src/3rdparty/qwt/qwt_legend_item.cpp \
#    src/3rdparty/qwt/qwt_plot_item.cpp \
#    src/3rdparty/qwt/qwt_plot_canvas.cpp \
#    src/3rdparty/qwt/qwt_plot_curve.cpp \
#    src/3rdparty/qwt/qwt_plot.cpp \
#    src/3rdparty/qwt/qwt_text_label.cpp \
#    src/3rdparty/qwt/qwt_scale_widget.cpp
##	src/3rdparty/qwt/qwt-all.cpp

HEADERS += src/RTMainWindow.h \
    src/AboutDialog.h \
	src/PreferencesDialog.h		\
	src/GoogleMapsProvider.h	\
	src/OpenStreetMapProvider.h	\
	src/WmsMapProvider.h		\
    src/GpxFile.h \
    src/MapProvider.h \
    src/MapView.h \
    src/PlotView.h \
    src/RuckTrackNetworkAccessManager.h \
    src/RouteTableModel.h \
    src/TrackPoint.h \
    src/Segmentiser.h \
    src/SegmentiserThread.h \
    src/SrtmLayer.h \
    src/StatisticsTable.h \
    src/QtBzip2File.h \
    src/3rdparty/quazip/quazip/quazipfile.h
#    src/3rdparty/qwt/qwt_dyngrid_layout.h \
#    src/3rdparty/qwt/qwt_legend.h \
##    src/3rdparty/qwt/qwt_legend_item.h \
#    src/3rdparty/qwt/qwt_plot_item.h \
#    src/3rdparty/qwt/qwt_plot_canvas.h \
#    src/3rdparty/qwt/qwt_plot_curve.h \
#    src/3rdparty/qwt/qwt_plot.h \
#    src/3rdparty/qwt/qwt_text_label.h \
#    src/3rdparty/qwt/qwt_scale_widget.h

INCLUDEPATH += src \
    src/3rdparty/quazip/quazip \
	src/3rdparty/bzip2/ \
        src/3rdparty/qwt/src \
        src/3rdparty/

DEFINES += NO_GDAL_SUPPORT
unix:LIBS += -lz
win32:LIBS += -lwsock32
LIBS += -Lsrc/3rdparty/qwt/lib -L$$PWD/lib -lqwt
FORMS += forms/rtmainwindow.ui		\
			forms/AboutDialog.ui	\
			forms/PreferencesDialog.ui
RESOURCES += resources/rucktrack.qrc
OTHER_FILES += src/3rdparty/quazip/quazip/quazip.pro
OBJECTS_DIR = obj
MOC_DIR = moc
UI_DIR = uic

