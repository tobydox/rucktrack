/*
 * PlotView.cpp - implementation of PlotView class
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

#include <QtCore/QDebug>

#include "PlotView.h"

#include "qwt_legend.h"
#include "qwt_legend_item.h"
#include "qwt_plot_canvas.h"


PlotCurve::PlotCurve( int _numPoints, const QString & _title,
										const QColor & _color ) :
	QwtPlotCurve( _title ),
	m_numPoints( _numPoints ),
	m_yData( new double[m_numPoints] )
{
	QColor c = _color;
	c.setAlpha( 176 );

	setPen( c );
	setBrush( c );
}




PlotCurve & PlotCurve::operator=( const PlotCurve & _other )
{
	setTitle( _other.title() );
	m_numPoints = _other.m_numPoints;
	m_yData = new double[m_numPoints];

	for( int i = 0; i < m_numPoints; ++i )
	{
		m_yData[i] = _other.m_yData[i];
	}

	setPen( _other.pen() );
	setBrush( _other.brush() );

	return * this;
}




PlotCurve::~PlotCurve()
{
	delete[] m_yData;
}




void PlotCurve::attachData( PlotView * _plotView, double * _xData )
{
	setData( _xData, m_yData, m_numPoints );
	attach( _plotView );
	setBaseline( 0 );
	delete[] m_yData;
	m_yData = NULL;

	// setup our legend item
	QWidget * w = _plotView->legend()->find( this );
	QPalette pal = w->palette();
	pal.setColor( QPalette::Text, pen().color() );
	w->setPalette( pal );
}





PlotView::PlotView( QWidget * _parent ) :
	QwtPlot( _parent )
{
	enableAxis( yRight );
	canvas()->setLineWidth( 0 );

	QwtLegend * legend = new QwtLegend;
	legend->setDisplayPolicy( QwtLegend::FixedIdentifier,
								QwtLegendItem::ShowText );
	legend->setStyleSheet( "font-weight:bold;" );
	legend->setFrameStyle( QFrame::NoFrame );
	insertLegend( legend, QwtPlot::RightLegend );

}




void PlotView::showRoute( const Route & _route )
{
	int numPoints = 0;
	foreach( const TrackSegment & seg, _route )
	{
		numPoints += seg.size()-1;
	}
	double * xData = new double[numPoints];

	m_curves[Elevation] = PlotCurve( numPoints, "Elevation", Qt::blue );
	m_curves[Speed] = PlotCurve( numPoints, "Speed", QColor( 0, 160, 0 ) );


	double length = 0;
	double elevGain = 0;
	double elevLoss = 0;
	double timeSecs = 0;
	double speed = 0;
	double slope = 0;
	int pointCount = 0;

	foreach( const TrackSegment & seg, _route )
	{
		bool first = true;
		TrackPoint lastPoint;
		foreach( const TrackPoint & pt, seg )
		{
			if( first )
			{
				first = false;
			}
			else
			{
				const double secs = lastPoint.secsTo( pt );
				const double dist = lastPoint.distanceTo( pt );
				timeSecs += secs;
				length += dist;
				const double dElev = pt.elevation() -
							lastPoint.elevation();
				if( dElev < 0 )
				{
					elevLoss += -dElev;
				}
				else
				{
					elevGain += dElev;
				}

				if( dist > 0 )
				{
					slope = slope*0.6 + dElev / ( dist * 10 ) * 0.4;
				}
				// smooth speed a bit
				speed = speed*0.6 + 0.4*(dist*1000 / secs);
				xData[pointCount] = length;
				m_curves[Elevation].data()[pointCount] = pt.elevation();
				m_curves[Speed].data()[pointCount] = speed*3.6;
				++pointCount;
			}
			lastPoint = pt;
		}
	}

	for( CurveMap::Iterator it = m_curves.begin(); it != m_curves.end(); ++it )
	{
		it.value().attachData( this, xData );
	}

	m_curves[Elevation].setYAxis( 0 );
	m_curves[Speed].setYAxis( 1 );


	delete[] xData;

	setAxisScale( xBottom, 0, length );
	replot();
}



