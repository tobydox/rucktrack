/*
 * MapProvider.cpp - implementation of abstrat MapProvider class
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

#include "MapProvider.h"


MapProvider::MapProvider( QWebEnginePage * _parent, const QString & name ) :
	QObject( _parent ),
	m_name( name ),
	m_webFrame( _parent )
{
}




MapProvider::~MapProvider()
{
}




/**
 *  Return the internal name of the MapProvider class.
 */
const QString & MapProvider::name() const
{
	return m_name;
}




void MapProvider::selectPoint( double _lat, double _lon )
{
	emit clickedPoint( _lat, _lon );
}

