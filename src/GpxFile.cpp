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
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QProcess>
#include <QtCore/QDebug>
#include <QtXml/QDomDocument>

#include "GpxFile.h"


GpxFile::GpxFile( const QString & fileName ) :
	m_fileName( fileName )
{
}



/**
 *  Convert the file to GPX format.
 *  \return QByteArray containing data in GPX format.
 *  \remarks
 *  We have the following config settings:
 *  <TABLE>
 *   <TR><TH>Name</TH><TH>Type</TH><TH>Description</TH></TR>
 *   <TR><TD><TT>GPX/UseGpsBabel</TT></TD><TD>QBool</TD><TD>allow import of foreign formats via GpsBabel</TD></TR>
 *   <TR><TD><TT>GPX/GpsBabelExecutable</TT></TD><TD>QString</TD><TD>command to execute GpsBabel</TD></TR>
 *   <TR><TD><TT>GPX/GpsBabelImportFormats</TT></TD><TD>QStringList</TD><TD>List of supported import formats:
 *     <TT>"format1,suffix1,description1","format2,suffix2,description2",...</TT> where <TT>format</TT> the format string as needed by
 *     GpsBabel, <TT>suffix</TT> is the file extension and <TT>description</TT> is a human-readable description for the file format
 *     used in the “Open File” dialog.
 *     </TD></TR>
 *  </TABLE>
 */
QByteArray GpxFile::convertToGpx() const
{
	QByteArray ba;

	bool useGpsBabel = QSettings().value( "GPX/UseGpsBabel", false ).toBool();
	if ( !useGpsBabel )
	{
		return "";
	}

	QString gpsBabelExecutable = QSettings().value( "GPX/GpsBabelExecutable", "gpsbabel").toString();
	QStringList conversionFormats = QSettings().value( "GPX/GpsBabelImportFormats", "" ).toStringList();
	QStringListIterator formatsIterator( conversionFormats );
	while ( formatsIterator.hasNext() )
	{
		QStringList formatList = formatsIterator.next().split( "," );
		QString format = formatList.at( 0 ); // first is format (as needed by GpsBabel)
		QString suffix = formatList.at( 1 ); // second is suffix

		QFileInfo fi( m_fileName.toLower() );
		QString fileSuffix =  fi.suffix();
		if ( fileSuffix == suffix )
		{
			// start GpsBabel process to read the foreign file and read the GPX output from stdout
			QProcess process;
			process.start( QString( "%1 -i %2 -f \"%3\" -o gpx -F -" ).arg( gpsBabelExecutable ).arg( format ).arg( m_fileName ) );
			if ( process.waitForFinished() )
			{
				ba = process.readAllStandardOutput();
				QByteArray error = process.readAllStandardError();
				if ( error.isEmpty() )
				{
					// conversion successful
					return ba;
				}
				else
				{
					qDebug() << "Error when executing GpsBabel: " << QString( error );
					return "";
				}
			}
			else
			{
				return "";
			}
		}
	}

	// we did not find a matching suffix
	return "";
}



/**
 * Loads the route from the file indicated in the constructor.
 * \param route reference to the route where the data is to be stored
 * \return whether loading the file was successful or not
 */
bool GpxFile::loadRoute( Route & route ) const
{
	QDomDocument doc;
	QFile f( m_fileName );
	QByteArray ba;

	QFileInfo fi( m_fileName.toLower() );
	QString fileSuffix =  fi.suffix();
	if ( fileSuffix == "gpx" )
	{
		if( !f.open( QFile::ReadOnly ) || !doc.setContent( &f ) )
		{
			return false;
		}
	}
	else
	{
		ba = convertToGpx();
		if ( ba.isEmpty() || !doc.setContent( ba ) )
		{
			return false;
		}
	}

	bool wrongTimestampFix = QSettings().value( "GPX/DropFaultyTrackPoints", true ).toBool();

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
					// fix gaps between track segments
					if( seg > 0 && point == 0 && lastPoint.isValid() )
					{
						timeOffset += qMax<int>( 0, lastPoint.secsTo( TrackPoint( 0, 0, 0, time.addSecs( -timeOffset ) ) ) - 3 );
					}
				}

				// if timestamp of last point is not before current time stamp → leave out track point
				// can be switched off by setting wrongTimestampFix to false
				if( !wrongTimestampFix || lastPoint.time() < time || time.isNull() )
				{
					TrackPoint t(
						e.attribute( "lat" ).toDouble(),
						e.attribute( "lon" ).toDouble(),
						!elev.isEmpty() ?
							elev.at( 0 ).toElement().text().toDouble() : 0,
						time.addSecs( -timeOffset ) );
					trackSeg << t;
					lastPoint = t;
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

