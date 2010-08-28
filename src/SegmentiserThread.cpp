/*
 * SegmentiserThread.cpp - implementation of SegmentiserThread class
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

#include "SegmentiserThread.h"
#include "Segmentiser.h"
#include <QtCore/QDebug>

SegmentiserThread::SegmentiserThread( QObject* parent ): QThread( parent )
{
	Q_ASSERT( !(data_set = false) );
}



void SegmentiserThread::run()
{
	Q_ASSERT( data_set );
	Segmentiser segmentiser( x_data, y_data, data_size );
	segmentiser.segmentise( segments );
	segments_x = segmentiser.segmentsX();
	segments_y = segmentiser.segmentsY();
}



void SegmentiserThread::setData( double* x_data_, double* y_data_, double data_size_, double segments_ )
{
	x_data = x_data_;
	y_data = y_data_;
	data_size = data_size_;
	segments = segments_;
	Q_ASSERT( data_set = true );
}



double* SegmentiserThread::segmentsX()
{
	return segments_x;
}



double* SegmentiserThread::segmentsY()
{
	return segments_y;
}
