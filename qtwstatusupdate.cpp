/* Copyright (c) 2010, Antonie Jovanoski
 *
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact e-mail: Antonie Jovanoski <minimoog77_at_gmail.com>
 */

#include <QNetworkRequest>
#include <QNetworkReply>
#include "qtwstatusupdate.h"

QTwStatusUpdate::QTwStatusUpdate(QObject *parent) :
    QtwitterNetBase(parent)
{
}

QTwStatusUpdate::QTwStatusUpdate(OAuthTwitter *oauthTwitter, QObject *parent) :
        QtwitterNetBase(oauthTwitter, parent)
{
}

void QTwStatusUpdate::post(const QString &status,
                           qint64 inReplyToStatus,
                           qreal latitude,
                           qreal longitude,
                           const QString &placeid,
                           bool displayCoordinates,
                           ResponseType respType)
{
    Q_ASSERT(oauthTwitter() != 0);

    QUrl url;

    if (respType == QtwitterNetBase::JSON)
        url.setUrl("http://api.twitter.com/1/statuses/update.json");
    else
        url.setUrl("http://api.twitter.com/1/statuses/update.xml");

    QUrl urlQuery;

    urlQuery.addQueryItem("status", status);

    if (inReplyToStatus != 0)
        urlQuery.addQueryItem("in_reply_to_status_id", QString::number(inReplyToStatus));

    if (latitude != 0)
        urlQuery.addQueryItem("lat", QString::number(latitude));

    if (longitude != 0)
        urlQuery.addQueryItem("long", QString::number(longitude));

    if (!placeid.isEmpty())
        urlQuery.addQueryItem("place_id", placeid);

    if (displayCoordinates)
        urlQuery.addQueryItem("display_coordinates", "true");

    QByteArray oauthHeader = oauthTwitter()->generateAuthorizationHeader(urlQuery, OAuth::POST);
    QNetworkRequest req(url);
    req.setRawHeader(AUTH_HEADER, oauthHeader);

    //build status post array
    QByteArray statusPost = urlQuery.toEncoded(QUrl::RemoveScheme | QUrl::RemoveAuthority | QUrl::RemovePath);

    //remove '?'
    statusPost.remove(0, 1);

    QNetworkReply *reply = oauthTwitter()->networkAccessManager()->post(req, statusPost);
    connect(reply, SIGNAL(finished()), this, SLOT(reply()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error()));
}

void QTwStatusUpdate::reply()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if (reply) {
         m_response = reply->readAll();
        emit finished(m_response);

        reply->deleteLater();
    }
}

void QTwStatusUpdate::error()
{
    // ### TODO:
}
