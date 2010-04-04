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

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QProgressBar>

#include "AboutDialog.h"
#include "GpxFile.h"
#include "RTMainWindow.h"
#include "ProgressTrackingNetworkAccessManager.h"
#include "RouteTableModel.h"
#include "SrtmLayer.h"

#include "ui_rtmainwindow.h"

RTMainWindow * RTMainWindow::_this = NULL;


RTMainWindow::RTMainWindow(QWidget *parent) :
	QMainWindow( parent ),
	ui( new Ui::RTMainWindow ),
	m_currentRoute(),
	m_routeTableModel( new RouteTableModel( m_currentRoute, this ) )
{
	_this = this;

	ui->setupUi(this);

	// the whole app is DockWidget-based, therefore hide centralWidget
	ui->centralWidget->hide();

	// graphsDock and trackDetailsDock should be tabified per default - designer doesn't
	// allow to configure this, so do this manually and make sure, graphsDock still is on top
	tabifyDockWidget( ui->graphsDock, ui->trackDetailsDock );	
	ui->graphsDock->raise();

	QProgressBar * webPageProgress = new QProgressBar;
	webPageProgress->setFixedHeight( 16 );
	webPageProgress->setTextVisible( false );
	ui->statusBar->addWidget( webPageProgress );
	//connect( ui->mapView, SIGNAL(loadStarted()), webPageProgress, SLOT(show()));
	//connect( ui->mapView->page(), SIGNAL(loadFinished(bool)), webPageProgress, SLOT(hide()));
	connect( ui->mapView, SIGNAL(loadProgress(int)), webPageProgress, SLOT(setValue(int)));

	ProgressTrackingNetworkAccessManager * nam =
			new ProgressTrackingNetworkAccessManager( this );
	ui->mapView->page()->setNetworkAccessManager( nam );
	connect( nam, SIGNAL( progressChanged( int ) ),
				webPageProgress, SLOT( setValue( int ) ) );

	// setup TrackPointsView
	ui->trackPointsView->setModel( m_routeTableModel );
	connect( ui->trackPointsView->selectionModel(),
				SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ),
				this, SLOT( highlightSelectedTrackPoint( const QModelIndex &, const QModelIndex & ) ) );
	connect( ui->mapView, SIGNAL( clickedPoint( double, double ) ),
				this, SLOT( selectTrackPoint( double, double ) ) );
	connect( ui->plotView, SIGNAL( clickedPoint( double, double ) ),
				this, SLOT( selectTrackPoint( double, double ) ) );

	// connect actions
	connect( ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT( about() ) );
	connect( ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT( openFile() ) );
	connect( ui->actionFixElevations, SIGNAL(triggered(bool)),
				this, SLOT( fixElevations() ) );

}



RTMainWindow::~RTMainWindow()
{
	delete ui;
	SrtmLayer::cleanup();
}




void RTMainWindow::about()
{
	AboutDialog( this ).exec();
}




void RTMainWindow::openFile()
{
	QString fileName =
		QFileDialog::getOpenFileName(
			this, tr( "Open File" ), QString(),
			tr("GPX files (*.gpx)") );
	if( fileName.isEmpty() )
	{
		return;
	}

	if( GpxFile( fileName ).loadRoute( m_currentRoute ) )
	{
		ui->mapView->showRoute( m_currentRoute );
		ui->plotView->showRoute( m_currentRoute );
		ui->statsTable->update( m_currentRoute );
		m_routeTableModel->update();
	}
}




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




void RTMainWindow::selectTrackPoint( double _lat, double _lon )
{
	const int row = m_routeTableModel->rowOfNearestTrackPoint(
												TrackPoint( _lat, _lon ) );
	if( row >= 0 )
	{
		ui->trackPointsView->selectRow( row );
	}
}




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

