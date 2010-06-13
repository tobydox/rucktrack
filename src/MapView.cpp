/*
 * MapView.cpp - implementation of MapView class
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

#include <QtWebKit/QWebFrame>

#include "MapProvider.h"
#include "MapView.h"

#define evalJS(x) page()->mainFrame()->evaluateJavaScript(x)


MapView::MapView( QWidget * _parent ) :
	QWebView( _parent ),
	m_mapProvider( NULL )
{
	page()->mainFrame()->addToJavaScriptWindowObject( "mapView", this );
}




MapView::~MapView()
{
	delete m_mapProvider;
}




void MapView::setMapProvider( MapProvider * mapProvider )
{
	m_mapProvider = mapProvider;
	setUrl( mapProvider->mapUrl() );
}




void MapView::showRoute( const Route & _route )
{
	if( mapProvider() )
	{
		mapProvider()->showRoute( _route );
	}
}




void MapView::highlightPoint( double _lat, double _lon )
{
	if( mapProvider() )
	{
		mapProvider()->highlightPoint( _lat, _lon );
	}
}

