/*
 * PlotView.h - header file for PlotView class
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

#ifndef _PLOT_VIEW_H
#define _PLOT_VIEW_H

#include <QtCore/QMap>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

#include "Route.h"

class PlotView;

class PlotCurve : public QwtPlotCurve
{
public:
	PlotCurve( int _numPoints = 0, const QString & _title = QString(),
				const QColor & _color = Qt::black );
	PlotCurve( const PlotCurve & _other ) :
		QwtPlotCurve()
	{
		*this = _other;
		setRenderHint( QwtPlotItem::RenderAntialiased );
	}
	PlotCurve & operator=( const PlotCurve & _other );
	~PlotCurve();

	void attachData( PlotView * _plotView, double * _xData );
	void attachData( PlotView * _plotView, double * _xData, double * _yData );

	void xAxisZoomBy( double factor, double centre );
	void xAxisPanBy( double s );

	inline double * data()
	{
		return m_yData;
	}

	inline double xAxisMin()
	{
		return m_xAxisMin;
	}
	inline double xAxisMax()
	{
		return m_xAxisMax;
	}

private:
	int m_numPoints;
	double * m_yData;
	double m_xAxisMin;
	double m_xAxisMax;
} ;




class PlotView : public QwtPlot
{
	Q_OBJECT
public:
	enum Curves
	{
		Elevation,
		Speed,
		Time,
		Climb,
		SegmentedElevation,
		NumCurves
	} ;

	PlotView( QWidget * _parent );

	void showRoute( const Route & _route );

	virtual bool eventFilter( QObject * _obj, QEvent * _event );

	enum DisplayType {DISPLAY_CONTINUOUS, DISPLAY_SEGMENTED};

private:
	typedef QMap<Curves, PlotCurve> CurveMap;
	void smoothSpeed(double sigma);
	CurveMap m_curves;
	int m_numPoints;
	double * m_xData;
	const TrackPoint * * m_trackPoints;

	void createSegmentedCurve( int );


signals:
	void clickedPoint( double, double );
	void turnedWheel( double, double );

public slots:
	void zoom( double amount, double x );
// 	void changeDisplay( DisplayType );
} ;

#endif // _PLOT_VIEW_H
