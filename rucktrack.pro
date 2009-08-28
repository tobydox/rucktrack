# -------------------------------------------------
# Project created by QtCreator 2009-07-23T23:49:12
# -------------------------------------------------
QT += script \
    webkit \
    network \
    xml
CONFIG += release
TARGET = rucktrack
TEMPLATE = app
SOURCES += src/main.cpp \
    src/RTMainWindow.cpp \
    src/MapView.cpp \
    src/PlotView.cpp \
    src/RouteTableModel.cpp \
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
	src/3rdparty/qwt/qwt-all.cpp

HEADERS += src/RTMainWindow.h \
    src/MapView.h \
    src/PlotView.h \
    src/ProgressTrackingNetworkAccessManager.h \
    src/RouteTableModel.h \
    src/TrackPoint.h \
    src/SrtmLayer.h \
    src/StatisticsTable.h \
    src/QtBzip2File.h \
    src/3rdparty/quazip/quazip/quazipfile.h \
    src/3rdparty/qwt/qwt_dyngrid_layout.h \
    src/3rdparty/qwt/qwt_legend.h \
    src/3rdparty/qwt/qwt_legend_item.h \
    src/3rdparty/qwt/qwt_plot_canvas.h \
    src/3rdparty/qwt/qwt_plot.h \
    src/3rdparty/qwt/qwt_text_label.h \
    src/3rdparty/qwt/qwt_scale_widget.h

INCLUDEPATH += src \
    src/3rdparty/quazip/quazip \
	src/3rdparty/bzip2/ \
	src/3rdparty/qwt/
LIBS += -lgdal1.5.0
FORMS += forms/rtmainwindow.ui
RESOURCES += resources/rucktrack.qrc
OTHER_FILES += src/3rdparty/quazip/quazip/quazip.pro
OBJECTS_DIR = obj
MOC_DIR = moc
UI_DIR = uic

