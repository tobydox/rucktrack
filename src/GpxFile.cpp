/*
 * GpxFile.cpp - implementation of GpxFile class
 *
 * Copyright (c) 2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#include <QtCore/QFile>
#include <QtXml/QDomDocument>

#include "GpxFile.h"


GpxFile::GpxFile( const QString & fileName ) :
	m_fileName( fileName )
{
}



bool GpxFile::loadRoute( Route & route ) const
{
	QDomDocument doc;
	QFile f( m_fileName );
	if( !f.open( QFile::ReadOnly ) || !doc.setContent( &f ) )
	{
		return false;
	}

	route.clear();

	int timeOffset = 0;
	TrackPoint lastPoint;
	QDomNodeList segs = doc.elementsByTagName( "trkseg" );
	for( unsigned int seg = 0; seg < segs.length(); ++seg )
	{
		QDomNodeList trackPointNodes = segs.at( seg ).childNodes();
		TrackSegment trackSeg;
		for( unsigned int point = 0; point < trackPointNodes.length(); ++point )
		{
			QDomNode trackPoint = trackPointNodes.at( point );
			if( trackPoint.isElement() &&
				trackPoint.nodeName() == "trkpt" )
			{
				QDomElement e = trackPoint.toElement();
				QDomNodeList elev =
						e.elementsByTagName( "ele" );
				QDomNodeList timeNode =
						e.elementsByTagName( "time" );
				QDateTime time;
				if( !timeNode.isEmpty() )
				{
					time = QDateTime::fromString(
							timeNode.at( 0 ).toElement().text(),
								Qt::ISODate );
				}
				// fix gaps between track segments
				if( seg > 0 && point == 0 && lastPoint.isValid() )
				{
					timeOffset += qMax<int>( 0, lastPoint.secsTo( TrackPoint( 0, 0, 0, time.addSecs( -timeOffset ) ) ) - 3 );
				}

				if( lastPoint.time() < time )
				{
					TrackPoint t(
						e.attribute( "lat" ).toDouble(),
						e.attribute( "lon" ).toDouble(),
						!elev.isEmpty() ?
							elev.at( 0 ).toElement().text().toDouble() : 0,
						time.addSecs( -timeOffset ) );
					trackSeg << t;
					lastPoint = t;
					//qDebug() << t.latitude() << t.longitude() << t.elevation() << t.time();
				}
				else
				{
					qWarning( "%s",
						qPrintable(
							QObject::tr( "Track point %1 dropped because its time is not later than previous one." ).arg( point + 1 ) ) );
				}
			}
		}
		route << trackSeg;
	}

	return true;
}

