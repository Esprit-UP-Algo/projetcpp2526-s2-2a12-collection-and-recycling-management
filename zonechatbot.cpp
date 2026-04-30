#include "zonechatbot.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QRegularExpression>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlRecord>
#include "connection.h"

ZoneChatbot::ZoneChatbot(QObject *parent) : QObject(parent) {
    m_networkManager = new QNetworkAccessManager(this);
// Cl� API supprim�e
}

ZoneChatbot::~ZoneChatbot() {}

void ZoneChatbot::clearHistory() {
    m_conversationHistory = QJsonArray();
    m_pendingZone.clear();
}

void ZoneChatbot::sendMessage(const QString &message) {
    QString msg = message.trimmed().toLower();

    if ((msg == "oui" || msg == "yes" || msg == "o" || msg == "confirmer")
        && !m_pendingZone.isEmpty()) {
        confirmAddition();
        // Le message de succès est géré par executeZoneInsertion via appendBotMessage
        emit processingFinished();
        return;
    }

    if ((msg == "non" || msg == "no" || msg == "n" || msg == "annuler")
        && !m_pendingZone.isEmpty()) {
        m_pendingZone.clear();
        emit responseReceived("❌ Ajout annulé.");
        emit processingFinished();
        return;
    }

// Cl� API supprim�e
}

// Cl� API supprim�e
    QString zonesData = fetchZonesData();

    // ✅ System prompt corrigé : exemple sans km², règles claires
    QString systemPrompt = QString(
        "Tu es Labib, assistant de gestion des zones municipales. "
        "Réponds TOUJOURS en français, de façon claire et concise.\n\n"
        "DONNÉES ACTUELLES DES ZONES :\n%1\n\n"
        "RÈGLES IMPORTANTES :\n"
        "1. Base tes réponses sur les données ci-dessus.\n"
        "2. Si l'utilisateur demande d'AJOUTER une zone (nouvelle zone, créer, insérer...), "
        "tu dois OBLIGATOIREMENT terminer ta réponse par ce tag sur une ligne seule :\n"
        "[INSERT:Nom|Localisation|Population|Superficie]\n"
        "Exemple : [INSERT:Zone Sud|Sfax Sud|85000|12.5]\n"
        "3. La population doit être un nombre entier UNIQUEMENT, sans espaces ni virgules (ex: 85000).\n"
        "4. La superficie doit être un nombre décimal UNIQUEMENT, sans unité (ex: 12.5 et non '12.5 km²').\n"
        "5. Ne mets le tag [INSERT:...] QUE si l'utilisateur veut ajouter une zone.\n"
        "6. Réponds de façon courte et directe."
    ).arg(zonesData);

    QJsonObject jsonRequest;
    jsonRequest["model"]       = "llama-3.3-70b-versatile";
    jsonRequest["max_tokens"]  = 512;
    jsonRequest["temperature"] = 0.4;

    QJsonArray messages;

    QJsonObject systemMsg;
    systemMsg["role"]    = "system";
    systemMsg["content"] = systemPrompt;
    messages.append(systemMsg);

    for (const QJsonValue &msg : m_conversationHistory) {
        messages.append(msg);
    }

    QJsonObject userMsg;
    userMsg["role"]    = "user";
    userMsg["content"] = userMessage;
    messages.append(userMsg);

    m_conversationHistory.append(userMsg);

    jsonRequest["messages"] = messages;

// Cl� API supprim�e
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
// Cl� API supprim�e

    QNetworkReply *reply = m_networkManager->post(
        request, QJsonDocument(jsonRequest).toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        this->onReplyFinished(reply);
    });
}

void ZoneChatbot::confirmAddition() {
    if (!m_pendingZone.isEmpty()) {
        emit insertRequestReady(
            m_pendingZone["nom"],
            m_pendingZone["loc"],
            m_pendingZone["pop"].toInt(),
            m_pendingZone["surf"]
            );
        emit processingFinished();
    }
}

QString ZoneChatbot::fetchZonesData() {
    QString result = "";
    QSqlDatabase db = Connection::instance()->getDatabase();
    QSqlQuery query(db);

    // On essaie d'abord ZONES (utilisé dans le CRUD)
    if (!query.exec("SELECT * FROM ZONES")) {
        // Fallback sur ZONE
        if (!query.exec("SELECT * FROM ZONE")) {
            return "Erreur SQL lors de la lecture des zones.";
        }
    }

    int count = 0;
    while (query.next()) {
        count++;
        result += QString("- Zone %1: %2 | Pop: %3 | Surf: %4\n")
                      .arg(query.value(0).toString())
                      .arg(query.value(1).toString())
                      .arg(query.value(4).toString())
                      .arg(query.value(3).toString());
    }

    if (count == 0) return "Aucune zone trouvée.";
    return result;
}

QString ZoneChatbot::parseApiResponse(const QByteArray &data) {
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray choices = doc.object()["choices"].toArray();
    if (!choices.isEmpty()) {
        return choices[0].toObject()["message"].toObject()["content"].toString().trimmed();
    }
    return "Aucune réponse reçue de l'IA.";
}

void ZoneChatbot::onReplyFinished(QNetworkReply *reply) {
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "[ZoneChatbot] Erreur réseau:" << reply->errorString();
        emit responseReceived("❌ Erreur de connexion au service IA.");
        emit processingFinished();
        return;
    }

    QByteArray data = reply->readAll();
    QString response = parseApiResponse(data);

    qDebug() << "[ZoneChatbot] Réponse brute:" << response;

    QString cleanResponse = response;

    if (response.contains("[INSERT:")) {
        int start = response.indexOf("[INSERT:") + 8;
        int end   = response.indexOf("]", start);

        if (end > start) {
            QStringList parts = response.mid(start, end - start).split("|");

            if (parts.size() >= 4) {
                m_pendingZone["nom"] = parts[0].trimmed();
                m_pendingZone["loc"] = parts[1].trimmed();

                // ✅ Nettoyage population : chiffres uniquement
                QString rawPop = parts[2].trimmed();
                rawPop.remove(QRegularExpression("[^0-9]"));
                m_pendingZone["pop"] = rawPop;

                // ✅ Nettoyage superficie : chiffres et point décimal uniquement (ligne en double supprimée)
                QString rawSurf = parts[3].trimmed();
                rawSurf.remove(QRegularExpression("[^0-9\\.]"));
                m_pendingZone["surf"] = rawSurf;

                qDebug() << "[ZoneChatbot] Zone en attente:"
                         << m_pendingZone["nom"] << m_pendingZone["loc"]
                         << m_pendingZone["pop"] << m_pendingZone["surf"];
            }
        }

        cleanResponse = response.left(response.indexOf("[INSERT:")).trimmed();
        emit responseReceived(cleanResponse + "\n\n📌 Voulez-vous ajouter cette zone ? (Oui / Non)");

    } else {
        emit responseReceived(response);
    }

    // Sauvegarder la réponse dans l'historique
    QJsonObject assistantMsg;
    assistantMsg["role"]    = "assistant";
    assistantMsg["content"] = cleanResponse;
    m_conversationHistory.append(assistantMsg);

    // Limiter l'historique à 10 échanges
    while (m_conversationHistory.size() > 20) {
        m_conversationHistory.removeFirst();
    }

    emit processingFinished();
}
