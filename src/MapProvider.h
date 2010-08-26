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
#include <QtWebKit/QWebFrame>

#include "Route.h"

class QWebFrame;

class MapProvider : public QObject
{
	Q_OBJECT
public:
	MapProvider( QWebFrame * _parent, const QString & name );
	virtual ~MapProvider();

	// pure virtual functions to be implemented by subclasses
	virtual void showRoute( const Route & _route ) = 0;
	virtual void highlightPoint( double _lat, double _lon ) = 0;

	virtual QUrl mapUrl() const = 0;

	const QString & name() const;
	static QString publicName()
	{
		return "Invalid";
	}


public slots:
	virtual void selectPoint( double _lat, double _lon );


protected:
	/**
	 *  Evaluate a JavaScript statement in the map frame.
	 *  Attention: The function may return before the execution of the JavaScript statement is finished.
	 *  \param scriptSource JavaScript code to be executed.
	 */
	inline QVariant evalJS( const QString & scriptSource )
	{
		return webFrame()->evaluateJavaScript( scriptSource );
	}

	QWebFrame * webFrame()
	{
		return m_webFrame;
	}


private:
	QString m_name;
	QWebFrame * m_webFrame;


signals:
	void clickedPoint( double _lat, double _lon );

} ;


#endif // _MAP_PROVIDER_H
