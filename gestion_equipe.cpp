#include "gestion_equipe.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include "connection.h"

Gestion_equipe::Gestion_equipe() 
{
    idEquipe = 0;
    nbEquipe = 0;
}

Gestion_equipe::Gestion_equipe(int id, QString nom, QString zone, QString chef, int nb, QDate date, QString statut)
{
    idEquipe = id;
    nomEquipe = nom;
    zoneEquipe = zone;
    chefEquipe = chef;
    nbEquipe = nb;
    datemission = date;
    statutEquipe = statut;
}

bool Gestion_equipe::ajouter()
{
    Connection *c = Connection::instance();
    QSqlQuery query(c->getDatabase());
    query.prepare("INSERT INTO EQUIPES (ID_EQUIPE, NOM_EQUIPE, ZONE_COUVERTE, CHEF_EQUIPE, NOMBRE_MEMBRES, DATE_MISSION, STATUT) "
                  "VALUES (:id, :nomEquipe, :zoneEquipe, :chefEquipe, :nbEquipe, :datemission, :statutEquipe)");

    query.bindValue(":id", idEquipe);
    query.bindValue(":nomEquipe", nomEquipe);
    query.bindValue(":zoneEquipe", zoneEquipe);
    query.bindValue(":chefEquipe", chefEquipe);
    query.bindValue(":nbEquipe", nbEquipe);
    query.bindValue(":datemission", datemission);
    query.bindValue(":statutEquipe", statutEquipe);

    if (!query.exec()) {
        // Fallback singular
        query.prepare("INSERT INTO EQUIPE (ID_EQUIPE, NOM_EQUIPE, ZONE_COUVERTE, CHEF_EQUIPE, NOMBRE_MEMBRES, DATE_MISSION, STATUT) "
                      "VALUES (:id, :nomEquipe, :zoneEquipe, :chefEquipe, :nbEquipe, :datemission, :statutEquipe)");
        query.bindValue(":id", idEquipe);
        query.bindValue(":nomEquipe", nomEquipe);
        query.bindValue(":zoneEquipe", zoneEquipe);
        query.bindValue(":chefEquipe", chefEquipe);
        query.bindValue(":nbEquipe", nbEquipe);
        query.bindValue(":datemission", datemission);
        query.bindValue(":statutEquipe", statutEquipe);
        if (!query.exec()) {
            qDebug() << "SQL Error:" << query.lastError().text();
            return false;
        }
    }
    return true;
}

QSqlQueryModel* Gestion_equipe::afficher()
{
    Connection *c = Connection::instance();
    QSqlQueryModel* model = new QSqlQueryModel();
    QString queryStr = "SELECT ID_EQUIPE, NOM_EQUIPE, ZONE_COUVERTE, CHEF_EQUIPE, NOMBRE_MEMBRES, DATE_MISSION, STATUT FROM EQUIPES";
    
    model->setQuery(queryStr, c->getDatabase());
    if (model->lastError().isValid() || model->rowCount() == 0) {
        queryStr = "SELECT ID_EQUIPE, NOM_EQUIPE, ZONE_COUVERTE, CHEF_EQUIPE, NOMBRE_MEMBRES, DATE_MISSION, STATUT FROM EQUIPE";
        model->setQuery(queryStr, c->getDatabase());
    }

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID Equipe"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom Equipe"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Zone Couverte"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Chef Equipe"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Nombre Membres"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Date Mission"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Statut"));
    return model;
}

bool Gestion_equipe::supprimer(int id)
{
    Connection *c = Connection::instance();
    QSqlQuery query(c->getDatabase());
    query.prepare("DELETE FROM EQUIPES WHERE ID_EQUIPE = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        query.prepare("DELETE FROM EQUIPE WHERE ID_EQUIPE = :id");
        query.bindValue(":id", id);
        return query.exec();
    }
    return true;
}

bool Gestion_equipe::modifier(int id, QString nom, QString zone, QString chef, int nb, QDate date, QString statut)
{
    Connection *c = Connection::instance();
    QSqlQuery query(c->getDatabase());
    query.prepare("UPDATE EQUIPES SET NOM_EQUIPE = :nom, ZONE_COUVERTE = :zone, CHEF_EQUIPE = :chef, NOMBRE_MEMBRES = :nb, DATE_MISSION = :date, STATUT = :statut WHERE ID_EQUIPE = :id");

    query.bindValue(":nom", nom);
    query.bindValue(":zone", zone);
    query.bindValue(":chef", chef);
    query.bindValue(":nb", nb);
    query.bindValue(":date", date);
    query.bindValue(":statut", statut);
    query.bindValue(":id", id);

    if (!query.exec()) {
        query.prepare("UPDATE EQUIPE SET NOM_EQUIPE = :nom, ZONE_COUVERTE = :zone, CHEF_EQUIPE = :chef, NOMBRE_MEMBRES = :nb, DATE_MISSION = :date, STATUT = :statut WHERE ID_EQUIPE = :id");
        query.bindValue(":nom", nom);
        query.bindValue(":zone", zone);
        query.bindValue(":chef", chef);
        query.bindValue(":nb", nb);
        query.bindValue(":date", date);
        query.bindValue(":statut", statut);
        query.bindValue(":id", id);
        return query.exec();
    }
    return true;
}

QSqlQueryModel* Gestion_equipe::trier()
{
    Connection *c = Connection::instance();
    QSqlQueryModel* model = new QSqlQueryModel();
    QString queryStr = "SELECT * FROM EQUIPES ORDER BY NOMBRE_MEMBRES ASC";
    
    model->setQuery(queryStr, c->getDatabase());
    if (model->lastError().isValid() || model->rowCount() == 0) {
        queryStr = "SELECT * FROM EQUIPE ORDER BY NOMBRE_MEMBRES ASC";
        model->setQuery(queryStr, c->getDatabase());
    }
    return model;
}

QSqlQueryModel* Gestion_equipe::rechercher(QString val)
{
    Connection *c = Connection::instance();
    QSqlQueryModel* model = new QSqlQueryModel();
    
    // On construit une requête qui cherche soit par ID (si val est un nombre) soit par NOM (via LIKE)
    QString queryStr = "SELECT * FROM EQUIPES WHERE NOM_EQUIPE LIKE '%" + val + "%' OR ID_EQUIPE LIKE '%" + val + "%'";
    
    model->setQuery(queryStr, c->getDatabase());
    
    // Fallback pour la table au singulier si la première requête échoue ou ne renvoie rien
    if (model->lastError().isValid() || model->rowCount() == 0) {
        queryStr = "SELECT * FROM EQUIPE WHERE NOM_EQUIPE LIKE '%" + val + "%' OR ID_EQUIPE LIKE '%" + val + "%'";
        model->setQuery(queryStr, c->getDatabase());
    }
    
    return model;
}
