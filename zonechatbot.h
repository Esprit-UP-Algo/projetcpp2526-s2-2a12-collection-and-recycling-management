#ifndef ZONECHATBOT_H
#define ZONECHATBOT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMap>
#include <QJsonArray>

// Key managed locally in .cpp

class ZoneChatbot : public QObject {
    Q_OBJECT
public:
    explicit ZoneChatbot(QObject *parent = nullptr);
    ~ZoneChatbot();

    void sendMessage(const QString &message);
    void confirmAddition();
    void clearHistory();

signals:
    void responseReceived(const QString &response);
    void errorOccurred(const QString &error);
    void processingFinished();
    void insertRequestReady(const QString &nom, const QString &loc,
                            int pop, const QString &surface);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QString parseApiResponse(const QByteArray &data);
    void callGroqApi(const QString &userMessage);
    QString fetchZonesData();

    QNetworkAccessManager *m_networkManager;
    QMap<QString, QString> m_pendingZone;
    QJsonArray m_conversationHistory; // ✅ Historique ajouté
    QString groq_api_key;
};

#endif
