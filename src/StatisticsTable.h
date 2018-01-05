/*
 * StatisticsTable.h - header file for StatisticsTable class
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

#ifndef _STATISTICS_TABLE_H
#define _STATISTICS_TABLE_H

#include <QTableWidget>

#include "Route.h"

class StatisticsTable : public QTableWidget
{
	Q_OBJECT
public:
	enum Properties
	{
		Date,
		Duration,
		Distance,
		AverageSpeed,
		MaxSpeed,
		ElevationGain,
		ElevationLoss,
		MinSlope,
		MaxSlope,
		NumProperties
	} ;
	typedef Properties Property;

	StatisticsTable( QWidget * _parent );

	void update( const Route & _route );


private:
	void setItem( Property _prop, int _col, const QString & _name );

	inline void setPropertyName( Property _prop, const QString & _name )
	{
		setItem( _prop, 0, _name );
	}
	inline void setPropertyValue( Property _prop, const QString & _value )
	{
		setItem( _prop, 1, _value );
	}

} ;

#endif // _STATISTICSTABLE_H
