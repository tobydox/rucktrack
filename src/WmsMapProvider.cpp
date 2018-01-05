/*
 * WmsMapProvider.cpp - implementation of WmsMapProvider class
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
#include <QWebFrame>
#include <QVariant>
#include <QSettings>

#include "WmsMapProvider.h"



/**
 * Constructor for WmsMapProvider.
 * \param _parent pointer to parent window.
 */
WmsMapProvider::WmsMapProvider( QWebFrame * _parent ) :
	MapProvider( _parent, publicName() )
{
	connect( webFrame(), SIGNAL( loadFinished(bool) ), this, SLOT( webFrameLoadFinished() ) );
}



/**
 * Destructor for WmsMapProvider.
 */
WmsMapProvider::~WmsMapProvider()
{
}



/**
 *  Load map source parameters from configuration file and create a WMS map layer for the OpenLayers map. 
 *  This function is called when the initial loading of the HTML file is finished. Maybe, it better should be
 *  called from JavaScript directly.
 */
void WmsMapProvider::webFrameLoadFinished()
{
	static bool called = false;

	if ( !called )
	{
		called = true;
		webFrame()->addToJavaScriptWindowObject( "mapProvider", this );

		const QString layerName = QSettings().value( "WmsMapProvider/LayerName",
			"Web Map Service" ).toString();
		const QUrl mapSource = QSettings().value( "WmsMapProvider/MapSource",
			"http://193.158.122.53/geobak/servlet/gtEntryPoint" ).toUrl();
		const QString credits = QSettings().value( "WmsMapProvider/CopyrightString",
			"Web Map Service" ).toString();
		const QString layers = QSettings().value( "WmsMapProvider/Layers", "basic" ).toString();
		const QString imageFormat = QSettings().value( "WmsMapProvider/ImageFormat", "image/png" ).toString();
		const QString projection = QSettings().value( "WmsMapProvider/Projection", "EPSG:900913" ).toString();
		const bool singleTile = QSettings().value( "WmsMapProvider/SingleTile", false ).toBool();
		const int tileSize = QSettings().value( "WmsMapProvider/TileSize", -1 ).toInt();
		const QString tileSizeStr = tileSize > 0 ? QString("tileSize: new OpenLayers.Size(%1, %1),").arg(tileSize) : "";

		// create WMS map layer
		evalJS(
			QString( "layerWms = new OpenLayers.Layer.WMS( \"%1\", \"%2\", { layers: \'%3\', format: \'%4\' },"
				"{ singleTile: %5, %6 projection:  new OpenLayers.Projection(\"%7\")});"
				"map.addLayers([layerWms, /*layerWmsExample,*/ layerMarkers, layerTrack]);"
				"map.setCenter(new OpenLayers.LonLat(Lon2Merc(lon), Lat2Merc(lat)), zoom);"
				"layerWms.redraw();" ).
				arg( layerName ).arg( mapSource.toString() ).arg( layers ).arg( imageFormat ).
				arg( singleTile ? "true" : "false" ).arg( tileSizeStr ).arg( projection )
			);

		// add credits at the bottom right edge
		evalJS(
			QString( "var credits = document.getElementById(\"credits\");"
			"credits.innerHTML = '%1';" ).arg( credits )
			);
	}
}




/**
 * Visualise a route on the map.
 * Draw the given route on the map and pan the map to the route. Show all markers.
 * \param _route route to visualise.
 */
void WmsMapProvider::showRoute( const Route & _route )
{
	QString polyline;
	QString markers;
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
				if( !markers.isEmpty() )
				{
					markers += ",";
				}
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

	if( minLat > invalidVal && minLon > invalidVal &&
		maxLat > invalidVal && maxLat > invalidVal )
	{
		evalJS(
			QString( "var minLon = Lon2Merc(%1);" ).arg( minLon ) +
			QString( "var maxLon = Lon2Merc(%1);" ).arg( maxLon ) +
			QString( "var minLat = Lat2Merc(%1);" ).arg( minLat ) +
			QString( "var maxLat = Lat2Merc(%1);" ).arg( maxLat ) +
			"var bounds = new OpenLayers.Bounds( minLon, minLat, maxLon, maxLat );" +
			"var zoom = map.getZoomForExtent( bounds );" +
			"map.setCenter( bounds.getCenterLonLat(), zoom, true, true);"
			);
	}

	evalJS(
		QString( "multiFeature = new OpenLayers.Feature.Vector(new OpenLayers.Geometry.Collection(["
			"new OpenLayers.Geometry.LineString([%1]), %2]));" ).arg( polyline ).arg( markers ) +  // create a track with all markers
		"layerTrack.destroyFeatures();" +                                                        // clear all previous tracks
		"layerTrack.addFeatures( [ multiFeature ] );" +                                          // draw tracks
		"layerTrack.redraw();" +                                                                 // redraw layer (otherwise displayed wrong)
		"GEvent.addListener(map, \"click\", function(overlay,latlng) { mapProvider.selectPoint(latlng.lat(),latlng.lng()); });" +
		"GEvent.addListener(polyline, \"click\", function(latlng) { mapProvider.selectPoint(latlng.lat(),latlng.lng()); });"
		);
}



/**
 * Set a marker. Set a marker at the track position that is nearest to the given position.
 */
void WmsMapProvider::highlightPoint( double _lat, double _lon )
{
	evalJS(
		"if ( !( typeof highlightMarker == 'undefined' ) ) layerMarkers.removeMarker( highlightMarker );" +
		QString( "var markerLonLat = new OpenLayers.LonLat( Lon2Merc( %1 ), Lat2Merc( %2 ) );" ).arg( _lon ).arg( _lat ) +
		"var highlightMarker = new OpenLayers.Marker( markerLonLat );" +
		"layerMarkers.addMarker( highlightMarker );"
		);
}



/**
 * The URL of the map to display in the map widget.
 */
QUrl WmsMapProvider::mapUrl() const
{
	return QUrl( "qrc:/resources/wms.html" );
}
