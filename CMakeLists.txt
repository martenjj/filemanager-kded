##########################################################################
##									##
##  Project:	File Manager Redirector					##
##  Edit:	19-Aug-21						##
##									##
##########################################################################
##									##
##  Copyright (c) 2021 Jonathan Marten <jjm@keelhaul.me.uk>		##
##  Home and download page:  http://www.keelhaul.me.uk/TBD/		##
##									##
##  This program is free software; you can redistribute it and/or	##
##  modify it under the terms of the GNU General Public License as	##
##  published by the Free Software Foundation; either version 2 of	##
##  the License, or (at your option) any later version.			##
##									##
##  It is distributed in the hope that it will be useful, but		##
##  WITHOUT ANY WARRANTY; without even the implied warranty of		##
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the	##
##  GNU General Public License for more details.			##
##									##
##  You should have received a copy of the GNU General Public		##
##  License along with this program; see the file COPYING for further	##
##  details.  If not, see <http://www.gnu.org/licenses>.		##
##									##
##########################################################################

cmake_minimum_required(VERSION 3.16)

project(filemanagerredirector)
set(PROJECT_VERSION "5.22.80")
set(PROJECT_VERSION_MAJOR 5)

set(QT_MIN_VERSION "5.15.0")
set(KF5_MIN_VERSION "5.82")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(CMakePackageConfigHelpers)
include(CheckIncludeFiles)
include(FeatureSummary)
include(ECMQtDeclareLoggingCategory)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core DBus)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS CoreAddons I18n DBusAddons KIO Service)
find_package(KDED CONFIG REQUIRED)

set(kded_filemanager_SRCS
    filemanager.cpp
    filemanager_dbus.cpp)

qt_add_dbus_adaptor(kded_filemanager_SRCS org.kde.filemanagerredirector.xml
    filemanager_dbus.h FileManagerRedirectorDBus
    filemanagerredirectoradaptor FileManagerRedirectorAdaptor)

kcoreaddons_add_plugin(filemanagerredirector SOURCES ${kded_filemanager_SRCS} INSTALL_NAMESPACE "kf5/kded")
kcoreaddons_desktop_to_json(filemanagerredirector filemanagerredirector.desktop)

target_link_libraries(filemanagerredirector
    Qt::DBus
    KF5::DBusAddons
    KF5::KIOCore
    KF5::KIOWidgets)

# The shared library is installed by kcoreaddons_add_plugin().
install(FILES org.kde.filemanagerredirector.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})
