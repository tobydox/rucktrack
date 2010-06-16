/*
 * OpenStreetMapProvider.cpp - implementation of OpenStreetMapProvider class
 *
 * Copyright © 2010 Jens Lang <jenslang/at/users.sourceforge.net>
 *
 * This file is part of RuckTrack - http://rucktrack.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation;
 * version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING).
 *
 */

#include <QtCore/QDebug>
#include <QtWebKit/QWebFrame>
#include <QVariant>

#include "OpenStreetMapProvider.h"

#define evalJS(x) webFrame()->evaluateJavaScript(x)



/**
 * Constructor for OpenStreetMapProvider.
 * \param _parent pointer to parent window.
 */
OpenStreetMapProvider::OpenStreetMapProvider( QWebFrame * _parent ) :
	MapProvider( _parent )
{
}



/**
 * Destructor for OpenStreetMapProvider.
 */
OpenStreetMapProvider::~OpenStreetMapProvider()
{
}




/**
 * Visualise a route on the map.
 * Draw the given route on the map and pan the map to the route. Show all markers.
 * \param _route route to visualise.
 */
void OpenStreetMapProvider::showRoute( const Route & _route )
{
	webFrame()->addToJavaScriptWindowObject( "mapProvider", this );
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
			QString pointStr = QString(
					"new OpenLayers.Geometry.Point( Lon2Merc( %1 ),Lat2Merc( %2 ))").
					arg( pt.longitude() ).arg( pt.latitude() );
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

	evalJS( QString( "multiFeature = new OpenLayers.Feature.Vector(new OpenLayers.Geometry.Collection([new OpenLayers.Geometry.LineString([%1]), %2]));" ).arg( polyline ).arg(markers[0]) );
	evalJS( "layerTrack.addFeatures( [ multiFeature ] );" );
	evalJS( "GEvent.addListener(map, \"click\", function(overlay,latlng) { mapProvider.selectPoint(latlng.lat(),latlng.lng()); });" );
	evalJS( "GEvent.addListener(polyline, \"click\", function(latlng) { mapProvider.selectPoint(latlng.lat(),latlng.lng()); });" );
	foreach( const QString & marker, markers )
	{
// 		evalJS( QString( "map.addOverlay(new GMarker(%1))" ).arg( marker ) );
	}
	if( minLat > invalidVal && minLon > invalidVal &&
		maxLat > invalidVal && maxLat > invalidVal )
	{
		evalJS( QString( "var minLon = Lon2Merc(%1);" ).arg( minLon ) );
		evalJS( QString( "var maxLon = Lon2Merc(%1);" ).arg( maxLon ) );
		evalJS( QString( "var minLat = Lat2Merc(%1);" ).arg( minLat ) );
		evalJS( QString( "var maxLat = Lat2Merc(%1);" ).arg( maxLat ) );
		evalJS( "var bounds = new OpenLayers.Bounds( minLon, minLat, maxLon, maxLat )" );
		evalJS( "var zoom = map.getZoomForExtent( bounds );" );
		evalJS( "map.setCenter( bounds.getCenterLonLat(), zoom, true, true); " );
	}
}




/**
 * Set a marker. Set a marker at the track position that is nearest to the given position.
 */
void OpenStreetMapProvider::highlightPoint( double _lat, double _lon )
{
	evalJS( "layerMarkers.removeMarker( highlightMarker );" );
	evalJS( QString( "var markerLonLat = new OpenLayers.LonLat( Lon2Merc( %1 ), Lat2Merc( %2 ) );" ).arg( _lon ).arg( _lat ) );
	evalJS( "var highlightMarker1 = new OpenLayers.Marker( markerLonLat );" );
	evalJS( "layerMarkers.addMarker( highlightMarker1 );" );
	evalJS( "highlightMarker =  highlightMarker1;" );
}



/**
 * The URL of the map to display in the map widget.
 */
QUrl OpenStreetMapProvider::mapUrl() const
{
	return QUrl( "qrc:/resources/openstreetmap.html" );
}
