#include "zones.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "connection.h"
zones::zones() {}
zones::zones(int id, QString nom, QString local, QString prior, int pop, QString super, int nb)
{
    id_zone = id;
    nom_zone = nom;
    localisation= local;
    priorite= prior;
    population = pop;
    superficie= super;
    nombre_poubelle = nb;
}

bool zones::ajouter()
{
    Connection *c = Connection::instance();
    QSqlQuery query(c->getDatabase());
    QString id = QString::number(id_zone);
    QString nb = QString::number(nombre_poubelle);
    query.prepare("INSERT INTO ZONES (ID_ZONE, NOM_ZONE, LOCALISATION, PRIORITE, POPULATION, SUPERFICE, NOMBRE_POUBELLE) "
                  "VALUES (:id, :nom, :local, :prior, :pop, :super, :nb)");
    query.bindValue(":id",  id);
    query.bindValue(":nom",  nom_zone);
    query.bindValue(":local",localisation);
    query.bindValue(":prior",priorite);
    query.bindValue(":pop", population);
    query.bindValue(":super", superficie);
    query.bindValue(":nb", nb);

    if (!query.exec()) {
        // Fallback singular
        query.prepare("INSERT INTO ZONE (ID_ZONE, NOM_ZONE, LOCALISATION, PRIORITE, POPULATION, SUPERFICE, NOMBRE_POUBELLE) "
                      "VALUES (:id, :nom, :local, :prior, :pop, :super, :nb)");
        if (!query.exec()) {
            qDebug() << "SQL Error:" << query.lastError().text();
            return false;
        }
    }
    return true;
}
QSqlQueryModel* zones::afficher()
{
    Connection *c = Connection::instance();
    QSqlQueryModel* model = new QSqlQueryModel();
    QString queryStr = "SELECT ID_ZONE, NOM_ZONE, LOCALISATION, PRIORITE, POPULATION, SUPERFICE, NOMBRE_POUBELLE FROM ZONES";
    
    model->setQuery(queryStr, c->getDatabase());
    if (model->lastError().isValid() || model->rowCount() == 0) {
        queryStr = "SELECT ID_ZONE, NOM_ZONE, LOCALISATION, PRIORITE, POPULATION, SUPERFICE, NOMBRE_POUBELLE FROM ZONE";
        model->setQuery(queryStr, c->getDatabase());
    }

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID Zone"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom Zone"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Localisation"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Priorité"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Population"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("superficie"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Nb Poubelles"));
    return model;
}
bool zones::modifier(int id, QString nom, QString local, QString prior, int pop, QString super, int nb)
{
    Connection *c = Connection::instance();
    QSqlQuery query(c->getDatabase());
    QString idStr = QString::number(id);
    QString popStr=QString::number(pop);
    QString nbStr =QString::number(nb);

    query.prepare("UPDATE ZONES SET NOM_ZONE = :nom, LOCALISATION = :local, PRIORITE = :prior, POPULATION = :pop, SUPERFICE = :super, NOMBRE_POUBELLE = :nb WHERE ID_ZONE = :id");
    query.bindValue(":id",idStr);
    query.bindValue(":nom",nom);
    query.bindValue(":local",local);
    query.bindValue(":prior",prior);
    query.bindValue(":pop",popStr);
    query.bindValue(":super", super);
    query.bindValue(":nb", nbStr);

    if (!query.exec()) {
        query.prepare("UPDATE ZONE SET NOM_ZONE = :nom, LOCALISATION = :local, PRIORITE = :prior, POPULATION = :pop, SUPERFICE = :super, NOMBRE_POUBELLE = :nb WHERE ID_ZONE = :id");
        query.bindValue(":id",idStr);
        query.bindValue(":nom",nom);
        query.bindValue(":local",local);
        query.bindValue(":prior",prior);
        query.bindValue(":pop",popStr);
        query.bindValue(":super", super);
        query.bindValue(":nb", nbStr);
        if (!query.exec()) {
            qDebug() << "SQL Error:" << query.lastError().text();
            return false;
        }
    }
    return true;
}
bool zones::supprimer(int id)
{
    Connection *c = Connection::instance();
    QSqlQuery query(c->getDatabase());
    QString idStr = QString::number(id);
    query.prepare("DELETE FROM ZONES WHERE ID_ZONE = :id");
    query.bindValue(":id", idStr);

    if (!query.exec()) {
        query.prepare("DELETE FROM ZONE WHERE ID_ZONE = :id");
        query.bindValue(":id", idStr);
        if (!query.exec()) {
            qDebug() << "SQL Error:" << query.lastError().text();
            return false;
        }
    }
    return true;
}
QSqlQueryModel* zones::trier()
{
    Connection *c = Connection::instance();
    QSqlQueryModel* model = new QSqlQueryModel();
    QString queryStr = "SELECT ID_ZONE, NOM_ZONE, LOCALISATION, PRIORITE, POPULATION, SUPERFICE, NOMBRE_POUBELLE FROM ZONES ORDER BY POPULATION ASC";
    
    model->setQuery(queryStr, c->getDatabase());
    if (model->lastError().isValid() || model->rowCount() == 0) {
        queryStr = "SELECT ID_ZONE, NOM_ZONE, LOCALISATION, PRIORITE, POPULATION, SUPERFICE, NOMBRE_POUBELLE FROM ZONE ORDER BY POPULATION ASC";
        model->setQuery(queryStr, c->getDatabase());
    }

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID Zone"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom Zone"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Localisation"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Priorité"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Population"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("superficie"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Nb Poubelles"));
    return model;
}
QSqlQueryModel* zones::rechercher(const int &id)
{
    Connection *c = Connection::instance();
    QSqlQueryModel* model = new QSqlQueryModel();
    QString queryStr = "SELECT ID_ZONE, NOM_ZONE, LOCALISATION, PRIORITE, POPULATION, SUPERFICE, NOMBRE_POUBELLE FROM ZONES WHERE ID_ZONE = " + QString::number(id);
    
    model->setQuery(queryStr, c->getDatabase());
    if (model->lastError().isValid() || model->rowCount() == 0) {
        queryStr = "SELECT ID_ZONE, NOM_ZONE, LOCALISATION, PRIORITE, POPULATION, SUPERFICE, NOMBRE_POUBELLE FROM ZONE WHERE ID_ZONE = " + QString::number(id);
        model->setQuery(queryStr, c->getDatabase());
    }

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID Zone"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom Zone"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Localisation"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Priorité"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Population"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("superficie"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Nb Poubelles"));
    return model;
}
