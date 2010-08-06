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
	static double distance2height(double x, double* x_data, double* y_data, int n_data);
	static void fit(double *p, double *new_height, int m, int n, void *adata);

	int n_points;
	double* data;
	double* x;
	double* y;
	int n_segments;
	double* segments_x;
	double* segments_y;
} ;