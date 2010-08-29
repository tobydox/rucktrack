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
#include <QtGui/QCompleter>
#include <QtGui/QDirModel>
#include <QtGui/QFileDialog>
#include <QtCore/QFileInfo>

#include "PreferencesDialog.h"
#include "GoogleMapsProvider.h"
#include "OpenStreetMapProvider.h"
#include "WmsMapProvider.h"

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

	QCompleter * completer = new QCompleter( this );
	completer->setModel( new QDirModel( completer ) );
	ui->cacheDirectory->setCompleter( completer );
	connect( ui->cacheDirectoryBtn, SIGNAL( clicked() ),
				this, SLOT( browseCacheDirectory() ) );
	connect( ui->gpsBabelExecutableBtn, SIGNAL( clicked() ), this, SLOT( browseGpsBabelExecutable() ) );

	ui->mapProviderComboBox->addItem( GoogleMapsProvider::publicName() );
	ui->mapProviderComboBox->addItem( OpenStreetMapProvider::publicName() );
	ui->mapProviderComboBox->addItem( WmsMapProvider::publicName() );

	QSettings s;
	ui->showProgressBar->setChecked( s.value( "UI/ShowProgressBar", true ).toBool() );
	ui->dropFaultyTrackPoints->setChecked( s.value( "GPX/DropFaultyTrackPoints", true ).toBool() );
	ui->useSystemCacheDirectory->setChecked( s.value( "General/UseSystemCacheDirectory", true ).toBool() );
	ui->cacheDirectory->setText( s.value( "General/CacheDirectory",
			QDir::homePath() + QDir::separator() + ".rucktrack" ).toString() );
	ui->mapProviderComboBox->setCurrentIndex(
							ui->mapProviderComboBox->findText(
								s.value( "Maps/MapProvider",
									GoogleMapsProvider::publicName() ).toString() ) );
	ui->enableCaching->setChecked(
							s.value( "Maps/EnableCaching", true ).toBool() );
	ui->cacheSizeSpinBox->setValue(
							s.value( "Maps/CacheSize", 50 ).toInt() );
	ui->gpsBabelExecutable->setText( s.value( "GPX/GpsBabelExecutable" ).toString() );
	ui->gpsBabelFileImport->setChecked( s.value( "GPX/UseGpsBabel" ).toBool() );
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
	s.setValue( "GPX/DropFaultyTrackPoints", ui->dropFaultyTrackPoints->isChecked() );
	s.setValue( "General/UseSystemCacheDirectory", ui->useSystemCacheDirectory->isChecked() );
	s.setValue( "General/CacheDirectory", ui->cacheDirectory->text() );
	s.setValue( "Maps/MapProvider", ui->mapProviderComboBox->currentText() );
	s.setValue( "Maps/EnableCaching", ui->enableCaching->isChecked() );
	s.setValue( "Maps/CacheSize", ui->cacheSizeSpinBox->value() );
	s.setValue( "GPX/UseGpsBabel", ui->gpsBabelFileImport->isChecked() );
	s.setValue( "GPX/GpsBabelExecutable", ui->gpsBabelExecutable->text() );

	QDialog::accept();
}



/**
 *  Called when clicking the "browse cache directory" button in the preferences
 *  dialog and opens a QFileDialog.
 *  \param
 */
void PreferencesDialog::browseCacheDirectory()
{
	QString d = QFileDialog::getExistingDirectory( this,
					tr( "Select cache directory" ),
					ui->cacheDirectory->text(),
					QFileDialog::ShowDirsOnly );
	if( !d.isEmpty() )
	{
		ui->cacheDirectory->setText( d );
	}
}

/**
 *  Called when clicking the "Browse GPSBabel executable" button in the preferences
 *  dialog and opens a QFileDialog.
 */
void PreferencesDialog::browseGpsBabelExecutable()
{
	QString d = QFileDialog::getOpenFileName( this,
					tr( "Select GPSBabel executable" ),
					QFileInfo( ui->gpsBabelExecutable->text() ).path(),
					tr( "All Files (*)" ) );
	if( !d.isEmpty() )
	{
		ui->gpsBabelExecutable->setText( d );
	}
}
