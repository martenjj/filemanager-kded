//////////////////////////////////////////////////////////////////////////
//									//
//  Project:	File Manager Redirector					//
//  Edit:	01-May-24						//
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

#include "filemanager_dbus.h"
#include "filemanagerredirectoradaptor.h"

#include <kservice.h>
#include <kapplicationtrader.h>
#include <kpropertiesdialog.h>
#include <kjobuidelegate.h>
#include <kdialogjobuidelegate.h>

#include <kio/commandlauncherjob.h>


FileManagerRedirectorDBus::FileManagerRedirectorDBus(QObject *pnt)
    : QObject(pnt)
{
}


bool FileManagerRedirectorDBus::connectToBus(const QString &service, const QString &path)
{
    qDebug() << "registering service" << service << "path" << path;

    // Although the name org.freedesktop.FileManager1 is also claimed by Dolphin,
    // it is declared as a DBus activated service by the service file
    // $KDEDIR/share/dbus-1/services/org.kde.dolphin.FileManager1.service to
    // be activated on demand.  If there has been no demand for the service yet
    // then the registration should succeed and the service file ignored from
    // then on.  If the name is already claimed by a running process then just
    // report that and give up on trying to register.

    QDBusConnection conn = QDBusConnection::sessionBus();
    // see https://unix.stackexchange.com/questions/379810/find-out-the-owner-of-a-dbus-service-name
    // and https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-messages
    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.DBus", "/",
                                                      "org.freedesktop.DBus",
                                                      "GetConnectionCredentials");
    QList<QVariant> args;
    args.append(service);
    msg.setArguments(args);

    QDBusMessage reply = conn.call(msg, QDBus::BlockWithGui, 1000);
							// a short timeout is enough
    if (reply.type()==QDBusMessage::ReplyMessage)	// will get reply if already claimed
    {
        qWarning() << "Service" << service << "is already registered on DBus";
        for (const QVariant &var : reply.arguments())
        {
            QDBusArgument arg = var.value<QDBusArgument>();
            if (arg.currentType()==QDBusArgument::MapType)
            {
                QVariantMap map = qdbus_cast<QVariantMap>(arg);
                uint peerPID = map["ProcessID"].toUInt();
                uint peerUID = map["UnixUserID"].toUInt();
                if (peerPID!=0) qWarning() << "Service registered by PID" << peerPID << "user" << peerUID;
            }
        }

        return (false);					// will not be able to connect
    }

    if (!QDBusConnection::sessionBus().registerService(service))
    {
        qWarning() << "registering service failed";
        return (false);
    }

    new FileManagerRedirectorAdaptor(this);
    if (!QDBusConnection::sessionBus().registerObject(path, this))
    {
        qWarning() << "registering object path failed";
        return (false);
    }

    m_service = service;				// remember name for unregistering
    return (true);
}


void FileManagerRedirectorDBus::disconnectFromBus()
{
    qDebug() << "unregistering service" << m_service;
    if (!QDBusConnection::sessionBus().unregisterService(m_service))
    {
        qWarning() << "unregistering service failed";
    }
}


void FileManagerRedirectorDBus::reconfigure()
{
    emit reconfigured();
}


static void startFileManager(const QStringList &args, const QString &startUpId)
{
    KService::Ptr offer = KApplicationTrader::preferredService("inode/directory");
    if (!offer)						// very unlikely
    {
        qWarning() << "No file manager service available";
        return;
    }

    qDebug() << "file manager service" << offer->name();
    QString exe = offer->exec().split(" ").value(0);
    if (exe.isEmpty())
    {
        qWarning() << "Cannot get executable from service";
        return;
    }

    if (exe=="kfmclient") exe = "konqueror";		// client does not support options,
							// so start Konqueror directly
    KIO::CommandLauncherJob *job = new KIO::CommandLauncherJob(exe, args);
    job->setStartupId(startUpId.toLocal8Bit());
    job->setUiDelegate(new KDialogJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, nullptr));
    qDebug() << "executing" << exe << "with args" << args;
    job->start();
}


void FileManagerRedirectorDBus::ShowFolders(const QStringList &uriList, const QString &startUpId)
{
    QStringList args;
    args << "--mimetype" << "inode/directory";
    args.append(uriList);
    startFileManager(args, startUpId);
}


void FileManagerRedirectorDBus::ShowItems(const QStringList &uriList, const QString &startUpId)
{
    QStringList args;
    args << "--select";
    args.append(uriList);
    startFileManager(args, startUpId);
}


void FileManagerRedirectorDBus::ShowItemProperties(const QStringList &uriList, const QString &startUpId)
{
    QList<QUrl> urls;
    for (const QString &u : uriList)			// convert to a list of URLs
    {
        urls.append(QUrl::fromUserInput(u, QDir::currentPath(), QUrl::AssumeLocalFile));
    }

    qDebug() << "opening properties for" << urls;
    KPropertiesDialog::showDialog(urls, nullptr, false);
}
