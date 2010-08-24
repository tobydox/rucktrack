/*
 * Segmentiser.h - implementation of Segmentiser class
 *
 * Copyright © 2010 Jens Lang <jenslang/at/users.sourceforge.net>
 *
 * This file is part of RuckTrack - http://rucktrack.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You have received a copy of the GNU General Public License along
 * with this program (see COPYING).
 *
 */

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "3rdparty/levmar/levmar.h"

#include "Segmentiser.h"

double Segmentiser::distance2height( double x, double* x_data, double* y_data, int n_data )
{
	if ( x < x_data[0] )
	{
		return y_data[0];
	}
	if ( x > x_data[n_data-1] )
	{
		return y_data[n_data-1];
	}
	
	assert( n_data >= 2 );
	assert( x >= x_data[0] && x <= x_data[n_data-1] );

	int first = 0;
	int last = n_data - 1;
	while ( last - first > 1 )
	{
		int centre = ( last - first ) / 2 + first;
		if ( x_data[centre] > x )
		{
			last = centre;
		}
		else
		{
			first = centre;
		}
	}
	assert( first + 1 == last );
	assert( x_data[first] <= x && x_data[last] >= x );

	double x1 = x_data[first];
	double x2 = x_data[first+1];
	double y1 = y_data[first];
	double y2 = y_data[first+1];
	double a = (y1 - y2) / (x1 - x2);
	double b = y1 - a * x1;

	return a * x + b;
}

void Segmentiser::fit( double *p, double *new_height, int m, int n, void *adata )
{
	int segments = m;
	double* distance = ( double* ) adata;
	double* height = ( ( double* ) adata ) + n;
	int n_points = n;

	p[0] = 0;
	p[m-1] = distance[n_points-1];

	int i = 0;
	for ( int segment = 0; segment < segments - 1; segment++ )
	{
		double x1 = p[segment];
		double x2 = p[segment+1];
		double y1 = distance2height( x1, distance, height, n_points );
		double y2 = distance2height( x2, distance, height, n_points );
		double a = ( y1 - y2 ) / ( x1 - x2 );
		double b = y1 - a * x1;
		
		while ( p[segment+1] > distance[i] && i < n_points )
		{
			if ( p[segment] < 0 || p[segment] > distance[n_points+1] || p[segment] > p[segment+1] )
			{
				new_height[i] = -1000;
			}
			else
			{
				new_height[i] = a * distance[i] + b;
			}
			i++;
		}
	}
}

Segmentiser::Segmentiser( double* x_values, double* y_values, int count )
{
	// allocate space for our points, needs to be consecutive due to reasons
	n_points = count;
	data = new double[count*2];
	assert( data != NULL );
	x = data;
	y = data + n_points;

	// copy data as the levmar interface does not specify them to be constant
	// although the documentation indicates that
	memcpy( x, x_values, n_points * sizeof( double ) );
	memcpy( y, y_values, n_points * sizeof( double ) );

	// NULL-initialise
	segments_x = NULL;
	segments_y = NULL;
}


void Segmentiser::segmentise( int segments )
{
	n_segments = segments;

	double* p = new double[segments];
	assert( p != NULL );

	double furthest = x[n_points - 1];
	for ( int i = 0; i < segments; i++ )
	{
		p[i] = ( i + 1 ) * furthest / segments;
	}

	int it = dlevmar_dif( fit, p, y, segments, n_points, 1000, NULL, NULL, NULL, NULL, data );

	printf("%d iterations\n", it);

	delete [] segments_y;
	delete [] segments_x;
	segments_x = new double[segments];
	segments_y = new double[segments];
	for ( int i = 0; i < segments; i++ )
	{
		segments_x[i] = p[i];
		segments_y[i] = distance2height( p[i], x, y, n_points );
	}

	delete [] p;
}

double* Segmentiser::segmentsX()
{
	return segments_x;
}

double* Segmentiser::segmentsY()
{
	return segments_y;
}

int Segmentiser::segmentsCount()
{
	return n_segments;
}

Segmentiser::~Segmentiser()
{
	delete [] data;
}
