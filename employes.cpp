#include "employes.h"
#include "connection.h"
#include <QDebug>

Employe::Employe()
    : id(0), heuresTravail(0), salaire(0.0), idEquipe(1) {}

Employe::Employe(int id, QString nom, QString email, QString poste,
                 QString disponibilite, int heuresTravail,
                 double salaire, QString sexe, int idEquipe)
    : id(id), nom(nom), email(email), poste(poste),
    disponibilite(disponibilite), heuresTravail(heuresTravail),
    salaire(salaire), sexe(sexe), idEquipe(idEquipe) {}

bool Employe::ajouter() {
    Connection *c = Connection::instance();

    if (!c->getDatabase().isOpen()) {
        qDebug() << "Base de donnÃ©es non ouverte!";
        return false;
    }

    QSqlQuery query(c->getDatabase());

    query.prepare("INSERT INTO EMPLOYE "
                  "(NOM, EMAIL, POSTE, DISPONIBILITE, HEURES_TRAVAIL, SALAIRE, SEXE, ID_EQUIPE) "
                  "VALUES "
                  "(:nom, :email, :poste, :dispo, :heures, :salaire, :sexe, :id_equipe)");

    query.bindValue(":nom", nom);
    query.bindValue(":email", email);
    query.bindValue(":poste", poste);
    query.bindValue(":dispo", disponibilite);
    query.bindValue(":heures", heuresTravail);
    query.bindValue(":salaire", salaire);
    query.bindValue(":sexe", sexe);
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
                  "ID_EQUIPE     = :id_equipe "
                  "WHERE ID_EMPLOYE = :id");

    query.bindValue(":nom", nom);
    query.bindValue(":email", email);
    query.bindValue(":poste", poste);
    query.bindValue(":dispo", disponibilite);
    query.bindValue(":heures", heuresTravail);
    query.bindValue(":salaire", salaire);
    query.bindValue(":sexe", sexe);
    query.bindValue(":id_equipe", idEquipe);
    query.bindValue(":id", id);

    bool ok = query.exec();
    if (!ok)
        qDebug() << "[ERREUR] Employe::modifier() :" << query.lastError().text();

    return ok;
}

QSqlQueryModel* Employe::afficher() {
    QSqlQueryModel *model = new QSqlQueryModel();

    model->setQuery(
        "SELECT e.ID_EMPLOYE, e.NOM, e.EMAIL, e.POSTE, "
        "e.DISPONIBILITE, e.HEURES_TRAVAIL, e.SALAIRE, e.SEXE, "
        "eq.NOM_EQUIPE "
        "FROM EMPLOYE e "
        "LEFT JOIN EQUIPE eq ON e.ID_EQUIPE = eq.ID_EQUIPE "
        "ORDER BY e.ID_EMPLOYE",
        Connection::instance()->getDatabase()
        );

    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Nom");
    model->setHeaderData(2, Qt::Horizontal, "Email");
    model->setHeaderData(3, Qt::Horizontal, "Poste");
    model->setHeaderData(4, Qt::Horizontal, "Disponibilite");
    model->setHeaderData(5, Qt::Horizontal, "Heures/Sem");
    model->setHeaderData(6, Qt::Horizontal, "Salaire (DT)");
    model->setHeaderData(7, Qt::Horizontal, "Sexe");
    model->setHeaderData(8, Qt::Horizontal, "Equipe");

    return model;
}

QSqlQueryModel* Employe::rechercher(const QString &valeur, const QString &critere) {
    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery query(Connection::instance()->getDatabase());

    QString condition;
    if (critere.toUpper() == "NOM") {
        condition = "e.NOM LIKE :val";
    } else if (critere.toUpper() == "EMAIL") {
        condition = "e.EMAIL LIKE :val";
    } else if (critere.toUpper() == "POSTE") {
        condition = "e.POSTE LIKE :val";
    } else if (critere.toUpper() == "DISPONIBILITE") {
        condition = "e.DISPONIBILITE LIKE :val";
    } else {
        // Recherche globale par defaut (incluant Disponibilite)
        condition = "(e.NOM LIKE :val OR e.EMAIL LIKE :val OR e.POSTE LIKE :val OR e.DISPONIBILITE LIKE :val)";
    }

    query.prepare("SELECT e.ID_EMPLOYE, e.NOM, e.EMAIL, e.POSTE, "
                  "e.DISPONIBILITE, e.HEURES_TRAVAIL, e.SALAIRE, e.SEXE, "
                  "eq.NOM_EQUIPE "
                  "FROM EMPLOYE e "
                  "LEFT JOIN EQUIPE eq ON e.ID_EQUIPE = eq.ID_EQUIPE "
                  "WHERE " + condition + " "
                  "ORDER BY e.ID_EMPLOYE");
    query.bindValue(":val", "%" + valeur + "%");

    if (!query.exec()) {
        qDebug() << "[ERREUR] Employe::rechercher() :" << query.lastError().text();
        delete model;
        return nullptr;
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
    model->setHeaderData(8, Qt::Horizontal, "Equipe");

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

    model->setQuery(
        "SELECT e.ID_EMPLOYE, e.NOM, e.EMAIL, e.POSTE, "
        "e.DISPONIBILITE, e.HEURES_TRAVAIL, e.SALAIRE, e.SEXE, "
        "eq.NOM_EQUIPE "
        "FROM EMPLOYE e "
        "LEFT JOIN EQUIPE eq ON e.ID_EQUIPE = eq.ID_EQUIPE "
        "ORDER BY " + col + " " + ord,
        Connection::instance()->getDatabase()
        );

    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Nom");
    model->setHeaderData(2, Qt::Horizontal, "Email");
    model->setHeaderData(3, Qt::Horizontal, "Poste");
    model->setHeaderData(4, Qt::Horizontal, "DisponibilitÃ©");
    model->setHeaderData(5, Qt::Horizontal, "Heures/Sem");
    model->setHeaderData(6, Qt::Horizontal, "Salaire (DT)");
    model->setHeaderData(7, Qt::Horizontal, "Sexe");
    model->setHeaderData(8, Qt::Horizontal, "Ã‰quipe");

    return model;
}
