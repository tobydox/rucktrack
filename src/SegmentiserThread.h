/*
 * SegmentiserThread.h - header file for SegmentiserThread class
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

#ifndef SEGMENTISER_THREAD_H_

#include <QtCore/QThread>

class SegmentiserThread : public QThread
{
	Q_OBJECT
public:
	SegmentiserThread(QObject* parent = 0);
	void run();
	void setData( double* x_data_, double* y_data_, double data_size_, double segments_ );
	double* segmentsX();
	double* segmentsY();

private:
	double* x_data;
	double* y_data;
	double data_size;
	double segments;
	bool data_set;
	double* segments_x;
	double* segments_y;
};

#define SEGMENTISER_THREAD_H_
#endif