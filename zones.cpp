#include "zones.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QObject>

zones::zones()
{
    id_zone = 0;
    nom_zone = "";
    localisation = "";
    priorite = "";
    population = 0;
    superficie = 0.0;
    nombre_poubelle = 0;
}

zones::zones(int id, QString nom, QString local, QString prior, int pop, double super, int nb)
{
    id_zone = id;
    nom_zone = nom;
    localisation = local;
    priorite = prior;
    population = pop;
    superficie = super;
    nombre_poubelle = nb;
}

bool zones::ajouter()
{
    QSqlQuery query;
    query.prepare("INSERT INTO ZONES (ID_ZONE, NOM_ZONE, LOCALISATION, SUPERFICIE, POPULATION, PRIORITE, NOMBRE_POUBELLE) "
                  "VALUES (:id, :nom, :local, :super, :pop, :prior, :nb)");
    query.bindValue(":id", id_zone);
    query.bindValue(":nom", nom_zone);
    query.bindValue(":local", localisation);
    query.bindValue(":super", superficie);
    query.bindValue(":pop", population);
    query.bindValue(":prior", priorite);
    query.bindValue(":nb", nombre_poubelle);

    if (!query.exec()) {
        qDebug() << "SQL Error (ajouter zone):" << query.lastError().text();
        return false;
    }
    return true;
}

QSqlQueryModel* zones::afficher()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM ZONES ORDER BY ID_ZONE");
    
    if (model->lastError().isValid()) {
        qDebug() << "SQL ERROR (ZONES):" << model->lastError().text();
    }
    
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID Zone"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom Zone"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Localisation"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Superficie"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Population"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Priorite"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Nb Poubelles"));
    return model;
}

bool zones::modifier(int id)
{
    QSqlQuery query;
    query.prepare("UPDATE ZONES SET "
                  "NOM_ZONE = :nom, "
                  "LOCALISATION = :local, "
                  "SUPERFICIE = :super, "
                  "POPULATION = :pop, "
                  "PRIORITE = :prior, "
                  "NOMBRE_POUBELLE = :nb "
                  "WHERE ID_ZONE = :id");
    query.bindValue(":nom", nom_zone);
    query.bindValue(":local", localisation);
    query.bindValue(":super", superficie);
    query.bindValue(":pop", population);
    query.bindValue(":prior", priorite);
    query.bindValue(":nb", nombre_poubelle);
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "SQL Error (modifier zone):" << query.lastError().text();
        return false;
    }
    return true;
}

bool zones::supprimer(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM ZONES WHERE ID_ZONE = :id");
    query.bindValue(":id", id);
    return query.exec();
}

QSqlQueryModel* zones::trier()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM ZONES ORDER BY POPULATION DESC");

    if (model->lastError().isValid()) {
        qDebug() << "SQL Error (trier zones):" << model->lastError().text();
    }

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID Zone"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom Zone"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Localisation"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Superficie"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Population"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Priorite"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Nb Poubelles"));
    
    return model;
}

QSqlQueryModel* zones::rechercher(const int &id)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;
    query.prepare("SELECT * FROM ZONES WHERE ID_ZONE = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qDebug() << "SQL Error (rechercher):" << query.lastError().text();
        delete model;
        return nullptr;
    }
    
    model->setQuery(std::move(query));
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID Zone"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom Zone"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Localisation"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Superficie"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Population"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Priorite"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Nb Poubelles"));
    
    return model;
}
