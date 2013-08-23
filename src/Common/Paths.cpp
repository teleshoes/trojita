/* Copyright (C) 2013 Pali Rohár <pali.rohar@gmail.com>

   This file is part of the Trojita Qt IMAP e-mail client,
   http://trojita.flaska.net/

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3 or any later version
   accepted by the membership of KDE e.V. (or its successor approved
   by the membership of KDE e.V.), which shall act as a proxy
   defined in Section 14 of version 3 of the license.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QCoreApplication>
#include <QDir>
#include <QMap>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

#include "Application.h"
#include "Paths.h"

namespace Common
{

QString writablePath(const LocationType location)
{
    static QMap<LocationType, QString> map;

    if (map.isEmpty()) {

        const QString &origApplicationName = QCoreApplication::applicationName();
        const QString &origApplicationVersion = QCoreApplication::applicationVersion();
        const QString &origOrganizationDomain = QCoreApplication::organizationDomain();
        const QString &origOrganizationName = QCoreApplication::organizationName();

        QCoreApplication::setApplicationName(Common::Application::name);
        QCoreApplication::setApplicationVersion(Common::Application::version);
        QCoreApplication::setOrganizationDomain(Common::Application::organization);
        QCoreApplication::setOrganizationName(Common::Application::organization);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        map[LOCATION_CACHE] = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        map[LOCATION_DATA] = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
        map[LOCATION_DOWNLOAD] = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
#else
        map[LOCATION_CACHE] = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
        map[LOCATION_DATA] = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
        map[LOCATION_DOWNLOAD] = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
#endif

        if (map[LOCATION_CACHE].isEmpty())
            map[LOCATION_CACHE] = QDir::homePath() + QLatin1String("/.cache");

        if (map[LOCATION_DATA].isEmpty())
            map[LOCATION_DATA] = QDir::homePath() + QLatin1String("/.data");

        if (map[LOCATION_DOWNLOAD].isEmpty())
            map[LOCATION_DOWNLOAD] = QDir::homePath();

        if (!map[LOCATION_CACHE].endsWith(QLatin1Char('/')))
            map[LOCATION_CACHE] += QLatin1Char('/');

        if (!map[LOCATION_DATA].endsWith(QLatin1Char('/')))
            map[LOCATION_DATA] += QLatin1Char('/');

        if (!map[LOCATION_DOWNLOAD].endsWith(QLatin1Char('/')))
            map[LOCATION_DOWNLOAD] += QLatin1Char('/');

        QCoreApplication::setApplicationName(origApplicationName);
        QCoreApplication::setApplicationVersion(origApplicationVersion);
        QCoreApplication::setOrganizationDomain(origOrganizationDomain);
        QCoreApplication::setOrganizationName(origOrganizationName);

    }

    return map[location];
}

}