/*
 * MapProvider.h - header file for MapProvider class
 *
 * Copyright (c) 2010 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef _MAP_PROVIDER_H
#define _MAP_PROVIDER_H

#include <QtCore/QObject>
#include <QtCore/QUrl>

#include "Route.h"

class QWebFrame;

class MapProvider : public QObject
{
	Q_OBJECT
public:
	MapProvider( QWebFrame * _parent );
	virtual ~MapProvider();

	// pure virtual functions to be implemented by subclasses
	virtual void showRoute( const Route & _route ) = 0;
	virtual void highlightPoint( double _lat, double _lon ) = 0;

	virtual QUrl mapUrl() const = 0;


public slots:
	virtual void selectPoint( double _lat, double _lon );


protected:
	QWebFrame * webFrame()
	{
		return m_webFrame;
	}


private:
	QWebFrame * m_webFrame;


signals:
	void clickedPoint( double _lat, double _lon );

} ;


#endif // _MAP_PROVIDER_H