#include "twillio.h"
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QUrl>
#include <QDebug>

twillio::twillio(QObject *parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &twillio::onReplyFinished);
}

twillio::~twillio()
{
    delete networkManager;
}

void twillio::sendSMS(const QString& to, const QString& from,
                      const QString& body, const QString& account_sid,
                      const QString& auth_token)
{
    QUrl url(QString("https://api.twilio.com/2010-04-01/Accounts/%1/Messages.json")
                 .arg(account_sid));

    QNetworkRequest request(url);
    QString auth = QString("%1:%2").arg(account_sid).arg(auth_token);
    request.setRawHeader("Authorization", "Basic " + auth.toUtf8().toBase64());
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/x-www-form-urlencoded");

    // ✅ Ajoute whatsapp: SEULEMENT si pas déjà présent (évite le double préfixe)
    QString toWhatsapp   = to.startsWith("whatsapp:")   ? to   : "whatsapp:" + to;
    QString fromWhatsapp = from.startsWith("whatsapp:") ? from : "whatsapp:" + from;

    QByteArray postBytes;
    postBytes += "To="    + QUrl::toPercentEncoding(toWhatsapp);
    postBytes += "&From=" + QUrl::toPercentEncoding(fromWhatsapp);
    postBytes += "&Body=" + QUrl::toPercentEncoding(body);

    networkManager->post(request, postBytes);

    qDebug() << "[Twillio] Requête WhatsApp envoyée";
    qDebug() << "[Twillio] To:"   << toWhatsapp;
    qDebug() << "[Twillio] From:" << fromWhatsapp;
}

void twillio::onReplyFinished(QNetworkReply *reply)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QString response = QString::fromUtf8(reply->readAll());

    qDebug() << "[Twillio] Code HTTP:" << statusCode;
    qDebug() << "[Twillio] Réponse:" << response;

    if (reply->error() == QNetworkReply::NoError && statusCode >= 200 && statusCode < 300) {
        // Extraire le SID
        QString sid = "";
        if (response.contains("\"sid\"")) {
            int start = response.indexOf("\"sid\"") + 7;
            int end   = response.indexOf("\"", start);
            if (start > 7 && end > start)
                sid = response.mid(start, end - start);
        }
        emit smsSent(sid);
    } else {
        // Extraire le message d'erreur Twilio
        QString errorMsg = reply->errorString();
        if (response.contains("\"message\"")) {
            int colonPos   = response.indexOf(":", response.indexOf("\"message\""));
            int quoteStart = response.indexOf("\"", colonPos);
            int quoteEnd   = response.indexOf("\"", quoteStart + 1);
            if (quoteStart != -1 && quoteEnd != -1)
                errorMsg = response.mid(quoteStart + 1, quoteEnd - quoteStart - 1);
        }
        emit smsError(errorMsg, statusCode);
    }

    reply->deleteLater();
}
