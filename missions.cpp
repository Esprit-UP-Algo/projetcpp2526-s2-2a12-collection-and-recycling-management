#include "missions.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "connection.h"

Missions::Missions() {
    id = 0;
    type = "";
    date = QDate::currentDate();
    duree = QTime(1, 0);
    equipe = 0;
    zone = 0;
    etat = "";
    priorite = "";
    description = "";
}

Missions::Missions(int id, QString type, QDate date, QTime duree, int equipe, int zone, QString etat, QString priorite, QString desc) {
    this->id = id;
    this->type = type;
    this->date = date;
    this->duree = duree;
    this->equipe = equipe;
    this->zone = zone;
    this->etat = etat;
    this->priorite = priorite;
    this->description = desc;
}

bool Missions::ajouter() {
    QSqlQuery query;
    query.prepare("INSERT INTO MISSION (TYPE, DATE_MISSION, DUREE, ETAT, PRIORITE, ID_EQUIPE, ID_ZONE) "
                  "VALUES (:type, :date, :duree, :etat, :priorite, :equipe, :zone)");
    query.bindValue(":type", type);
    query.bindValue(":date", date);
    query.bindValue(":duree", duree.hour()*60 + duree.minute());
    query.bindValue(":etat", etat);
    query.bindValue(":priorite", priorite);
    query.bindValue(":equipe", equipe);
    query.bindValue(":zone", zone);
    if (!query.exec()) {
        qDebug() << "Erreur SQL (ajouter mission):" << query.lastError().text();
        return false;
    }
    return true;
}

QSqlQueryModel* Missions::afficher() {
    Connection *c = Connection::instance();
    QSqlQueryModel* model = new QSqlQueryModel();
    QString queryStr = "SELECT ID_MISSION, TYPE, DATE_MISSION, DUREE, ETAT, PRIORITE, ID_EQUIPE, ID_ZONE FROM MISSIONS ORDER BY DATE_MISSION DESC";
    
    model->setQuery(queryStr, c->getDatabase());
    if (model->lastError().isValid() || model->rowCount() == 0) {
        queryStr = "SELECT ID_MISSION, TYPE, DATE_MISSION, DUREE, ETAT, PRIORITE, ID_EQUIPE, ID_ZONE FROM MISSION ORDER BY DATE_MISSION DESC";
        model->setQuery(queryStr, c->getDatabase());
    }

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID Mission"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Type"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Date"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Durée (min)"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("État"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Priorité"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("ID Équipe"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("ID Zone"));
    return model;
}

bool Missions::supprimer(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM MISSION WHERE ID_MISSION = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        qDebug() << "Erreur SQL (supprimer mission):" << query.lastError().text();
        return false;
    }
    return true;
}

bool Missions::modifier() {
    QSqlQuery query;
    query.prepare("UPDATE MISSION SET TYPE=:type, DATE_MISSION=:date, DUREE=:duree, "
                  "ETAT=:etat, PRIORITE=:priorite, ID_EQUIPE=:equipe, ID_ZONE=:zone "
                  "WHERE ID_MISSION=:id");
    query.bindValue(":id", id);
    query.bindValue(":type", type);
    query.bindValue(":date", date);
    query.bindValue(":duree", duree.hour()*60 + duree.minute());
    query.bindValue(":etat", etat);
    query.bindValue(":priorite", priorite);
    query.bindValue(":equipe", equipe);
    query.bindValue(":zone", zone);
    if (!query.exec()) {
        qDebug() << "Erreur SQL (modifier mission):" << query.lastError().text();
        return false;
    }
    return true;
}

QSqlQueryModel* Missions::trier(QString critere) {
    QSqlQueryModel* model = new QSqlQueryModel();
    QStringList valides = {"TYPE","DATE_MISSION","DUREE","ETAT","PRIORITE"};
    QString col = valides.contains(critere.toUpper()) ? critere.toUpper() : "DATE_MISSION";
    QString queryStr = "SELECT ID_MISSION, TYPE, DATE_MISSION, DUREE, ETAT, PRIORITE, ID_EQUIPE, ID_ZONE FROM MISSION ";
    if (col == "PRIORITE") {
        queryStr += "ORDER BY CASE PRIORITE WHEN 'Haute' THEN 1 WHEN 'Moyenne' THEN 2 WHEN 'Basse' THEN 3 ELSE 4 END";
    } else {
        queryStr += "ORDER BY " + col;
    }
    model->setQuery(queryStr);
    return model;
}

QSqlQueryModel* Missions::rechercher(QString valeur) {
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;
    query.prepare("SELECT ID_MISSION, TYPE, DATE_MISSION, DUREE, ETAT, PRIORITE, ID_EQUIPE, ID_ZONE "
                  "FROM MISSION WHERE UPPER(TYPE) LIKE UPPER(:val) "
                  "OR UPPER(ETAT) LIKE UPPER(:val) "
                  "OR UPPER(PRIORITE) LIKE UPPER(:val) "
                  "OR TO_CHAR(DATE_MISSION, 'DD/MM/YYYY') LIKE :val "
                  "OR TO_CHAR(ID_MISSION) LIKE :val");
    query.bindValue(":val", "%" + valeur + "%");
    if (!query.exec()) {
        qDebug() << "Erreur SQL (rechercher mission):" << query.lastError().text();
        delete model;
        return nullptr;
    }
    model->setQuery(std::move(query));
    return model;
}
