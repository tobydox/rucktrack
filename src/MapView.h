/*
 * MapView.h - header file for MapView class
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

#ifndef _MAP_VIEW_H
#define _MAP_VIEW_H

#include <QWebView>

#include "Route.h"

class MapProvider;

class MapView : public QWebView
{
public:
	MapView( QWidget * _parent );
	virtual ~MapView();

	void setMapProvider( MapProvider * mapProvider );
	MapProvider * mapProvider()
	{
		return m_mapProvider;
	}

	void showRoute( const Route & _route );
	void highlightPoint( double _lat, double _lon );


private:
	MapProvider * m_mapProvider;

} ;


#endif // _MAP_VIEW_H
