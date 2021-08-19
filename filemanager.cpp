//////////////////////////////////////////////////////////////////////////
//									//
//  Project:	File Manager Redirector					//
//  Edit:	18-Aug-21						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Copyright (c) 2021 Jonathan Marten <jjm@keelhaul.me.uk>		//
//  Home and download page:  http://www.keelhaul.me.uk/TBD/		//
//									//
//  This program is free software; you can redistribute it and/or	//
//  modify it under the terms of the GNU General Public License as	//
//  published by the Free Software Foundation; either version 2 of	//
//  the License, or (at your option) any later version.			//
//									//
//  It is distributed in the hope that it will be useful, but		//
//  WITHOUT ANY WARRANTY; without even the implied warranty of		//
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the	//
//  GNU General Public License for more details.			//
//									//
//  You should have received a copy of the GNU General Public		//
//  License along with this program; see the file COPYING for further	//
//  details.  If not, see <http://www.gnu.org/licenses>.		//
//									//
//////////////////////////////////////////////////////////////////////////

#include "filemanager.h"
#include "filemanager_dbus.h"
#include "filemanagerredirectoradaptor.h"

#include <QDBusInterface>

#include <kpluginfactory.h>
#include <kpluginloader.h>


K_PLUGIN_FACTORY_WITH_JSON(FileManagerRedirectorFactory, "filemanagerredirector.json", registerPlugin<FileManagerRedirectorModule>();)


FileManagerRedirectorModule::FileManagerRedirectorModule(QObject *parent, const QVariantList &args)
  : KDEDModule(parent)
{
    reconfigure();
    m_fileManagerDBus = new FileManagerRedirectorDBus(this);
    m_fileManagerDBus->connectToBus("org.freedesktop.FileManager1", "/org/freedesktop/FileManager1");
    qDebug() << "registered KDED module";
}


FileManagerRedirectorModule::~FileManagerRedirectorModule()
{
    qDebug();
}


// This method is not really used any more,
// but has to be kept for DBus compatibility.
void FileManagerRedirectorModule::reconfigure()
{
}


#include "filemanager.moc"
