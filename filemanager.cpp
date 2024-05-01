//////////////////////////////////////////////////////////////////////////
//									//
//  Project:	File Manager Redirector					//
//  Edit:	30-Apr-24						//
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

#include <qtimer.h>

#include <kpluginfactory.h>
#include <kpluginmetadata.h>


K_PLUGIN_FACTORY_WITH_JSON(FileManagerRedirectorFactory, "filemanagerredirector.json", registerPlugin<FileManagerRedirectorModule>();)


static const int DBUS_RETRY_DELAY = 60;			// connection retry wait in seconds
static const int DBUS_RETRY_COUNT = 5;			// how many times to retry connection


FileManagerRedirectorModule::FileManagerRedirectorModule(QObject *parent, const QVariantList &args)
  : KDEDModule(parent)
{
    m_retryCount = 0;
    reconfigure();
    m_fileManagerDBus = new FileManagerRedirectorDBus(this);
    slotConnectToBus();
    qDebug() << "KDED module activated";
}


FileManagerRedirectorModule::~FileManagerRedirectorModule()
{
    m_fileManagerDBus->disconnectFromBus();
}


// This method is not really used any more,
// but has to be kept for DBus compatibility.
void FileManagerRedirectorModule::reconfigure()
{
}


void FileManagerRedirectorModule::slotConnectToBus()
{
    bool status = m_fileManagerDBus->connectToBus("org.freedesktop.FileManager1", "/org/freedesktop/FileManager1");
    if (status)						// connection succeeded
    {
        qDebug() << "DBus service registered";
        return;
    }

    ++m_retryCount;					// count up failed connection
    if (m_retryCount>DBUS_RETRY_COUNT)			// check if too many times
    {
        // If the failure persists, then the module can be unloaded and
        // the connection retried using the module's "pause" and "start"
        // buttons in System Settings -> Workspace -> Startup and Shutdown ->
        // Background Services.
        qWarning() << "Unable to register DBus service, giving up";
        return;
    }

    qDebug() << "Unable to register DBus service, trying again in" << DBUS_RETRY_DELAY << "seconds";
    QTimer::singleShot(DBUS_RETRY_DELAY*1000, this, &FileManagerRedirectorModule::slotConnectToBus);
}


#include "filemanager.moc"
