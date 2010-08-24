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
#include <QtGui/QMouseEvent>
#include <QtGui/QToolTip>
#include <QtGui/QApplication>

#include "PlotView.h"

#include <qwt_legend.h>
#include <qwt_legend_item.h>
#include <qwt_plot_canvas.h>

#include "Segmentiser.h"

PlotCurve::PlotCurve( int _numPoints, const QString & _title,
										const QColor & _color ) :
	QwtPlotCurve( _title ),
	m_numPoints( _numPoints ),
	m_yData( new double[m_numPoints] ),
	m_xAxisMin( 0 ),
	m_xAxisMax( 0 )
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

	m_xAxisMin = _other.m_xAxisMin;
	m_xAxisMax = _other.m_xAxisMax;

	return * this;
}




PlotCurve::~PlotCurve()
{
	delete[] m_yData;
}




void PlotCurve::attachData(PlotView* _plotView, double* _xData, double* _yData)
{
	delete [] m_yData;
	m_yData = _yData;

	attachData( _plotView, _xData );
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

	// save min and max values for x axis
	m_xAxisMin = _xData[0];
	m_xAxisMax = _xData[m_numPoints-1];
}



/**
 *  Set \b xAxisMin and \b xAxis max to the values after zooming.
 *  \param factor zoom factor
 *  \param centre centre of the zoom
 */
void PlotCurve::xAxisZoomBy(double factor, double centre)
{
	// calculate new width of the plot
	double distance = ( m_xAxisMax - m_xAxisMin ) * factor;

	if ( distance >  x( m_numPoints - 1 ) -  x( 0 ) )
	{
		m_xAxisMin = x( 0 );
		m_xAxisMax = x( m_numPoints - 1 );
		return;
	}

	m_xAxisMin = centre - distance / 2;
	m_xAxisMax = centre + distance / 2;

	if ( m_xAxisMax > x( m_numPoints - 1 ) )
	{
		xAxisPanBy( x( m_numPoints - 1 ) - m_xAxisMax);
		return;
	}

	if ( m_xAxisMin < 0 )
	{
		xAxisPanBy( -m_xAxisMin );
		return;
	}
}




/**
 *  Set \b xAxisMin and \b xAxis max to the values after moving the presented section.
 *  \param s distance to pan the plot
 */
void PlotCurve::xAxisPanBy(double s)
{
	m_xAxisMin += s;
	m_xAxisMax += s;
}




PlotView::PlotView( QWidget * _parent ) :
	QwtPlot( _parent ),
	m_curves(),
	m_numPoints( 0 ),
	m_xData( NULL ),
	m_trackPoints( NULL ),
	m_curveViewMode( CurveViewModeContinuous )
{
	enableAxis( yRight );
	canvas()->setLineWidth( 0 );
	canvas()->installEventFilter( this );
	canvas()->setMouseTracking( true );

	QwtLegend * legend = new QwtLegend;
	legend->setDisplayPolicy( QwtLegend::FixedIdentifier,
								QwtLegendItem::ShowText );
	legend->setStyleSheet( "font-weight:bold;" );
	legend->setFrameStyle( QFrame::NoFrame );
	insertLegend( legend, QwtPlot::RightLegend );

	connect( this, SIGNAL( turnedWheel( double, double ) ), this, SLOT( zoom( double, double ) ) );
}




void PlotView::showRoute( const Route & _route )
{
	m_numPoints = 0;
	foreach( const TrackSegment & seg, _route )
	{
		m_numPoints += seg.size()-1;
	}

	delete[] m_trackPoints;
	delete[] m_xData;
	m_trackPoints = new const TrackPoint *[m_numPoints];
	m_xData = new double[m_numPoints];

	// TODO: check if we might have to delete the previous contents
	m_curves[Elevation] = PlotCurve( m_numPoints, tr( "Elevation" ), Qt::darkBlue );
	m_curves[Speed] = PlotCurve( m_numPoints, tr( "Speed" ), QColor( 0, 160, 0 ) );

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
				m_xData[pointCount] = length;
				m_trackPoints[pointCount] = &pt;
				m_curves[Elevation].data()[pointCount] = pt.elevation();
				speed = (dist*1000 / secs);
				m_curves[Speed].data()[pointCount] =
											qRound( speed*3.6 * 100 ) / 100.0;
				++pointCount;
			}
			lastPoint = pt;
		}
	}

	smoothSpeed( 1.2 );
	createSegmentedCurve( 20 );

	m_curves[Elevation].attachData( this, m_xData );
	m_curves[Speed].attachData( this, m_xData );

	m_curves[SegmentedElevation].setYAxis( 0 );
	m_curves[Elevation].setYAxis( 0 );
	m_curves[Speed].setYAxis( 1 );

	setAxisScale( xBottom, 0, length );
	hideUnneededCurves();
	replot();
}



/**
 *  Create the segmented elevation curve.
 */
void PlotView::createSegmentedCurve(int segments)
{
	m_curves[SegmentedElevation] = PlotCurve( segments, tr( "Elevation" ), Qt::darkBlue );
	double* x_data = m_xData;
	double* y_data = m_curves[Elevation].data();

	Segmentiser segmentiser( x_data, y_data, m_numPoints );
	segmentiser.segmentise( segments );

	m_curves[SegmentedElevation].attachData( this, segmentiser.segmentsX(), segmentiser.segmentsY() );
}




/**
 *  Smooth the speed curve.
 * The speed curve is smoothed by calculating a weighted arithmetic mean from each track point's surrounding
 * points with the bell curve as weights.
 * \param sigma standard deviation of the bell curve.
 */
void PlotView::smoothSpeed(double sigma)
{
	// smooth it after the bell curve: f(x) = 1∕(σ * √(2π)) * exp(-½((x - µ)∕σ)²); µ: mean, σ: standard deviation

	double c_[5];        // we want to smooth with 5 values
	double* c = &c_[2];  // we want the array to run -2...+2
	double c_sum = 0;

	for (int x = -2; x <= 2; x++)
	{
		c[x] = 1 / ( sigma * sqrt( 2 * M_PI ) ) * exp( -0.5 * pow( x / sigma, 2));
		c_sum += c[x];
	}

	// sum of all coefficients shall be one
	for (int x = -2; x <= 2; x++)
	{
		c[x] /= c_sum;
	}

	int last = m_numPoints;
	double * data = m_curves[Speed].data();

	// TODO: the first and the last two points are not smoothed
	// do the actual bell curve smoothing
	for ( int i = 2; i < last - 2; i++ )
	{
		double speed_i = 0;
		for ( int x = -2; x <= 2; x++ )
		{
			speed_i += c[x] * data[i+x];
		}

		data[i] = speed_i;
	}
}




bool PlotView::eventFilter( QObject * _obj, QEvent * _event )
{
	if( _obj != (QObject *) canvas() )
	{
		return false;
	}

	switch( _event->type() )
	{
		case QEvent::MouseMove:
		{
			const double x = invTransform( xBottom,
									((QMouseEvent *)_event)->pos().x() );
			for( int i = 0; i < m_numPoints; ++i )
			{
				if( m_xData[i] >= x )
				{
					emit clickedPoint( m_trackPoints[i]->latitude(),
										m_trackPoints[i]->longitude() );
					QToolTip::showText( QCursor::pos(),
						QString( tr( "at %1 km\nspeed: %2 km/h\nelevation: %3 m" ) ).
									arg( qRound( x*100 ) / 100.0 ).
									arg( m_curves[Speed].y( i ) ).
									arg( m_curves[Elevation].y( i ) ),
									this );
					break;
				}
			}
			return true;
		}
		case QEvent::Wheel:
		{
			QWheelEvent * e = (QWheelEvent *) _event;

			// 15° normally is one step
			// positive: forwads/away from the user, negative: backwards/towards the user
			const double rotated = e->delta() / 360.0;
			const double x = invTransform( xBottom, e->x() );

			emit turnedWheel( rotated, x );
		}
		default:
			break;
	}

	return QObject::eventFilter( _obj, _event );
}




/**
 *  Zoom into speed/elevation plot.
 *  \param amount zoom factor
 *  \param centre centre of zoom
 */
void PlotView::zoom(double amount, double centre)
{
	double zoomFactor = amount > 0 ? 0.5 : 2;
	for( CurveMap::Iterator it = m_curves.begin(); it != m_curves.end(); ++it )
	{
		PlotCurve& curve = it.value();
		curve.xAxisZoomBy( zoomFactor, centre );
		setAxisScale( curve.xAxis(), curve.xAxisMin(), curve.xAxisMax() );
	}

	replot();
}



/**
 *  Set the mode in which the elevation curve is drawn (continuous/segmented).
 *  \param mode desired mode (corresponds to \b CurveViewMode enum).
 */
void PlotView::changeCurveViewMode( int mode )
{
	Q_ASSERT( mode >= 0 && mode < (int) CurveViewModesNumberOf );
	m_curveViewMode = (CurveViewMode) mode;
	hideUnneededCurves();
}



void PlotView::hideUnneededCurves()
{
	switch ( m_curveViewMode )
	{
		case CurveViewModeContinuous:
			m_curves[SegmentedElevation].setStyle( QwtPlotCurve::NoCurve );
			m_curves[Elevation].setStyle( QwtPlotCurve::Lines );
			break;
		case CurveViewModeSegmented:
			m_curves[Elevation].setStyle( QwtPlotCurve::NoCurve );
			m_curves[SegmentedElevation].setStyle( QwtPlotCurve::Lines );
			break;
		default:
			Q_ASSERT( false );
	}
	replot();
}
