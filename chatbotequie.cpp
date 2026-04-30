#include "chatbotequie.h"
#include "connection.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

chatbotEquie::chatbotEquie(QObject *parent) : QObject(parent)
{
    m_networkManager = new QNetworkAccessManager(this);
// Cl� API supprim�e
}

chatbotEquie::~chatbotEquie() {}

void chatbotEquie::clearHistory()
{
    m_conversationHistory = QJsonArray();
}

// ✅ CORRECTION 1 : Format lisible pour le LLM
QString chatbotEquie::fetchEquipesData()
{
    QString result = "";
    QSqlDatabase db = Connection::instance()->getDatabase();
    QSqlQuery query(db);

    // On utilise le nom de table et les colonnes confirmés par la capture écran
    if (!query.exec("SELECT ID_EQUIPE, NOM_EQUIPE, ZONE_COUVERTE, CHEF_EQUIPE, NOMBRE_MEMBRES, STATUT FROM EQUIPES")) {
        // Fallback sur EQUIPE au cas où
        if (!query.exec("SELECT * FROM EQUIPE")) {
            return "Erreur SQL: " + query.lastError().text();
        }
    }

    int count = 0;
    while (query.next()) {
        count++;
        result += QString("- Équipe #%1: %2 | Chef: %3 | Zone: %4 | Statut: %5\n")
                      .arg(query.value(0).toString())
                      .arg(query.value(1).toString())
                      .arg(query.value(3).toString())
                      .arg(query.value(2).toString())
                      .arg(query.value(query.record().count()-1).toString());
    }

    if (count == 0) return "Aucune équipe trouvée dans EQUIPES.";
    return result;
}

QString chatbotEquie::parseApiResponse(const QByteArray &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray choices = doc.object()["choices"].toArray();
    if (!choices.isEmpty()) {
        return choices[0].toObject()["message"].toObject()["content"]
            .toString().trimmed();
    }
    return "Aucune réponse reçue.";
}

void chatbotEquie::sendMessage(const QString &userMessage)
{
    // Les données sont rechargées à chaque message (temps réel)
    QString equipesData = fetchEquipesData();

    QString systemPrompt = QString(
                               "Tu es Labib, assistant intelligent de gestion des équipes municipales. "
                               "Réponds TOUJOURS en français, de façon claire et concise. "
                               "Tu as accès aux données temps réel de la table EQUIPES ci-dessous.\n\n"
                               "DONNÉES ACTUELLES:\n%1\n"
                               "RÈGLES IMPORTANTES:\n"
                               "1. Base tes réponses UNIQUEMENT sur ces données réelles.\n"
                               "2. Si l'utilisateur demande de MODIFIER un statut, chef, zone ou date, "
                               "ajoute OBLIGATOIREMENT à la toute fin de ta réponse le tag suivant sur une ligne seule:\n"
                               "[UPDATE:NOM_EQUIPE|CHAMP|NOUVELLE_VALEUR]\n"
                               "Exemple: [UPDATE:Alpha|STATUT|En mission]\n"
                               "3. Champs modifiables: STATUT, CHEF_EQUIPE, ZONE_COUVERTE, DATE_MISSION\n"
                               "4. Valeurs STATUT valides: Disponible, Occupée, En mission, Formation, En repos\n"
                               "5. Ne simule JAMAIS une mise à jour sans mettre le tag [UPDATE:...]\n"
                               "6. Réponds de façon courte et directe."
                               ).arg(equipesData);

    QJsonObject jsonRequest;
    jsonRequest["model"]       = "llama-3.3-70b-versatile";
    jsonRequest["max_tokens"]  = 512;
    jsonRequest["temperature"] = 0.5; // ✅ CORRECTION 2 : température augmentée

    QJsonArray messages;

    QJsonObject systemMsg;
    systemMsg["role"]    = "system";
    systemMsg["content"] = systemPrompt;
    messages.append(systemMsg);

    // ✅ CORRECTION 3 : Injection de l'historique de conversation
    for (const QJsonValue &msg : m_conversationHistory) {
        messages.append(msg);
    }

    // Ajout du message utilisateur actuel
    QJsonObject userMsg;
    userMsg["role"]    = "user";
    userMsg["content"] = userMessage;
    messages.append(userMsg);

    // Sauvegarder dans l'historique
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

void chatbotEquie::onReplyFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "[ChatbotEquipe] Erreur réseau:" << reply->errorString();
        emit responseReceived("❌ Erreur de connexion au service IA.");
        emit processingFinished();
        return;
    }

    QByteArray data = reply->readAll();
    QString response = parseApiResponse(data);

    qDebug() << "[ChatbotEquipe] Réponse brute:" << response;

    // ✅ Sauvegarder la réponse dans l'historique
    QString cleanResponse = response;

    if (response.contains("[UPDATE:")) {
        int start = response.indexOf("[UPDATE:") + 8;
        int end   = response.indexOf("]", start);

        if (end > start) {
            QStringList parts = response.mid(start, end - start).split("|");
            if (parts.size() >= 3) {
                emit updateRequested(
                    parts[0].trimmed(),
                    parts[1].trimmed(),
                    parts[2].trimmed()
                    );
            }
        }

        cleanResponse = response.left(response.indexOf("[UPDATE:")).trimmed();
    }

    // Ajouter la réponse de l'assistant à l'historique
    QJsonObject assistantMsg;
    assistantMsg["role"]    = "assistant";
    assistantMsg["content"] = cleanResponse;
    m_conversationHistory.append(assistantMsg);

    // Limiter l'historique à 10 échanges (20 messages) pour éviter de dépasser les tokens
    while (m_conversationHistory.size() > 20) {
        m_conversationHistory.removeFirst();
    }

    emit responseReceived(cleanResponse);
    emit processingFinished();
}
