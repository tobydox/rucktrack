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
	Q_ASSERT( !(m_dataSet = false) );
}



void SegmentiserThread::run()
{
	Q_ASSERT( m_dataSet );
	Segmentiser segmentiser( m_xData, m_yData, m_dataSize );
	segmentiser.segmentise( m_segments );
	m_xSegments = segmentiser.segmentsX();
	m_ySegments = segmentiser.segmentsY();
}



void SegmentiserThread::setData( double* x_data_, double* y_data_, double data_size_, double segments_ )
{
	m_xData = x_data_;
	m_yData = y_data_;
	m_dataSize = data_size_;
	m_segments = segments_;
	Q_ASSERT( m_dataSet = true );
}



double* SegmentiserThread::segmentsX()
{
	return m_xSegments;
}



double* SegmentiserThread::segmentsY()
{
	return m_ySegments;
}
