/*
 * TrackPoint.h - header file for TrackPoint class
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

#ifndef _TRACK_POINT_H
#define _TRACK_POINT_H

#include <QtCore/QDateTime>
#include <QtCore/QList>

#include <math.h>

#define EARTH_RADIUS 6371.01


class TrackPoint
{
public:
	TrackPoint( double _latitude, double _longitude, double _elevation = 0,
			const QDateTime & _time = QDateTime() ) :
		m_latitude( _latitude ),
		m_longitude( _longitude ),
		m_elevation( _elevation ),
		m_time( _time ),
		m_valid( true )
	{
	}

	TrackPoint() :
		m_latitude( 0 ),
		m_longitude( 0 ),
		m_elevation( 0 ),
		m_time(),
		m_valid( false )
	{
	}

	TrackPoint( const TrackPoint & _other ) :
		m_latitude( _other.latitude() ),
		m_longitude( _other.longitude() ),
		m_elevation( _other.elevation() ),
		m_time( _other.time() ),
		m_valid( _other.isValid() )
	{
	}

	double latitude() const
	{
		return m_latitude;
	}

	double longitude() const
	{
		return m_longitude;
	}

	double elevation() const
	{
		return m_elevation;
	}

	void setElevation( double _elevation )
	{
		m_elevation = _elevation;
	}

	const QDateTime & time() const
	{
		return m_time;
	}

	inline bool isValid() const
	{
		return m_valid;
	}

	double distanceTo( const TrackPoint & _other ) const
	{
		if( !_other.isValid() || !isValid() )
		{
			return 0;
		}
		const double f = M_PI / 180.0;
		const double lat1 = latitude()*f;
		const double lat2 = _other.latitude()*f;
		const double a1 = sin( (lat2-lat1) * 0.5 );
		const double a2 = sin( ( _other.longitude() - longitude() ) *
								f * 0.5 );
		const double a = a1*a1 + cos( lat1 ) * cos( lat2 ) * a2*a2;
		return EARTH_RADIUS * 2 * atan2( sqrt( a ), sqrt( 1-a ) );
	}

	double secsTo( const TrackPoint & _other ) const
	{
		if( !_other.isValid() || !isValid() )
		{
			return 1 / 1000;
		}
		const int wholeSecs = time().secsTo( _other.time() );
		double msecs = time().time().msecsTo( _other.time().time() );
		if( msecs < 0 )
		{
			msecs += 24*60*60*1000;
		}
		msecs = wholeSecs*1000 + msecs - floor( msecs / 1000 )*1000;
		if( msecs == 0 )
		{
			msecs = 1;
		}
		return msecs / 1000;
	}


private:
	double m_latitude;
	double m_longitude;
	double m_elevation;
	QDateTime m_time;
	bool m_valid;

} ;

typedef QList<TrackPoint> TrackSegment;
typedef QList<TrackSegment> Route;


#endif // _TRACK_POINT_H
