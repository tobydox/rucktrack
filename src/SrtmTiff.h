/*
 * SrtmTiff.h - header file for SrtmTiff class
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

#ifndef _SRTM_TIFF_H
#define _SRTM_TIFF_H

#include <QtCore/QString>

class GDALDataset;

class SrtmTiff
{
public:
	SrtmTiff( const QString & _filename );

	inline bool isReady() const
	{
		return m_tile != NULL && m_tile->dataset != NULL;
	}

	double getElevation( double lat, double lon );


private:
	void loadTile( const QString & _filename );
	void posFromLatLon( double lat, double lon, int * row, int * col,
						float * row_f, float * col_f );

	struct Tile
	{
		GDALDataset * dataset;
		int xsize;
		int ysize;
		double lat_origin;
		double lon_origin;
		double lat_pixel;
		double lon_pixel;
		double N, S, E, W;
	} ;

	Tile * m_tile;

} ;

#endif // _SRTM_TIFF_H
