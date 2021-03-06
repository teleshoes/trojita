/* Copyright (C) 2006 - 2015 Jan Kundrát <jkt@kde.org>

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

#include "IconLoader.h"
#include <QFileInfo>
#include <QHash>

namespace UiUtils {

/** @short Wrapper around the QIcon::fromTheme with sane fallback
 *
 * The issue with the QIcon::fromTheme is that it does not really work on non-X11
 * platforms, unless you ship your own theme index and specify your theme name.
 * In Trojitá, we do not really want to hardcode anything, because the
 * preference is to use whatever is available from the current theme, but *also*
 * provide an icon as a fallback. And that is exactly why this function is here.
 *
 * It is implemented inline in order to prevent a dependency from the Imap lib
 * into the Gui lib.
 * */
QIcon loadIcon(const QString &name)
{
    static QHash<QString, QIcon> iconDict;
    QHash<QString, QIcon>::const_iterator it = iconDict.constFind(name);
    if (it != iconDict.constEnd())
        return *it;

    auto overrideIcon = QString(QLatin1String(":/icons/%1/%2.svg")).arg(QIcon::themeName(), name);
    if (QFileInfo(overrideIcon).exists()) {
        QIcon icon(overrideIcon);
        iconDict.insert(name, icon);
        return icon;
    }

    QString iconInTheme = name;
    if (QIcon::themeName().toLower() == QLatin1String("breeze")) {
        if (name == QLatin1String("mail-flagged")) {
            iconInTheme = QLatin1String("flag-yellow");
        } else if (name == QLatin1String("folder-bookmark")) {
            iconInTheme = QLatin1String("folder-blue");
        } else if (name == QLatin1String("folder-open")) {
            iconInTheme = QLatin1String("folder");
        } else if (name == QLatin1String("mail-read")) {
            iconInTheme = QLatin1String("mail-mark-read");
        } else if (name == QLatin1String("mail-unread")) {
            iconInTheme = QLatin1String("mail-mark-unread");
        }
    }

    // A QIcon(QString) constructor creates non-null QIcons, even though the resulting icon will
    // not ever return a valid and usable pixmap. This means that we have to actually look at the
    // icon's pixmap to find out what to return.
    // If we do not do that, the GUI shows empty pixmaps instead of a text fallback, which is
    // clearly suboptimal.
    QIcon res = QIcon::fromTheme(iconInTheme, QIcon(QString::fromUtf8(":/icons/%1").arg(name)));
    if (res.pixmap(QSize(16, 16)).isNull()) {
        res = QIcon();
    }
    iconDict.insert(name, res);
    return res;
}

}
