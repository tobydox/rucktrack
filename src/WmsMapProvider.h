/*
 * WmsMapProvider.cpp - a MapProvider for Web Map Services
 *
 * Copyright © 2010 Jens Lang <jenslang/at/users.sourceforge.net>
 *
 * This file is part of RuckTrack - http://rucktrack.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation;
 * version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING).
 *
 */
#ifndef _WMS_MAP_PROVIDER_H
#define _WMS_MAP_PROVIDER_H

#include "MapProvider.h"

/**
 * MapProvider for Web Map Services, http://www.opengeospatial.org/standards/wms .
 */
class WmsMapProvider : public MapProvider
{
	Q_OBJECT
public:
	WmsMapProvider( QWebEnginePage * _parent );
	virtual ~WmsMapProvider();

	virtual void showRoute( const Route & _route );
	virtual void highlightPoint( double _lat, double _lon );

	virtual QUrl mapUrl() const;

	static QString publicName()
	{
		return "Web Map Service";
	}

private slots:
	void webFrameLoadFinished();
} ;


#endif // _WMS_MAP_PROVIDER_H
