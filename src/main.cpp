/*
 * main.cpp - implementation of main() function
 *
 * Copyright (c) 2009-2011 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#include <QtCore/QDebug>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#include <QApplication>

#include "RTMainWindow.h"
#include "SrtmLayer.h"


static void loadTranslations()
{
	const QString loc = QLocale::system().name();

    QTranslator *tr = new QTranslator;
    tr->load( QString( ":/resources/%1.qm" ).arg( loc ) );
    QCoreApplication::installTranslator( tr );

    QTranslator *qtTr = new QTranslator;
#ifdef QT_TRANSLATIONS_DIR
    qtTr->load( QString( "qt_%1.qm" ).arg( loc ), QT_TRANSLATIONS_DIR );
#else
    qtTr->load( QString( ":/qt_%1.qm" ).arg( loc ) );
#endif
    QCoreApplication::installTranslator( qtTr );
}




int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QCoreApplication::setOrganizationName( "RuckTrack Developers" );
	QCoreApplication::setApplicationName( "RuckTrack" );

#ifndef QT_TRANSLATIONS_DIR
//	Q_INIT_RESOURCE(qt_qm);
#endif

	loadTranslations();

	RTMainWindow w;
	w.show();

	return a.exec();
}
