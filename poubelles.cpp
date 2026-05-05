#include "poubelles.h"
#include "connection.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

// ============================================================
// CONSTRUCTEURS
// ============================================================

Poubelle::Poubelle() {
    id_poubelle        = 0;
    localisation       = "";
    niveau_remplissage = 0;
    capacite           = 0;
    type_dechets       = "";
    id_zone            = 0;
    etat               = "";
    date_vidange       = QDate::currentDate();
}

Poubelle::Poubelle(int id, QString loc, int niv, int cap,
                   QString type, int zone, QString et, QDate date) {
    this->id_poubelle        = id;
    this->localisation       = loc;
    this->niveau_remplissage = niv;
    this->capacite           = cap;
    this->type_dechets       = type;
    this->id_zone            = zone;
    this->etat               = et;
    this->date_vidange       = date;
}

// ============================================================
// AJOUTER
// ============================================================

bool Poubelle::ajouter() {
    Connection *c = Connection::instance();

    if (!c->getDatabase().isOpen()) {
        qDebug() << "[ERREUR] Poubelle::ajouter() : Base de données non ouverte !";
        return false;
    }

    // Vérifier que l'ID n'existe pas déjà
    QSqlQuery checkQuery(c->getDatabase());
    checkQuery.prepare("SELECT COUNT(*) FROM POUBELLE WHERE ID_POUBELLE = :id");
    checkQuery.bindValue(":id", id_poubelle);
    if (checkQuery.exec() && checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        qDebug() << "[ERREUR] Poubelle::ajouter() : ID déjà existant !";
        return false;
    }

    QSqlQuery query(c->getDatabase());
    query.prepare(
        "INSERT INTO POUBELLE "
        "(ID_POUBELLE, TYPE, CAPACITE_MAX, NIVEAU_REMPLISSAGE, ETAT, LOCALISATION, DATE_VIDANGE, ID_ZONE) "
        "VALUES (:id, :type, :cap, :niv, :etat, :loc, :date, :zone)"
        );
    query.bindValue(":id",   id_poubelle);
    query.bindValue(":type", type_dechets);
    query.bindValue(":cap",  capacite);
    query.bindValue(":niv",  niveau_remplissage);
    query.bindValue(":etat", etat);
    query.bindValue(":loc",  localisation);
    query.bindValue(":date", date_vidange);
    query.bindValue(":zone", id_zone);

    bool ok = query.exec();
    if (!ok)
        qDebug() << "[ERREUR] Poubelle::ajouter() :" << query.lastError().text();

    return ok;
}

// ============================================================
// SUPPRIMER
// ============================================================

bool Poubelle::supprimer(int id) {
    Connection *c = Connection::instance();

    if (!c->getDatabase().isOpen()) {
        qDebug() << "[ERREUR] Poubelle::supprimer() : Base de données non ouverte !";
        return false;
    }

    QSqlQuery query(c->getDatabase());
    query.prepare("DELETE FROM POUBELLE WHERE ID_POUBELLE = :id");
    query.bindValue(":id", id);

    bool ok = query.exec();
    if (!ok)
        qDebug() << "[ERREUR] Poubelle::supprimer() :" << query.lastError().text();

    return ok;
}

// ============================================================
// MODIFIER
// ============================================================

bool Poubelle::modifier(int id) {
    Connection *c = Connection::instance();

    if (!c->getDatabase().isOpen()) {
        qDebug() << "[ERREUR] Poubelle::modifier() : Base de données non ouverte !";
        return false;
    }

    QSqlQuery query(c->getDatabase());
    query.prepare(
        "UPDATE POUBELLE SET "
        "TYPE               = :type, "
        "NIVEAU_REMPLISSAGE = :niv,  "
        "CAPACITE_MAX       = :cap,  "
        "LOCALISATION       = :loc,  "
        "ETAT               = :etat, "
        "DATE_VIDANGE       = :date, "
        "ID_ZONE            = :zone  "  // ← MANQUAIT
        "WHERE ID_POUBELLE  = :id"
        );

    query.bindValue(":id",   id);
    query.bindValue(":type", type_dechets);
    query.bindValue(":niv",  niveau_remplissage);
    query.bindValue(":cap",  capacite);
    query.bindValue(":loc",  localisation);
    query.bindValue(":etat", etat);
    query.bindValue(":date", date_vidange);
    query.bindValue(":zone", id_zone);  // ← MANQUAIT

    bool ok = query.exec();
    if (!ok)
        qDebug() << "[ERREUR] Poubelle::modifier() :" << query.lastError().text();

    return ok;
}
// ============================================================
// AFFICHER  ← CORRECTION PRINCIPALE : passage de la connexion
// ============================================================

QSqlQueryModel* Poubelle::afficher() {
    Connection *c = Connection::instance();

    if (!c->getDatabase().isOpen()) {
        qDebug() << "[ERREUR] Poubelle::afficher() : Base de données non ouverte !";
        return new QSqlQueryModel();
    }

    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery(
        "SELECT ID_POUBELLE, TYPE, CAPACITE_MAX, NIVEAU_REMPLISSAGE, "
        "LOCALISATION, ETAT, DATE_VIDANGE, ID_ZONE "
        "FROM POUBELLE ORDER BY ID_POUBELLE",
        c->getDatabase()   // ← CORRECTION : connexion explicite
        );

    if (model->lastError().isValid())
        qDebug() << "[ERREUR] Poubelle::afficher() :" << model->lastError().text();

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Type"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Capacité Max (L)"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Remplissage (%)"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Localisation"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Statut"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Date Vidange"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("ID Zone"));

    return model;
}
