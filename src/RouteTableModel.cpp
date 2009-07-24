/*
 * RouteTableModel.cpp - implementation of RouteTableModel class
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

#include "RouteTableModel.h"


RouteTableModel::RouteTableModel( const Route & _route, QObject * _parent ) :
	QAbstractTableModel( _parent ),
	m_route( _route )
{
}




const TrackPoint & RouteTableModel::trackPoint( const QModelIndex & _idx ) const
{
	const int row = _idx.row();

	int item = 0;
	foreach( const TrackSegment & seg, m_route )
	{
		foreach( const TrackPoint & pt, seg )
		{
			if( item == row )
			{
				return pt;
			}
			++item;
		}
	}
	static TrackPoint pt;
	return pt;
}




int RouteTableModel::rowOfNearestTrackPoint( const TrackPoint & _tp ) const
{
	// iterate until we found a matching point in the neighbourhood of
	// _tp or delta got too big
	double delta = 0.00005;
	while( delta < 0.1 )
	{
		int item = 0;
		foreach( const TrackSegment & seg, m_route )
		{
			foreach( const TrackPoint & pt, seg )
			{
				if( qAbs( pt.latitude() - _tp.latitude() ) < delta &&
					qAbs( pt.longitude() - _tp.longitude() ) < delta )
				{
					return item;
				}
				++item;
			}
		}
		delta *= 2;
	}
	return -1;
}




int RouteTableModel::rowCount( const QModelIndex & parent ) const
{
	Q_UNUSED(parent);

	int items = 0;
	foreach( const TrackSegment & seg, m_route )
	{
		items += seg.size();
	}
	return items;
}




QVariant RouteTableModel::data( const QModelIndex & _idx, int _role ) const
{
	if( _role != Qt::DisplayRole )
	{
		return QVariant();
	}

	const int col = _idx.column();
	const int row = _idx.row();

	TrackPoint lastPoint;
	int item = 0;
	double timeSecs = 0;
	double length = 0;
	foreach( const TrackSegment & seg, m_route )
	{
		foreach( const TrackPoint & pt, seg )
		{
			if( col == Time ) timeSecs += lastPoint.secsTo( pt );
			if( col == Distance ) length += lastPoint.distanceTo( pt );
			if( item == row )
			{
				switch( _idx.column() )
				{
					case Time: return QTime().addSecs( timeSecs ).toString( "hh:mm:ss" );
					case Distance: return QString( "%1 km" ).arg( length, 0, 'f', 1 );
					case Elevation: return QString( "%1 m" ).arg( pt.elevation(), 0, 'f', 1 );
					case Speed: return QString( "%1 km/h" ).
								arg( 3600 * lastPoint.distanceTo( pt ) /
										lastPoint.secsTo( pt ), 0, 'f', 1 );
					case Slope: return QString( "%1 %" ).
								arg( ( pt.elevation() - lastPoint.elevation() ) /
										( lastPoint.distanceTo( pt ) * 10 ), 0, 'f', 1 );
				}
			}
			++item;
			lastPoint = pt;
		}
	}

	return QVariant();
}




QVariant RouteTableModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
	if( orientation == Qt::Horizontal && role == Qt::DisplayRole )
	{
		switch( section )
		{
			case Time: return tr( "Time" );
			case Distance: return tr( "Distance" );
			case Elevation: return tr( "Elevation" );
			case Speed: return tr( "Speed" );
			case Slope: return tr( "Slope" );
		}
	}
	return QAbstractTableModel::headerData( section, orientation, role );
}


