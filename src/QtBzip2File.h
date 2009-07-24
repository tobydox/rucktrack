/*
 * QtBzip2File.h - header file for QtBzip2File class
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

#ifndef _QT_BZIP2_FILE_H
#define _QT_BZIP2_FILE_H

#include <QtCore/QObject>
#include <QtCore/QString>

class QtBzip2File : public QObject
{
	Q_OBJECT
public:
	QtBzip2File( const QString & _bz2File );
	~QtBzip2File();

	void extractTo( const QString & _outputFileName );
	void compressFrom( const QString & _inputFileName );


private:
	const QString m_bz2File;


signals:
	void progressChanged( int );

} ;

#endif // _QT_BZIP2_FILE_H
