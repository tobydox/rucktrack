/*
 * RuckTrackNetworkAccessManager.h - header file for
 *                       RuckTrackNetworkAccessManager class
 *
 * Copyright (c) 2009-2010 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef _RUCKTRACK_NETWORK_ACCESS_MANAGER_H
#define _RUCKTRACK_NETWORK_ACCESS_MANAGER_H

#include <QtNetwork/QNetworkAccessManager>


/**
 *  A QNetworkAccessManager subclass that tracks the progress of GET operations
 *  and manages disk caching.
 */
class RuckTrackNetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
public:
	enum CachingPolicies
	{
		DiskCaching,
		NoCaching,
		NumCachingPolicies
	} ;
	typedef CachingPolicies CachingPolicy;

    RuckTrackNetworkAccessManager( QObject * _parent,
									CachingPolicy cachingPolicy = DiskCaching );

    virtual QNetworkReply * createRequest( Operation op,
                                            const QNetworkRequest & req,
                                            QIODevice * outgoingData = 0 );


public slots:
    void updateProgress( qint64 done, qint64 total );


signals:
    void progressChanged( int );

} ;


#endif // _RUCKTRACK_NETWORK_ACCESS_MANAGER_H
