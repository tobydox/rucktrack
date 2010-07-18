/*
 * StatisticsTable.cpp - implementation of StatisticsTable
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

#include <QtCore/QDebug>

#include "StatisticsTable.h"


StatisticsTable::StatisticsTable( QWidget * _parent ) :
	QTableWidget( _parent )
{
	setRowCount( NumProperties );
	setColumnCount( 2 );

	setPropertyName( Time, tr( "Time" ) );
	setPropertyName( Distance, tr( "Distance" ) );
	setPropertyName( AverageSpeed, tr( "Average speed" ) );
	setPropertyName( MaxSpeed, tr( "Max speed" ) );
	setPropertyName( ElevationGain, tr( "Elevation gain" ) );
	setPropertyName( ElevationLoss, tr( "Elevation loss" ) );
	setPropertyName( MinSlope, tr( "Min slope" ) );
	setPropertyName( MaxSlope, tr( "Max slope" ) );
}




void StatisticsTable::update( const Route & _route )
{

	double length = 0;
	double elevGain = 0;
	double elevLoss = 0;
	double maxSpeed = -1;
	double timeSecs = 0;
	double speed = 0;
	double minSlope = 0;
	double maxSlope = 0;
	double slope = 0;

	foreach( const TrackSegment & seg, _route )
	{
		bool first = true;
		TrackPoint lastPoint;
		foreach( const TrackPoint & pt, seg )
		{
			if( first )
			{
				first = false;
			}
			else
			{
				const double secs = lastPoint.secsTo( pt );
				const double dist = lastPoint.distanceTo( pt );
				timeSecs += secs;
				length += dist;
				const double dElev = pt.elevation() -
							lastPoint.elevation();
				if( dElev < 0 )
				{
					elevLoss += -dElev;
				}
				else
				{
					elevGain += dElev;
				}

				if( dist > 0 )
				{
					slope = slope*0.6 + dElev / ( dist * 10 ) * 0.4;
					if( slope > maxSlope )
					{
						maxSlope = slope;
					}
					if( slope < minSlope )
					{
						minSlope = slope;
					}
				}
				// smooth speed a bit
				speed = speed*0.3 + 0.7*(dist*1000 / secs);
				if( maxSpeed == -1 || speed > maxSpeed )
				{
					maxSpeed = speed;
				}
			}
			lastPoint = pt;
		}
	}

	setPropertyValue( Time, QTime().addSecs( timeSecs ).toString( tr( "hh:mm:ss" ) ) );
	setPropertyValue( Distance, QString( tr( "%1 km" ) ).arg( length, 0, 'f', 1 ) );
	setPropertyValue( AverageSpeed, QString( tr( "%1 km/h" ) ).arg( 3600 * length / timeSecs, 0, 'f', 1 ) );
	setPropertyValue( MaxSpeed, QString( tr( "%1 km/h" ) ).arg( maxSpeed * 3.6, 0, 'f', 1 ) );
	setPropertyValue( ElevationGain, QString( tr( "%1 m" ) ).arg( elevGain, 0, 'f', 1 ) );
	setPropertyValue( ElevationLoss, QString( tr( "%1 m" ) ).arg( elevLoss, 0, 'f', 1 ) );
	setPropertyValue( MinSlope, QString( tr( "%1 %" ) ).arg( minSlope, 0, 'f', 1 ) );
	setPropertyValue( MaxSlope, QString( tr( "%1 %" ) ).arg( maxSlope, 0, 'f', 1 ) );
}




void StatisticsTable::setItem( Property _prop, int _col, const QString & _name )
{
	if( item( _prop, _col ) == NULL )
	{
		QTableWidget::setItem( _prop, _col, new QTableWidgetItem( _name ) );
	}
	else
	{
		item( _prop, _col )->setText( _name );
	}
}


