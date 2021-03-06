/* Copyright (C) 2006 - 2015 Jan Kundrát <jkt@kde.org>
   Copyright (C) 2013 - 2015 Pali Rohár <pali.rohar@gmail.com>

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
#include "OneEnvelopeAddress.h"
#include <QHeaderView>
#include <QFontMetrics>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#  include <QUrl>
#  include <QTextDocument>
#else
#  include <QUrlQuery>
#endif
#include "Gui/MessageView.h"
#include "Gui/Util.h"
#include "Plugins/PluginManager.h"
#include "Plugins/AddressbookPlugin.h"

namespace Gui {

OneEnvelopeAddress::OneEnvelopeAddress(QWidget *parent, const Imap::Message::MailAddress &address, MessageView *messageView, const Position lastOneInRow):
    QLabel(parent), m_address(address), m_lastOneInRow(lastOneInRow), m_pluginManager(messageView->pluginManager())
{
    setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse);
    setIndent(5);
    connect(this, SIGNAL(linkHovered(QString)), this, SLOT(onLinkHovered(QString)));

    QFontMetrics fm(font());
    int iconSize = fm.boundingRect(QLatin1Char('M')).height();
    contactKnownUrl = Gui::Util::resizedImageAsDataUrl(QLatin1String(":/icons/contact-known.png"), iconSize);
    contactUnknownUrl = Gui::Util::resizedImageAsDataUrl(QLatin1String(":/icons/contact-unknown.png"), iconSize);

    connect(this, SIGNAL(linkActivated(QString)), messageView, SLOT(headerLinkActivated(QString)));

    processAddress();
}

void OneEnvelopeAddress::processAddress()
{
    // show fast version without data from addressbook now
    finishProcessAddress(QStringList());

    // and later if there is an addressbook and all addressbook jobs have finished, show the full version
    Plugins::AddressbookPlugin *addressbook = m_pluginManager->addressbook();
    if (!addressbook || !(addressbook->features() & Plugins::AddressbookPlugin::FeaturePrettyNames))
        return;

    Plugins::AddressbookJob *job = addressbook->requestPrettyNamesForAddress(m_address.mailbox + QLatin1Char('@') + m_address.host);
    if (!job)
        return;

    connect(job, SIGNAL(prettyNamesForAddressAvailable(QStringList)), this, SLOT(finishProcessAddress(QStringList)));
    job->setAutoDelete(true);
    job->start();
}

void OneEnvelopeAddress::finishProcessAddress(const QStringList &matchingDisplayNames)
{
    QUrl url = m_address.asUrl();
    QString icon = QString::fromUtf8("<span style='width: 4px;'>&nbsp;</span><a href='x-trojita-manage-contact:%1'>"
                                     "<img src='%2' align='center'/></a>").arg(
                url.toString(QUrl::RemoveScheme),
                matchingDisplayNames.isEmpty() ? contactUnknownUrl : contactKnownUrl);
    setText(m_address.prettyName(Imap::Message::MailAddress::FORMAT_CLICKABLE) + icon + (m_lastOneInRow == Position::Middle ? tr(","): QString()));
}

void OneEnvelopeAddress::onLinkHovered(const QString &target)
{
    QUrl url(target);

    Imap::Message::MailAddress addr;
    if (!Imap::Message::MailAddress::fromUrl(addr, url, QLatin1String("mailto")) &&
            !Imap::Message::MailAddress::fromUrl(addr, url, QLatin1String("x-trojita-manage-contact"))) {
        setToolTip(QString());
        return;
    }

    // show fast version without data from addressbook now
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    m_link = Qt::escape(addr.prettyName(Imap::Message::MailAddress::FORMAT_READABLE));
#else
    m_link = addr.prettyName(Imap::Message::MailAddress::FORMAT_READABLE).toHtmlEscaped();
#endif
    setToolTip(m_link);

    // and later if there is an addressbook and all addressbook jobs have finished, show the full version
    Plugins::AddressbookPlugin *addressbook = m_pluginManager->addressbook();
    if (!addressbook || !(addressbook->features() & Plugins::AddressbookPlugin::FeaturePrettyNames))
        return;

    Plugins::AddressbookJob *job = addressbook->requestPrettyNamesForAddress(addr.mailbox + QLatin1Char('@') + addr.host);
    if (!job)
        return;

    connect(job, SIGNAL(prettyNamesForAddressAvailable(QStringList)), this, SLOT(finishOnLinkHovered(QStringList)));
    job->setAutoDelete(true);
    job->start();
}

void OneEnvelopeAddress::finishOnLinkHovered(const QStringList &matchingDisplayNames)
{
    if (matchingDisplayNames.isEmpty())
        return;

    // Addressbook can contain empty display name for email address
    // Instead empty line show something better in tooltip
    QStringList matchingIdentities;
    Q_FOREACH(const QString &str, matchingDisplayNames) {
        const QString &strToEscape = str.isEmpty() ? tr("(empty display name)") : str;
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        matchingIdentities << Qt::escape(strToEscape);
#else
        matchingIdentities << strToEscape.toHtmlEscaped();
#endif
    }

    QString identities = matchingIdentities.join(QLatin1String("<br/>\n"));
    setToolTip(m_link + tr("<hr/><b>Address Book:</b><br/>") + identities);
}

}
