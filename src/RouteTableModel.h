/*
 * RouteTableModel.cpp - header for RouteTableModel class
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

#ifndef _ROUTE_TABLE_MODEL_H
#define _ROUTE_TABLE_MODEL_H

#include <QtCore/QAbstractTableModel>

#include "Route.h"

class RouteTableModel : public QAbstractTableModel
{
public:
	enum Columns
	{
		Time,
		Distance,
		Elevation,
		Speed,
		Slope,
		NumColumns
	} ;
	RouteTableModel( const Route & _route, QObject * _parent = NULL );

	const TrackPoint & trackPoint( const QModelIndex & _idx ) const;
	int rowOfNearestTrackPoint( const TrackPoint & _tp ) const;
	void update()
	{
		reset();
	}


private:
	virtual int rowCount( const QModelIndex & parent ) const;
	virtual int columnCount( const QModelIndex & parent ) const
	{
		Q_UNUSED(parent);
		return NumColumns;
	}
	virtual QVariant data( const QModelIndex & _idx, int role ) const;
	virtual QVariant headerData ( int section, Qt::Orientation orientation, int role ) const;

	const Route & m_route;

} ;

#endif // _ROUTE_TABLE_MODEL_H
