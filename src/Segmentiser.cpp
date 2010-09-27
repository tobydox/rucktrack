/*
 * Segmentiser.h - implementation of Segmentiser class
 *
 * Copyright © 2010 Jens Lang <jenslang/at/users.sourceforge.net>
 * Algorithm and GAP implementation by Stefan Vogel
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
#include <cstring>
#include <cmath>
#include "Segmentiser.h"

/**
 *  A number of points from index \b i1 to index \b i2
 *  is being approximated by a line and the deviation is
 *  calculated.
 *  \param i1 index of first point.
 *  \param i2 index of last point.
 */
double Segmentiser::lineDist( int i1, int i2 )
{
	double x1 = x[i1];
	double x2 = x[i2];
	double y1 = y[i1];
	double y2 = y[i2];
	double d = 0;

	if (x1 != x2)  // there might be multiple points
	{
		// calculate a,b of an approximation line y = a*x+b
		// several variants possible, we do it like this at the moment:
		double a = (y1 - y2) / (x1 - x2);
		double b = (x1 * y2 - x2 * y1) / (x1 - x2);
		// now summation of the squared distances
// 		// again, several methods possible
		for (int k = i1; k <= i2; k++)
		{
			d += pow(y[k] - (a * x[k] + b), 2);
		}
	}

	return d;
}

/**
 *  The decomposition of the whole curve in these line segments
 *  is done with a vector \b ind which determines the indices of
 *  the decomposition points. Hence, at the beginning, we have
 *  <tt>ind=[1..9]</tt>, at the end, let's say, <tt>ind=[1,3,6,9]</tt>.
 *  This function calculates how the overall deviation would change
 *  if we took out the position <tt>ind[i]</tt> out and thereby
 *  reduced the number of lines by one.
 */
double Segmentiser::diffDist( const QVector<int>& ind, int i )
{
	double i1;
	double i2;
	double d;

	i1 = ind[i-1];
	i2 = ind[i+1];
	d = lineDist( i1, i2 );

	i1 = ind[i-1];
	i2 = ind[i];
	d = d - lineDist( i1, i2 );

	i1 = ind[i];
	i2 = ind[i+1];
	d = d - lineDist( i1, i2 );

	return d;
}

/**
 *  In ind[i], the indices are stored,
 *  in inddiffdist, the deviation calculated by
 *  diffDist() is stored.
 *  The search for the point to be deleted therefore
 *  is only finding the minimum in the inddiffdist
 *  vector.
 */
QVector<int> Segmentiser::letsStart()
{
	QVector<int> ind( n );
	QVector<int> result;
	QVector<double> inddiffdist( n );

	for ( int k = 0; k < n; k++ )
	{
		ind[k] = k;
	}

	for ( int k = 1; k < n - 1; k++ )
	{
		inddiffdist[k] = diffDist( ind, k );
	}

	// now take out one point in every step k
	for ( int k = 0; k < n - 2; k++ )
	{
		double dmin = lineDist( 0, n - 1 );
		int imerk = 1;
		// now determine the smalles value of indiffdist
		for ( int i = 1; i < ind.size() - 1; i++ )
		{
			double d = inddiffdist[i];
			if ( dmin > d )
			{
				dmin = d;
				imerk = i;
			}
		}

		// remove position from ind
		ind.remove( imerk );
		// ... and also from inddiffdist
		inddiffdist.remove( imerk );

		// now do only update inddiffdist around position i
		// (this saves a lot of execution time)
		if ( imerk > 1 )
		{
			inddiffdist[imerk-1] = diffDist( ind, imerk - 1 );
		}

		if (imerk < ind.size() - 1)
		{
			inddiffdist[imerk] = diffDist( ind, imerk );
		}

		// remember result when we have 15 lines
		if ( ind.size() == n_segments )
		{
			return ind;
		}
	}

	Q_ASSERT( false );
	return ind;
}


/**
 *  Create a Segmentiser object. This will take some track points as input variables
 *  find the optimal division into linear sections. The desired number of sections
 *  has to be given.
 *  \param x_values x co-ordinates of the track points
 *  \param y_valies y co-ordinates of the track points
 *  \param count number of track points
 */
Segmentiser::Segmentiser( double* x_values, double* y_values, int count )
{
	n = count;
	x = new double[n];
	y = new double[n];
	memcpy( x, x_values, sizeof(double) * n );
	memcpy( y, y_values, sizeof(double) * n );

	// NULL-initialise
	segments_x = NULL;
	segments_y = NULL;
}


/**
 *  Calculate the best division of the track into linear segments. The result
 *  can be retrieved with \b segmentsX() and \b segmentY().
 *  \param segments number of segments
 */
void Segmentiser::segmentise( int segments )
{
	n_segments = segments;

	// only segmentise if we have less segments than track points
	if ( segments > n )
	{
		return;
	}

	QVector<int> result = letsStart();

	delete [] segments_y;
	delete [] segments_x;
	segments_x = new double[segments];
	segments_y = new double[segments];
	for ( int i = 0; i < segments; i++ )
	{
		segments_x[i] = x[result[i]];
		segments_y[i] = y[result[i]];
	}
}

/**
 *  Get the x co-ordinates of the segment boundaries. The segments produced at
 *  the last call of \b segmentise() will be returned. Will be \b NULL if there
 *  are no segments.
 */
double* Segmentiser::segmentsX()
{
	return segments_x;
}

/**
 *  Get the y co-ordinates of the segment boundaries. The segments produced at
 *  the last call of \b segmentise() will be returned. Will be \b NULL if there
 *  are no segments.
 */
double* Segmentiser::segmentsY()
{
	return segments_y;
}

/**
 *  Get the number of segments as returned by \b segmentsX() and \b segmentY().
 *  Equals \b segments of the last call of \b segmentise().
 */
int Segmentiser::segmentsCount()
{
	return n_segments;
}

Segmentiser::~Segmentiser()
{
	delete [] x;
	delete [] y;
}
