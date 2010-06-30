/*
 * GoogleMapsProvider.h - a MapProvider for GoogleMaps
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

#ifndef _GOOGLE_MAPS_PROVIDER_H
#define _GOOGLE_MAPS_PROVIDER_H

#include "MapProvider.h"


class GoogleMapsProvider : public MapProvider
{
public:
	GoogleMapsProvider( QWebFrame * _parent, QString name = GoogleMapsProvider::publicName() );
	virtual ~GoogleMapsProvider();

	virtual void showRoute( const Route & _route );
	virtual void highlightPoint( double _lat, double _lon );

	virtual QUrl mapUrl() const;

	static QString publicName()
	{
		return "GoogleMaps";
	}


} ;


#endif // _GOOGLE_MAPS_PROVIDER_H
