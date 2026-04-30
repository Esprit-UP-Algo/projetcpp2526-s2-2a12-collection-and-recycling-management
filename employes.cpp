#include "employes.h"
#include "connection.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

Employe::Employe()
    : id(0), heuresTravail(0), salaire(0.0), rfid_uid(""), idEquipe(1) {}

Employe::Employe(int id, QString nom, QString email, QString poste,
                 QString disponibilite, int heuresTravail,
                 double salaire, QString sexe, QString rfidUid, int idEquipe)
    : id(id), nom(nom), email(email), poste(poste),
      disponibilite(disponibilite), heuresTravail(heuresTravail),
      salaire(salaire), sexe(sexe), rfid_uid(rfidUid), idEquipe(idEquipe) {}

bool Employe::ajouter() {
    Connection *c = Connection::instance();

    if (!c->getDatabase().isOpen()) {
        qDebug() << "Base de donnÃ©es non ouverte!";
        return false;
    }

    QSqlQuery query(c->getDatabase());

    query.prepare("INSERT INTO EMPLOYE "
                  "(NOM, EMAIL, POSTE, DISPONIBILITE, HEURES_TRAVAIL, SALAIRE, SEXE, RFID_UID, ID_EQUIPE) "
                  "VALUES "
                  "(:nom, :email, :poste, :dispo, :heures, :salaire, :sexe, :rfid_uid, :id_equipe)");

    query.bindValue(":nom", nom);
    query.bindValue(":email", email);
    query.bindValue(":poste", poste);
    query.bindValue(":dispo", disponibilite);
    query.bindValue(":heures", heuresTravail);
    query.bindValue(":salaire", salaire);
    query.bindValue(":sexe", sexe);
    query.bindValue(":rfid_uid", rfid_uid);
    query.bindValue(":id_equipe", idEquipe);

    bool ok = query.exec();
    if (!ok)
        qDebug() << "[ERREUR] Employe::ajouter() :" << query.lastError().text();

    return ok;
}

bool Employe::supprimer(int id) {
    Connection *c = Connection::instance();

    if (!c->getDatabase().isOpen()) {
        qDebug() << "Base de donnÃ©es non ouverte!";
        return false;
    }

    QSqlQuery query(c->getDatabase());

    query.prepare("DELETE FROM EMPLOYE WHERE ID_EMPLOYE = :id");
    query.bindValue(":id", id);

    bool ok = query.exec();
    if (!ok)
        qDebug() << "[ERREUR] Employe::supprimer() :" << query.lastError().text();

    return ok;
}

bool Employe::modifier(int id) {
    Connection *c = Connection::instance();

    if (!c->getDatabase().isOpen()) {
        qDebug() << "Base de donnÃ©es non ouverte!";
        return false;
    }
    QSqlQuery checkQuery(c->getDatabase());
    checkQuery.prepare("SELECT COUNT(*) FROM EMPLOYE WHERE EMAIL = :email AND ID_EMPLOYE != :id");
    checkQuery.bindValue(":email", email);
    checkQuery.bindValue(":id", id);
    checkQuery.exec();
    if (checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        qDebug() << "[ERREUR] Email already exists for another employee";
        return false;
    }

    QSqlQuery query(c->getDatabase());

    query.prepare("UPDATE EMPLOYE SET "
                  "NOM           = :nom,    "
                  "EMAIL         = :email,  "
                  "POSTE         = :poste,  "
                  "DISPONIBILITE = :dispo,  "
                  "HEURES_TRAVAIL= :heures, "
                  "SALAIRE       = :salaire,"
                  "SEXE          = :sexe,   "
                  "RFID_UID      = :rfid_uid, "
                  "ID_EQUIPE     = :id_equipe "
                  "WHERE ID_EMPLOYE = :id");

    query.bindValue(":nom", nom);
    query.bindValue(":email", email);
    query.bindValue(":poste", poste);
    query.bindValue(":dispo", disponibilite);
    query.bindValue(":heures", heuresTravail);
    query.bindValue(":salaire", salaire);
    query.bindValue(":sexe", sexe);
    query.bindValue(":rfid_uid", rfid_uid);
    query.bindValue(":id_equipe", idEquipe);
    query.bindValue(":id", id);

    bool ok = query.exec();
    if (!ok)
        qDebug() << "[ERREUR] Employe::modifier() :" << query.lastError().text();

    return ok;
}

QSqlQueryModel* Employe::afficher() {
    Connection *c = Connection::instance();
    QSqlQueryModel *model = new QSqlQueryModel();

    QString queryStr = "SELECT e.ID_EMPLOYE, e.NOM, e.EMAIL, e.POSTE, e.DISPONIBILITE, e.HEURES_TRAVAIL, e.SALAIRE, e.SEXE, e.RFID_UID, eq.NOM_EQUIPE "
                       "FROM EMPLOYE e LEFT JOIN EQUIPES eq ON e.ID_EQUIPE = eq.ID_EQUIPE ORDER BY e.ID_EMPLOYE";
    
    model->setQuery(queryStr, c->getDatabase());
    
    if (model->lastError().isValid() || model->rowCount() == 0) {
        queryStr = "SELECT e.ID_EMPLOYE, e.NOM, e.EMAIL, e.POSTE, e.DISPONIBILITE, e.HEURES_TRAVAIL, e.SALAIRE, e.SEXE, e.RFID_UID, eq.NOM_EQUIPE "
                   "FROM EMPLOYE e LEFT JOIN EQUIPE eq ON e.ID_EQUIPE = eq.ID_EQUIPE ORDER BY e.ID_EMPLOYE";
        model->setQuery(queryStr, c->getDatabase());
    }

    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Nom");
    model->setHeaderData(2, Qt::Horizontal, "Email");
    model->setHeaderData(3, Qt::Horizontal, "Poste");
    model->setHeaderData(4, Qt::Horizontal, "Disponibilite");
    model->setHeaderData(5, Qt::Horizontal, "Heures/Sem");
    model->setHeaderData(6, Qt::Horizontal, "Salaire (DT)");
    model->setHeaderData(7, Qt::Horizontal, "Sexe");
    model->setHeaderData(8, Qt::Horizontal, "RFID UID");
    model->setHeaderData(9, Qt::Horizontal, "Equipe");

    return model;
}

QSqlQueryModel* Employe::rechercher(const QString &valeur, const QString &critere) {
    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery query(Connection::instance()->getDatabase());

    QString condition;
    if (critere.toUpper() == "NOM") {
        condition = "UPPER(e.NOM) LIKE UPPER(:val)";
    } else if (critere.toUpper() == "EMAIL") {
        condition = "UPPER(e.EMAIL) LIKE UPPER(:val)";
    } else if (critere.toUpper() == "POSTE") {
        condition = "UPPER(e.POSTE) LIKE UPPER(:val)";
    } else if (critere.toUpper() == "DISPONIBILITE") {
        condition = "UPPER(e.DISPONIBILITE) LIKE UPPER(:val)";
    } else {
        // Recherche globale par defaut (incluant Disponibilite)
        condition = "(UPPER(e.NOM) LIKE UPPER(:val) OR UPPER(e.EMAIL) LIKE UPPER(:val) OR UPPER(e.POSTE) LIKE UPPER(:val) OR UPPER(e.DISPONIBILITE) LIKE UPPER(:val))";
    }

    QString baseQuery = "SELECT e.ID_EMPLOYE, e.NOM, e.EMAIL, e.POSTE, e.DISPONIBILITE, e.HEURES_TRAVAIL, e.SALAIRE, e.SEXE, e.RFID_UID, eq.NOM_EQUIPE "
                        "FROM %1 e LEFT JOIN %2 eq ON e.ID_EQUIPE = eq.ID_EQUIPE "
                        "WHERE " + condition + " "
                        "ORDER BY e.ID_EMPLOYE";

    // Essai 1: EMPLOYE + EQUIPES
    query.prepare(baseQuery.arg("EMPLOYE").arg("EQUIPES"));
    query.bindValue(":val", "%" + valeur.toUpper() + "%");
    
    if (!query.exec()) {
        // Essai 2: EMPLOYE + EQUIPE
        query.prepare(baseQuery.arg("EMPLOYE").arg("EQUIPE"));
        query.bindValue(":val", "%" + valeur.toUpper() + "%");
        
        if (!query.exec()) {
            // Essai 3: EMPLOYES + EQUIPES
            query.prepare(baseQuery.arg("EMPLOYES").arg("EQUIPES"));
            query.bindValue(":val", "%" + valeur.toUpper() + "%");
            
            if (!query.exec()) {
                qDebug() << "[ERREUR] Employe::rechercher() final fallback failed:" << query.lastError().text();
                return model; 
            }
        }
    }

    model->setQuery(std::move(query));
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Nom");
    model->setHeaderData(2, Qt::Horizontal, "Email");
    model->setHeaderData(3, Qt::Horizontal, "Poste");
    model->setHeaderData(4, Qt::Horizontal, "Disponibilite");
    model->setHeaderData(5, Qt::Horizontal, "Heures/Sem");
    model->setHeaderData(6, Qt::Horizontal, "Salaire (DT)");
    model->setHeaderData(7, Qt::Horizontal, "Sexe");
    model->setHeaderData(8, Qt::Horizontal, "RFID UID");
    model->setHeaderData(9, Qt::Horizontal, "Equipe");

    return model;
}



QSqlQueryModel* Employe::trier(const QString &critere, const QString &ordre) {
    QSqlQueryModel *model = new QSqlQueryModel();

    QStringList colonnesValides = {"ID_EMPLOYE","NOM","EMAIL","POSTE",
                                   "DISPONIBILITE","HEURES_TRAVAIL","SALAIRE","SEXE"};
    QStringList ordresValides   = {"ASC","DESC"};

    QString col = colonnesValides.contains(critere.toUpper())
                      ? critere.toUpper() : "ID_EMPLOYE";
    QString ord = ordresValides.contains(ordre.toUpper())
                      ? ordre.toUpper() : "ASC";

    QString sql = "SELECT e.ID_EMPLOYE, e.NOM, e.EMAIL, e.POSTE, e.DISPONIBILITE, e.HEURES_TRAVAIL, e.SALAIRE, e.SEXE, e.RFID_UID, eq.NOM_EQUIPE "
                  "FROM EMPLOYE e LEFT JOIN EQUIPES eq ON e.ID_EQUIPE = eq.ID_EQUIPE ORDER BY " + col + " " + ord;
    
    model->setQuery(sql, Connection::instance()->getDatabase());
    
    if (model->lastError().isValid()) {
        sql = "SELECT e.ID_EMPLOYE, e.NOM, e.EMAIL, e.POSTE, e.DISPONIBILITE, e.HEURES_TRAVAIL, e.SALAIRE, e.SEXE, e.RFID_UID, eq.NOM_EQUIPE "
              "FROM EMPLOYE e LEFT JOIN EQUIPE eq ON e.ID_EQUIPE = eq.ID_EQUIPE ORDER BY " + col + " " + ord;
        model->setQuery(sql, Connection::instance()->getDatabase());
    }

    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Nom");
    model->setHeaderData(2, Qt::Horizontal, "Email");
    model->setHeaderData(3, Qt::Horizontal, "Poste");
    model->setHeaderData(4, Qt::Horizontal, "Disponibilite");
    model->setHeaderData(5, Qt::Horizontal, "Heures/Sem");
    model->setHeaderData(6, Qt::Horizontal, "Salaire (DT)");
    model->setHeaderData(7, Qt::Horizontal, "Sexe");
    model->setHeaderData(8, Qt::Horizontal, "RFID UID");
    model->setHeaderData(9, Qt::Horizontal, "Equipe");

    return model;
}



