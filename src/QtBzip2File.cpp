/*
 * QtBzip2File.cpp - implementation of QtBzip2File class
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

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "QtBzip2File.h"

#include "bzlib.h"

#include <cstdio>


static const int BUFSIZE = 1024*256;	// 256 KB per chunk


QtBzip2File::QtBzip2File( const QString & _bz2File ) :
	QObject(),
	m_bz2File( _bz2File )
{
}




QtBzip2File::~QtBzip2File()
{
}




void QtBzip2File::extractTo( const QString & _outputFileName )
{
	int err;
	FILE * bzFileHandle = fopen( m_bz2File.toUtf8().constData(), "rb" );
	BZFILE * bzFile = BZ2_bzReadOpen( &err, bzFileHandle, 0, 0, NULL, 0 );

	const int bz2FileSize = QFileInfo( m_bz2File ).size();

	QFile outputFile( _outputFileName );
	outputFile.open( QFile::WriteOnly );

	while( err == BZ_OK )
	{
		char buf[BUFSIZE];
		int ret = BZ2_bzRead( &err, bzFile, buf, BUFSIZE );
		if( err == BZ_OK || err == BZ_STREAM_END )
		{
			outputFile.write( buf, ret );
		}
		if( err == BZ_STREAM_END )
		{
			void * tmp;
			int unused;
			BZ2_bzReadGetUnused( &err, bzFile, &tmp, &unused );
			outputFile.write( (const char *) tmp, unused );
			break;
		}

		emit progressChanged( ftell( bzFileHandle ) * 100 / bz2FileSize );
		QCoreApplication::processEvents( QEventLoop::AllEvents, 10 );
	}

	BZ2_bzReadClose( &err, bzFile );
	fclose( bzFileHandle );
}



void QtBzip2File::compressFrom( const QString & _inputFileName )
{
	int err;
	FILE * bzFileHandle = fopen( m_bz2File.toUtf8().constData(), "wb" );
	BZFILE * bzFile = BZ2_bzWriteOpen( &err, bzFileHandle, 9, 0, 5 );

	QFile inputFile( _inputFileName );
	inputFile.open( QFile::ReadOnly );

	while( err == BZ_OK && !inputFile.atEnd() )
	{
		char buf[BUFSIZE];
		const int numBytes = qMin<int>( BUFSIZE, inputFile.bytesAvailable() );
		inputFile.read( buf, numBytes );
		BZ2_bzWrite( &err, bzFile, buf, numBytes );

		emit progressChanged( inputFile.pos() * 100 / inputFile.size() );
		QCoreApplication::processEvents( QEventLoop::AllEvents, 10 );
	}

	BZ2_bzWriteClose( &err, bzFile, 0, NULL, NULL );
	fclose( bzFileHandle );
}

