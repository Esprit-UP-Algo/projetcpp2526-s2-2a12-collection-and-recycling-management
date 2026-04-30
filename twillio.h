#ifndef TWILLIO_H
#define TWILLIO_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>

class twillio : public QObject
{
    Q_OBJECT

public:
    explicit twillio(QObject *parent = nullptr);
    ~twillio();

    void sendSMS(const QString& to, const QString& from,
                 const QString& body, const QString& account_sid,
                 const QString& auth_token);

signals:
    void smsSent(const QString& sid);
    void smsError(const QString& errorMsg, int statusCode);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *networkManager;
};

#endif // TWILLIO_H
