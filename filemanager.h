//////////////////////////////////////////////////////////////////////////
//									//
//  Project:	File Manager Redirector					//
//  Edit:	19-Aug-21						//
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

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <kdedmodule.h>
#include <QDBusContext>

class FileManagerRedirectorDBus;


class FileManagerRedirectorModule : public KDEDModule, protected QDBusContext
{
    Q_OBJECT

public:
    FileManagerRedirectorModule(QObject *parent, const QVariantList &args);
    virtual ~FileManagerRedirectorModule() override;

private Q_SLOTS:
    /**
     * Reconfigure module
     */
    void reconfigure();

    /**
     * Try to connect to DBus
     */
    void slotConnectToBus();

private:
    FileManagerRedirectorDBus *m_fileManagerDBus;
    int m_retryCount;
};

#endif // FILEMANAGER_H
