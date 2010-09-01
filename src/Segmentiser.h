/*
 * Segmentiser.h - header file for Segmentiser class
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

#ifndef SEGMENTISER_H_
#define SEGMENTISER_H_

#include <QtCore/QVector>

class Segmentiser
{
public:
	Segmentiser(double* x_values, double* y_values, int count);
	~Segmentiser();

	void segmentise(int segments);

	double* segmentsX();
	double* segmentsY();
	int segmentsCount();

private:
	QVector<int> letsStart();
	double diffDist(const QVector<int>& ind, int i);
	double lineDist(int i1, int i2);

	int n;
	double* x;
	double* y;
	int n_segments;
	double* segments_x;
	double* segments_y;
} ;

#endif /* SEGMENTISER_H_ */
