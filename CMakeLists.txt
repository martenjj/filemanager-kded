##########################################################################
##									##
##  Project:	File Manager Redirector					##
##  Edit:	20-Nov-24						##
##									##
##########################################################################
##									##
##  Copyright (c) 2021-2024 Jonathan Marten <jjm@keelhaul.me.uk>	##
##  Home page:  https://github.com/martenjj/filemanager-kded/		##
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

if (NOT DEFINED QT_MAJOR_VERSION)
  set (QT_MAJOR_VERSION 6)
endif ()

if (QT_MAJOR_VERSION EQUAL 6)
  set(QT_MIN_VERSION "6.3.0")
  set(KF_MIN_VERSION "6.1.0")
else ()
  if (QT_MAJOR_VERSION EQUAL 5)
    set(QT_MIN_VERSION "5.15.0")
    set(KF_MIN_VERSION "5.82.0")
  else ()
    message(FATAL_ERROR "Expecting to build for either Qt5 or Qt6")
  endif ()
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(CMakePackageConfigHelpers)
include(CheckIncludeFiles)
include(FeatureSummary)
include(ECMQtDeclareLoggingCategory)

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core DBus)
find_package(KF${QT_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS CoreAddons I18n DBusAddons KIO Service)
find_package(KF${QT_MAJOR_VERSION}KDED CONFIG REQUIRED)

remove_definitions("-DQT_NO_CAST_FROM_ASCII")
remove_definitions("-DQT_NO_CAST_FROM_BYTEARRAY")
remove_definitions("-DQT_NO_KEYWORDS")

set(kded_filemanager_SRCS
    filemanager.cpp
    filemanager_dbus.cpp)

qt_add_dbus_adaptor(kded_filemanager_SRCS org.kde.filemanagerredirector.xml
    filemanager_dbus.h FileManagerRedirectorDBus
    filemanagerredirectoradaptor FileManagerRedirectorAdaptor)

kcoreaddons_add_plugin(filemanagerredirector SOURCES ${kded_filemanager_SRCS} INSTALL_NAMESPACE "kf${QT_MAJOR_VERSION}/kded")

target_link_libraries(filemanagerredirector
    Qt::DBus
    KF${QT_MAJOR_VERSION}::DBusAddons
    KF${QT_MAJOR_VERSION}::KIOCore
    KF${QT_MAJOR_VERSION}::KIOWidgets)

# The shared library is installed by kcoreaddons_add_plugin().
install(FILES org.kde.filemanagerredirector.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})
