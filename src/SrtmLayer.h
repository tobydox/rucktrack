/*
 * SrtmLayer.h - header file for SrtmLayer class
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

#ifndef _SRTM_LAYER_H
#define _SRTM_LAYER_H

#include <QtCore/QMap>
#include <QtCore/QString>

#include "ProgressTrackingNetworkAccessManager.h"

class QFile;
class QNetworkReply;
class SrtmTiff;


class SrtmLayer : public QObject
{
	Q_OBJECT
public:
	SrtmLayer();

	static void cleanup();

	bool getElevation( float lat, float lon, float & elev );

	QString cachePath() const;
	QString fastCachePath() const;


private slots:
	void putDownloadData();
	void finishDownload();


private:
	bool downloadSrtmTiff( const QString & filename );
	QString getSrtmFilename( float lat, float lon );

	ProgressTrackingNetworkAccessManager m_netAccMgr;
	QNetworkReply * m_activeDownload;
	QFile * m_targetFile;

	typedef QMap<QString, SrtmTiff *> Cache;
	static Cache s_cache;

} ;

#endif // _SRTM_LAYER_H
