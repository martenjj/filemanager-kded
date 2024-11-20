//////////////////////////////////////////////////////////////////////////
//									//
//  Project:	File Manager Redirector					//
//  Edit:	20-Nov-24						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Copyright (c) 2021-2024 Jonathan Marten <jjm@keelhaul.me.uk>	//
//  Home page:  https://github.com/martenjj/filemanager-kded/		//
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

#ifndef FILEMANAGERDBUS_H
#define FILEMANAGERDBUS_H

#include <QObject>
#include <QDBusContext>


class FileManagerRedirectorDBus : public QObject, protected QDBusContext
{
    Q_OBJECT

public:
    explicit FileManagerRedirectorDBus(QObject *pnt = nullptr);
    virtual ~FileManagerRedirectorDBus() override = default;

    bool connectToBus(const QString &service, const QString &path);
    void disconnectFromBus();

    /**
     * DBus method for reconfiguring the KDED module
     */
    void reconfigure();

    /**
     * DBus methods for performing file manager actions
     */
    void ShowFolders(const QStringList& uriList, const QString& startUpId);
    void ShowItems(const QStringList& uriList, const QString& startUpId);
    void ShowItemProperties(const QStringList& uriList, const QString& startUpId);

Q_SIGNALS:
    /**
     * This signal is emitted on a reconfigure() request.
     */
    void reconfigured();

private:
    QString m_service;
};

#endif // FILEMANAGERDBUS_H
