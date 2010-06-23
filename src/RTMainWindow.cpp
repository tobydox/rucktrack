/*
 * RTMainWindow.cpp - implementation of RTMainWindow class
 *
 * Copyright (c) 2009-2010 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 *
 * This file is part of RuckTrack - http://rucktrack.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */

#include <QtCore/QSettings>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

#include "AboutDialog.h"
#include "GoogleMapsProvider.h"
#include "GpxFile.h"
#include "RTMainWindow.h"
#include "OpenStreetMapProvider.h"
#include "PreferencesDialog.h"
#include "RuckTrackNetworkAccessManager.h"
#include "RouteTableModel.h"
#include "SrtmLayer.h"

#include "ui_rtmainwindow.h"

RTMainWindow * RTMainWindow::_this = NULL;

/**
 *  Constructor for the Rucktrack Main Window.
 *  \param
 */
RTMainWindow::RTMainWindow(QWidget *parent) :
	QMainWindow( parent ),
	ui( new Ui::RTMainWindow ),
	m_currentRoute(),
	m_routeTableModel( new RouteTableModel( m_currentRoute, this ) ),
	m_webPageProgressBar( new QProgressBar )
{
	_this = this;

	ui->setupUi( this );

	// the whole app is DockWidget-based, therefore hide centralWidget
	ui->centralWidget->hide();

	// graphsDock and trackDetailsDock should be tabified per default - designer doesn't
	// allow to configure this, so do this manually and make sure, graphsDock still is on top
	tabifyDockWidget( ui->graphsDock, ui->trackDetailsDock );
	ui->graphsDock->raise();

	RuckTrackNetworkAccessManager * nam =
									new RuckTrackNetworkAccessManager( this );
	ui->mapView->page()->setNetworkAccessManager( nam );

	// setup progress bar for MapView
	m_webPageProgressBar->setFixedHeight( 16 );
	m_webPageProgressBar->setTextVisible( false );
	ui->statusBar->addWidget( m_webPageProgressBar );
	connect( ui->mapView, SIGNAL( loadProgress( int ) ),
				m_webPageProgressBar, SLOT( setValue( int ) ) );
	connect( nam, SIGNAL( progressChanged( int ) ),
				m_webPageProgressBar, SLOT( setValue( int ) ) );

	setPreferences();

	// setup TrackPointsView
	ui->trackPointsView->setModel( m_routeTableModel );
	connect( ui->trackPointsView->selectionModel(),
				SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ),
				this, SLOT( highlightSelectedTrackPoint( const QModelIndex &, const QModelIndex & ) ) );
	connect( ui->mapView->mapProvider(), SIGNAL( clickedPoint( double, double ) ),
				this, SLOT( selectTrackPoint( double, double ) ) );
	connect( ui->plotView, SIGNAL( clickedPoint( double, double ) ),
				this, SLOT( selectTrackPoint( double, double ) ) );

	// connect actions
	connect( ui->actionOpen, SIGNAL( triggered(bool) ), this, SLOT( openFile() ) );
	connect( ui->actionPreferences, SIGNAL( triggered(bool) ), this, SLOT( preferences() ) );
	connect( ui->actionAbout, SIGNAL( triggered(bool) ), this, SLOT( about() ) );
	connect( ui->actionFixElevations, SIGNAL(triggered(bool)),
				this, SLOT( fixElevations() ) );

	// parse command line parameters after map is loaded (would not work if we did it immediately)
	connect( ui->mapView, SIGNAL( loadFinished(bool) ), this, SLOT( parseCommandLineParameters() ) );
}



/**
 *  Destroy the Rucktrack main window and cleanup.
 */
RTMainWindow::~RTMainWindow()
{
	delete ui;
	SrtmLayer::cleanup();
}




/**
 *  Show the “Preferences” dialog.
 */
void RTMainWindow::preferences()
{
	PreferencesDialog().exec();

	setPreferences();
}




/**
 *  Show the “About” dialog.
 */
void RTMainWindow::about()
{
	AboutDialog( this ).exec();
}




/**
 *  Open the “Open File” dialog and load the specified GPX file.
 */
void RTMainWindow::openFile()
{
	QSettings s;

	QString fileName =
		QFileDialog::getOpenFileName(
			this, tr( "Open File" ),
			s.value( "Misc/LastDirectory" ).toString(),
			tr("GPX files (*.gpx)") );
	if( fileName.isEmpty() )
	{
		return;
	}

	s.setValue( "Misc/LastDirectory",
						QFileInfo( fileName ).absoluteDir().absolutePath() );

  loadRoute( fileName );
}



/**
 *  Do all the necessary internal and UI stuff to load a GPX file.
 *  \param fileName name of the GPX file to be loaded.
 */
void RTMainWindow::loadRoute( const QString & fileName )
{
	if( GpxFile( fileName ).loadRoute( m_currentRoute ) )
	{
		ui->mapView->showRoute( m_currentRoute );
		ui->plotView->showRoute( m_currentRoute );
		ui->statsTable->update( m_currentRoute );
		m_routeTableModel->update();
	}
}




/**
 *  Fix elevations of the route by using SRTM data and update the user interface.
 */
void RTMainWindow::fixElevations()
{
	if( m_currentRoute.isEmpty() )
	{
		QMessageBox::information( this, tr( "No track loaded" ),
									tr( "Please load a track first!" ) );
		return;
	}

	double err = 0;
	int items = 0;
	float curElev = 0;
	SrtmLayer layer;
	bool validData = true;
	for( Route::Iterator routeIt = m_currentRoute.begin();
			routeIt != m_currentRoute.end() && validData; ++routeIt )
	{
		for( TrackSegment::Iterator it = routeIt->begin(); it != routeIt->end(); ++it )
		{
			if( layer.getElevation( it->latitude(), it->longitude(), curElev ) )
			{
				err += qAbs( curElev - it->elevation() );
				it->setElevation( curElev );
			}
			else
			{
				validData = false;
				break;
			}
		}
		items += routeIt->size();
	}
	if( validData )
	{
		ui->statusBar->showMessage(
			QString( "Fixed elevation of %1 points (average error of %2 m)." ).
				arg( items ).arg( err / items, 0, 'f', 1 ), 5000 );
		ui->statsTable->update( m_currentRoute );
		ui->plotView->showRoute( m_currentRoute );
	}
	else
	{
		ui->statusBar->showMessage(
			QString( "Failed to retrieve SRTM elevation data. "
						"Please try again." ), 5000 );
	}
}




/**
 *  Select a track point on the map. Move the blue marker (Google Maps)
 *  to the trackpoint least distant to the given position.
 *  \param _lat Latitude of the desired point.
 *  \param _lat Longitude of the desired point.
 */
void RTMainWindow::selectTrackPoint( double _lat, double _lon )
{
	const int row = m_routeTableModel->rowOfNearestTrackPoint(
												TrackPoint( _lat, _lon ) );
	if( row >= 0 )
	{
		ui->trackPointsView->selectRow( row );
	}
}




/**
 *  Highlight a position on the map. Put markers to the indicated track
 *  points.
 *  \param _current Track point index of the first marker.
 *  \param _prev    Track point index of the second marker.
 */
void RTMainWindow::highlightSelectedTrackPoint( const QModelIndex & _idx,
												const QModelIndex & )
{
	const TrackPoint & selectedTP = m_routeTableModel->trackPoint( _idx );
	if( selectedTP.isValid() )
	{
		ui->mapView->highlightPoint( selectedTP.latitude(),
									  selectedTP.longitude() );
	}
}




/**
 *  Parse the command line parameters given to the application and do
 *  all tasks demanded there.
 */
void RTMainWindow::parseCommandLineParameters()
{
	// we do not want to parse command line parameters more than once
	disconnect( this, SLOT( parseCommandLineParameters() ) );

	QStringList args = QApplication::instance()->arguments();

	// Remove the first argument which normally contains the executable
	// for that we don't take it as an argument accidentially.
	if ( !args.isEmpty() )
	{
		args.removeFirst();
	}

	// The last argument is the file name. We want to open this file.
	if ( !args.isEmpty() )
	{
		QString filename = args.first();

		loadRoute( filename );
	}
}



/**
 *  Synchronize preference settings with currently active settings.
 */
void RTMainWindow::setPreferences()
{
	setPreferenceShowProgressBar();
	setPreferenceMapProvider();
}




/**
 *  Set the map provider to the value saved in the settings.
 */
void RTMainWindow::setPreferenceMapProvider()
{
	QSettings s;
	QString preferredMapProvider =  s.value( "Maps/MapProvider" ).toString();

	// only act if MapProvider changed
	if (ui->mapView->mapProvider() == NULL ||
		ui->mapView->mapProvider()->name() != preferredMapProvider)
	{
		// TODO: dynamic plugin-based concept
		MapProvider * mapProvider;
		if( preferredMapProvider ==
				OpenStreetMapProvider::publicName() )
		{
			// install OpenStreetMapProvider
			mapProvider =  new OpenStreetMapProvider(
											ui->mapView->page()->mainFrame() );
		}
		else
		{
			// install GoogleMapsProvider
			mapProvider =  new GoogleMapsProvider(
											ui->mapView->page()->mainFrame() );
		}
		ui->mapView->setMapProvider( mapProvider );
	}
}



/**
 *  Set the visibility of the MapView progress bar to the value saved in the settings.
 */
void RTMainWindow::setPreferenceShowProgressBar()
{
	QSettings s;
	bool showProgressBar = s.value( "UI/ShowProgressBar", true ).toBool();

	m_webPageProgressBar->setVisible( showProgressBar );
}
