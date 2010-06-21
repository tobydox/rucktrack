/*
 * SrtmLayer.cpp - implementation of SrtmLayer class
 *
 * Copyright (c) 2009-2010 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 *
 * Parts of this code are based on Python code found in gpxtools
 * by Wojciech Lichota (http://lichota.pl/blog/topics/gpxtools)
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

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtGui/QProgressDialog>
#include <QtNetwork/QNetworkReply>

#include "RTMainWindow.h"
#include "SrtmLayer.h"
#include "SrtmTiff.h"
#include "QtBzip2File.h"

#include "quazipfile.h"

#include <math.h>


SrtmLayer::Cache SrtmLayer::s_cache;


SrtmLayer::SrtmLayer() :
	m_netAccMgr( this, RuckTrackNetworkAccessManager::NoCaching ),
	m_activeDownload( NULL )
{
}




void SrtmLayer::cleanup()
{
#ifdef HAVE_GDAL_SUPPORT
	for( Cache::Iterator it = s_cache.begin(); it != s_cache.end(); ++it )
	{
		delete it.value();
	}
	s_cache.clear();
#endif
}




bool SrtmLayer::getElevation( float lat, float lon, float & elev )
{
#ifdef HAVE_GDAL_SUPPORT
	QString fileName = getSrtmFilename( lat, lon );
	if( !s_cache.contains( fileName ) )
	{
		const QString cachedTIF = fastCachePath() + fileName + ".tif";
		const QString cachedBZ2 = cachePath() + fileName + ".bz2";

		// did we extract the file to the fast cache before?
		if( !QFileInfo( cachedTIF ).isFile() )
		{
			QtBzip2File bzip2File( cachedBZ2 );
			// no, do we have it in our standard cache?
			if( !QFileInfo( cachedBZ2 ).isFile() )
			{
				const QString zipFileName = fileName + ".ZIP";

				QDir().mkpath( fastCachePath() );

				// create zip object
				QuaZip zip( fastCachePath() + zipFileName );
				zip.open( QuaZip::mdUnzip );

				// create zip-file object
				QuaZipFile zipFile( &zip );
				zip.goToFirstFile();
				zipFile.open( QFile::ReadOnly );

				if( !QFileInfo( fastCachePath() + zipFileName ).isFile() ||
															!zipFile.isOpen() )
				{
					if( !downloadSrtmTiff( zipFileName ) )
					{
						return false;
					}
					// re-open ZIP file
					zip.open( QuaZip::mdUnzip );
					zip.goToFirstFile();
					zipFile.open( QFile::ReadOnly );
				}

				QProgressDialog progDlg(
					tr( "Recompressing downloaded SRTM data for future use "
						"(this may take some time)." ),
					QString(), 0, 100, RTMainWindow::instance() );
				progDlg.setModal( true );
				progDlg.show();
				progDlg.setValue( 0 );
				connect( &bzip2File, SIGNAL( progressChanged( int ) ),
							&progDlg, SLOT( setValue( int ) ) );

				// create output file
				QFile outputFile( fastCachePath() + fileName + ".tif" );
				outputFile.open( QFile::WriteOnly );

				// extract our file
				while( zipFile.isOpen() && !zipFile.atEnd() )
				{
					outputFile.write( zipFile.read( 1024*1024 ) );	// read 1 MB chunks
				}
				outputFile.close();
				zipFile.close();
				zip.close();

				// recompress TIF file via bzip2
				bzip2File.compressFrom( cachedTIF );

				// remove ZIP file
				QFile( fastCachePath() + zipFileName ).remove();
			}
			else
			{
				QProgressDialog progDlg(
					tr( "Decompressing cached SRTM data (this may take some time)." ),
					QString(), 0, 100, RTMainWindow::instance() );
				progDlg.setModal( true );
				progDlg.show();
				progDlg.setValue( 0 );
				connect( &bzip2File, SIGNAL( progressChanged( int ) ),
							&progDlg, SLOT( setValue( int ) ) );
				QCoreApplication::processEvents( QEventLoop::AllEvents );

				bzip2File.extractTo( cachedTIF );
			}
		}
		s_cache[fileName] = new SrtmTiff( cachedTIF );
	}

	if( s_cache[fileName]->isReady() )
	{
		elev = s_cache[fileName]->getElevation( lat, lon );
		return true;
	}

	delete s_cache[fileName];
	s_cache.remove( fileName );
#endif

	return false;
}




QString SrtmLayer::cachePath()
{
	const QString path =
		QSettings().value( "General/CacheDirectory",
			QDir::homePath() + QDir::separator() + ".rucktrack" ).toString() +
						QDir::separator() + "SrtmCache" + QDir::separator();

	if( !QFileInfo( path ).isDir() )
	{
		QDir().mkpath( path );
	}
	return path;
}




QString SrtmLayer::fastCachePath()
{
	return QDir::tempPath() + QDir::separator();
}




void SrtmLayer::putDownloadData()
{
	if( m_targetFile->isOpen() )
	{
		m_targetFile->write( m_activeDownload->readAll() );
	}
}




void SrtmLayer::finishDownload()
{
	putDownloadData();
	m_targetFile->close();
	delete m_targetFile;
	delete m_activeDownload;

	m_activeDownload = NULL;
	m_targetFile = NULL;
}




bool SrtmLayer::downloadSrtmTiff( const QString & filename )
{
	if( m_activeDownload )
	{
		qWarning( "a download is already running" );
		return false;
	}

	m_activeDownload = m_netAccMgr.get( QNetworkRequest(
		"http://hypersphere.telascience.org/elevation/cgiar_srtm_v4/tiff/zip/" +
		filename ) );
	connect( m_activeDownload, SIGNAL( readyRead() ),
				this, SLOT( putDownloadData() ) );
	connect( m_activeDownload, SIGNAL( finished() ),
				this, SLOT( finishDownload() ) );

	m_targetFile = new QFile( QDir::tempPath() + QDir::separator() +
									filename );
	m_targetFile->open( QFile::WriteOnly );

	QProgressDialog progDlg( tr( "Initially downloading required SRTM data." ),
								tr( "Cancel" ),
								0, 100, RTMainWindow::instance() );
	progDlg.setModal( true );
	progDlg.show();
	progDlg.setValue( 0 );
	connect( &m_netAccMgr, SIGNAL( progressChanged( int ) ),
				&progDlg, SLOT( setValue( int ) ) );

	while( m_activeDownload )
	{
		QCoreApplication::processEvents( QEventLoop::AllEvents, 500 );
		if( progDlg.wasCanceled() )
		{
			m_targetFile->remove();
			finishDownload();
			return false;
		}
	}
	return true;
}




QString SrtmLayer::getSrtmFilename( float lat, float lon )
{
	float colmin = floor( ( 6000 * ( 180 + lon ) ) / 5 );
	float rowmin = floor( ( 6000 * ( 60 - lat ) ) / 5 );
	int ilon = (int) ceil( colmin / 6000 );
	int ilat = (int) ceil( rowmin / 6000 );

	return QString( "srtm_%1_%2" ).arg( ilon, 2, 10, QChar( '0' ) ).
										arg( ilat, 2, 10, QChar( '0' ) );
}




