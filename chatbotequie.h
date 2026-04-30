#ifndef CHATBOTEQUIE_H
#define CHATBOTEQUIE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>  // ← AJOUT

// Key managed locally in .cpp

class chatbotEquie : public QObject
{
    Q_OBJECT

public:
    explicit chatbotEquie(QObject *parent = nullptr);
    ~chatbotEquie();
    void sendMessage(const QString &userMessage);
    void clearHistory(); // ← AJOUT : pour réinitialiser la conversation

signals:
    void responseReceived(const QString &response);
    void processingFinished();
    void updateRequested(const QString &nomEquipe,
                         const QString &champ,
                         const QString &valeur);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_networkManager;
    QString fetchEquipesData();
    QString parseApiResponse(const QByteArray &data);
    QString groq_api_key;
    QJsonArray m_conversationHistory; // ← AJOUT : historique des messages
};

#endif
