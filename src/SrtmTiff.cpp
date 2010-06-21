/*
 * SrtmTiff.cpp - implementation of SrtmTiff class
 *
 * Copyright (c) 2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 *
 * Parts of this code are based on Python code found in gpxtools
 * by Wojciech Lichota (http://lichota.pl/blog/topics/gpxtools)
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

#ifndef NO_GDAL_SUPPORT

#include <QtCore/QFile>

#include "SrtmTiff.h"

#include <math.h>

#include <gdal_priv.h>


SrtmTiff::SrtmTiff( const QString & _filename ) :
	m_tile( NULL )
{
	loadTile( _filename );
}


static inline double bilinearInterpolation( double tl, double tr,
											double bl, double br,
											double a, double b )
{
	const double b1 = tl;
	const double b2 = bl - tl;
	const double b3 = tr - tl;
	const double b4 = tl - bl -tr + br;

	return b1 + b2*a + b3*b + b4*a*b;
}



double SrtmTiff::getElevation( double lat, double lon )
{
	int row, col;
	float row_f, col_f;
	posFromLatLon( lat, lon, &row, &col, &row_f, &col_f );

	if( row == 5999 )
	{
		row = 5998;
	}
	if( col == 5999 )
	{
		col = 5998;
	}

	double elevations[2][2];
	m_tile->dataset->GetRasterBand( 1 )->
		RasterIO( GF_Read, col, row, 2, 2, elevations, 2, 2, GDT_Float64, 0, 0 );

	return bilinearInterpolation( elevations[0][0],
									elevations[0][1],
									elevations[1][0],
									elevations[1][1],
									row_f - row, col_f - col );
	
}




void SrtmTiff::loadTile( const QString & _filename )
{
	delete m_tile;

	m_tile = new Tile;
	GDALAllRegister();
	m_tile->dataset = static_cast<GDALDataset *>(
					GDALOpen( _filename.toUtf8().constData(), GA_ReadOnly ) );
	if( !m_tile->dataset )
	{
		QFile( _filename ).remove();
		qWarning( "GDALOpen() returned NULL" );
		return;
	}
	double geotransform[6];
	m_tile->dataset->GetGeoTransform( geotransform );

	m_tile->xsize = m_tile->dataset->GetRasterXSize();
	m_tile->ysize = m_tile->dataset->GetRasterYSize();

	m_tile->lat_origin = geotransform[3];
	m_tile->lon_origin = geotransform[0];
	m_tile->lat_pixel = geotransform[5];
	m_tile->lon_pixel = geotransform[1];

	m_tile->N = m_tile->lat_origin;
	m_tile->S = m_tile->lat_origin + m_tile->lat_pixel*m_tile->ysize;
	m_tile->E = m_tile->lon_origin + m_tile->lon_pixel*m_tile->xsize;
	m_tile->W = m_tile->lon_origin;

}




void SrtmTiff::posFromLatLon( double lat, double lon, int * row, int * col,
							float * row_f, float * col_f )
{
	*row_f = ( lat - m_tile->N ) / m_tile->lat_pixel;
	*col_f = ( lon - m_tile->W ) / m_tile->lon_pixel;
	*row = (int) floor( *row_f );
	*col = (int) floor( *col_f );

	if( *row < 0 ) *row = 0;
	if( *row > m_tile->xsize-1 ) *row = m_tile->xsize-1;
	if( *col < 0 ) *col = 0;
	if( *col > m_tile->ysize-1 ) *col = m_tile->ysize-1;
}

#endif
