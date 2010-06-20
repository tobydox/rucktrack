/*
 * PreferencesDialog.cpp - implementation of PreferencesDialog
 *
 * Copyright (c) 2010 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 *
 * This file is part of Linux MultiMedia Studio - http://lmms.sourceforge.net
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

#include <QtCore/QSettings>
#include <QtGui/QApplication>

#include "PreferencesDialog.h"
#include "GoogleMapsProvider.h"
#include "OpenStreetMapProvider.h"

#include "ui_PreferencesDialog.h"


/**
 *  Constructor for the RuckTrack preferences dialog. Sets up the user
 *  interface and initializes all widgets according to current configuration.
 *  \param
 */
PreferencesDialog::PreferencesDialog() :
	QDialog( QApplication::activeWindow() ),
	ui( new Ui::PreferencesDialog )
{
	ui->setupUi( this );

	ui->mapProviderComboBox->addItem( GoogleMapsProvider::publicName() );
	ui->mapProviderComboBox->addItem( OpenStreetMapProvider::publicName() );

	QSettings s;
	ui->showProgressBar->setChecked( s.value( "UI/ShowProgressBar" ).toBool() );
	ui->mapProviderComboBox->setCurrentIndex(
							ui->mapProviderComboBox->findText(
								s.value( "Maps/MapProvider" ).toString() ) );
}



/**
 *  Called when clicking the "OK" button in the preferences dialog and
 *  saves the configuration.
 *  \param
 */
void PreferencesDialog::accept()
{
	QSettings s;
	s.setValue( "UI/ShowProgressBar", ui->showProgressBar->isChecked() );
	s.setValue( "Maps/MapProvider", ui->mapProviderComboBox->currentText() );

	QDialog::accept();
}

