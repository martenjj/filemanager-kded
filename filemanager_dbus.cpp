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
    // TODO: if something is already registered at that service, then
    // tell it to quit and wait for it to do so.

    qDebug() << "registering service" << service << "path" << path;

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

    return (true);
}


void FileManagerRedirectorDBus::reconfigure()
{
    emit reconfigured();
}


static void startFileManager(const QStringList &args, const QString &startUpId)
{
    KService::Ptr offer = KApplicationTrader::preferredService("inode/directory");
    if (!offer)						// very unlikley
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
