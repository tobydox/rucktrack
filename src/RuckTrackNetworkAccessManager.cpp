/*
 * RuckTrackNetworkAccessManager.h - header file for
 *								   RuckTrackNetworkAccessManager class
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

#include "RuckTrackNetworkAccessManager.h"

#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtGui/QDesktopServices>
#include <QtNetwork/QNetworkDiskCache>
#include <QtNetwork/QNetworkReply>


RuckTrackNetworkAccessManager::RuckTrackNetworkAccessManager(
							QObject * _parent,
							CachingPolicy cachingPolicy ) :
	QNetworkAccessManager( _parent )
{
	QSettings s;
	if( cachingPolicy != NoCaching &&
			s.value( "Maps/EnableCaching", true ).toBool() )
	{
		QNetworkDiskCache * diskCache = new QNetworkDiskCache( this );
		diskCache->setMaximumCacheSize(
			QSettings().value( "Maps/CacheSize", 50 ).toInt() * 1024*1024 );
		diskCache->setCacheDirectory( cachePath() );
		setCache( diskCache );
	}
}




QNetworkReply * RuckTrackNetworkAccessManager::createRequest(
											Operation op,
											const QNetworkRequest & req,
											QIODevice * outgoingData )
{
	QNetworkReply * reply =
				QNetworkAccessManager::createRequest( op, req, outgoingData );
	if( op == GetOperation )
	{
		connect( reply, SIGNAL( downloadProgress( qint64, qint64 ) ),
					this, SLOT( updateProgress( qint64, qint64 ) ) );
	}
	return reply;
}




QString RuckTrackNetworkAccessManager::cachePath()
{
	QString path;
	const bool useSystemCacheDirectory =
		QSettings().value( "General/UseSystemCacheDirectory", false ).toBool();

	if ( useSystemCacheDirectory )
	{
		path = QDesktopServices::storageLocation( QDesktopServices::CacheLocation );
	}
	else
	{
		path = QSettings().value( "General/CacheDirectory",
			QDir::homePath() + QDir::separator() + ".rucktrack" ).toString() +
						QDir::separator() + "MapTileCache" + QDir::separator();
	}

	if( !QFileInfo( path ).isDir() )
	{
		QDir().mkpath( path );
	}
	return path;
}




void RuckTrackNetworkAccessManager::updateProgress( qint64 done, qint64 total )
{
	emit progressChanged( qBound<qint64>( 0, done * 100 /
										qMax<qint64>( 1, total ), 100 ) );
}

