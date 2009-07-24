/*
 * MapView.cpp - implementation of MapView class
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
#include <QtWebKit/QWebFrame>

#include "MapView.h"

#define evalJS(x) page()->mainFrame()->evaluateJavaScript(x)


MapView::MapView( QWidget * _parent ) :
	QWebView( _parent )
{
	page()->mainFrame()->addToJavaScriptWindowObject( "mapView", this );
	setUrl( QUrl( "qrc:/resources/googlemaps.html" ) );
}




void MapView::showRoute( const Route & _route )
{
	page()->mainFrame()->addToJavaScriptWindowObject( "mapView", this );
	QString polyline;
	QStringList markers;
	const double invalidVal = -1000;
	double minLat = invalidVal;
	double minLon = invalidVal;
	double maxLat = invalidVal;
	double maxLon = invalidVal;

	foreach( const TrackSegment & seg, _route )
	{
		bool first = true;
		foreach( const TrackPoint & pt, seg )
		{
			if( minLat <= invalidVal || pt.latitude() < minLat )
			{
				minLat = pt.latitude();
			}
			if( minLon <= invalidVal || pt.longitude() < minLon )
			{
				minLon = pt.longitude();
			}
			if( maxLat <= invalidVal || pt.latitude() > maxLat )
			{
				maxLat = pt.latitude();
			}
			if( maxLon <= invalidVal || pt.longitude() > maxLon )
			{
				maxLon = pt.longitude();
			}
			QString pointStr = QString( "new GLatLng(%1,%2)").
					arg( pt.latitude() ).arg( pt.longitude() );
			if( first )
			{
				markers += pointStr;
				first = false;
			}
			if( !polyline.isEmpty() )
			{
				polyline += ",";
			}
			polyline += pointStr;
		}
	}

	evalJS( "map.clearOverlays()" );
	evalJS( QString( "var polyline = new GPolyline([%1], \"#ff0000\", 4)" ).arg( polyline ) );
	evalJS( "map.addOverlay(polyline)" );
	evalJS( "GEvent.addListener(map, \"click\", function(overlay,latlng) { mapView.selectPoint(latlng.lat(),latlng.lng()); });" );
	evalJS( "GEvent.addListener(polyline, \"click\", function(latlng) { mapView.selectPoint(latlng.lat(),latlng.lng()); });" );
	foreach( const QString & marker, markers )
	{
		evalJS( QString( "map.addOverlay(new GMarker(%1))" ).arg( marker ) );
	}
	if( minLat > invalidVal && minLon > invalidVal &&
		maxLat > invalidVal && maxLat > invalidVal )
	{
		evalJS( QString( "var bounds = new GLatLngBounds;"
					"bounds.extend(new GLatLng(%1,%2));"
					"bounds.extend(new GLatLng(%3,%4));" ).
					arg( minLat ).arg( minLon ).
					arg( maxLat ).arg( maxLon ) );
		evalJS( "map.setCenter(bounds.getCenter(), map.getBoundsZoomLevel(bounds))" );
	}
}




void MapView::highlightPoint( double _lat, double _lon )
{
	evalJS( "map.removeOverlay(pointMarker)" );
	evalJS( QString( "pointMarker = new GMarker(new GLatLng(%1, %2), markerOptions);").
				arg( _lat ).arg( _lon ) );
	evalJS( "map.addOverlay(pointMarker)" );
}




void MapView::selectPoint( double _lat, double _lon )
{
	emit clickedPoint( _lat, _lon );
}

