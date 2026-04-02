#include "equipes.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

Gestion_equipe::Gestion_equipe() {}

Gestion_equipe::Gestion_equipe(int id,QString nom,QString zone,QString chef,int nb,QDate date,QString statut)
{
    idEquipe=id;
    nomEquipe=nom;
    zoneEquipe=zone;
    chefEquipe=chef;
    nbEquipe=nb;
    datemission=date;
    statutEquipe=statut;
}

bool Gestion_equipe::ajouter()
{
    QSqlQuery query;
    query.prepare("INSERT INTO EQUIPE (NOM_EQUIPE, ID_ZONE, CHEF_EQUIPE, NOMBRE_MEMBRES, STATUT) "
                  "VALUES (:nomEquipe, :zoneEquipe, :chefEquipe, :nbEquipe, :statutEquipe)");

    query.bindValue(":nomEquipe", nomEquipe);
    query.bindValue(":zoneEquipe", zoneEquipe);
    query.bindValue(":chefEquipe", chefEquipe);
    query.bindValue(":nbEquipe", nbEquipe);
    query.bindValue(":statutEquipe", statutEquipe);

    if (!query.exec()) {
        QString erreur = query.lastError().text();
        qDebug() << "SQL Error:" << erreur;
        if (erreur.contains("ORA-00001") || erreur.contains("duplicate")) {
            QMessageBox::critical(nullptr, "Erreur", "Cette équipe existe déjà !");
        } else {
            QMessageBox::critical(nullptr, "Erreur SQL", erreur);
        }
        return false;
    }
    return true;
}

QSqlQueryModel* Gestion_equipe::afficher()
{
    if (!QSqlDatabase::database().isOpen()) {
        qDebug() << "Database not open!";
        return nullptr;
    }
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT ID_EQUIPE, NOM_EQUIPE, ID_ZONE, CHEF_EQUIPE, NOMBRE_MEMBRES, STATUT FROM EQUIPE ORDER BY ID_EQUIPE");

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID Equipe"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom Equipe"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("ID Zone"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Chef Equipe"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Nb Membres"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Statut"));
    return model;
}

bool Gestion_equipe::supprimer(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM EQUIPE WHERE ID_EQUIPE = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Erreur SQL (supprimer equipe):" << query.lastError().text();
        return false;
    }
    return true;
}

bool Gestion_equipe::modifier(int id)
{
    QSqlQuery query;
    query.prepare("UPDATE EQUIPE SET NOM_EQUIPE = :nom, ID_ZONE = :zone, CHEF_EQUIPE = :chef, "
                  "NOMBRE_MEMBRES = :nb, STATUT = :statut WHERE ID_EQUIPE = :id");

    query.bindValue(":nom", nomEquipe);
    query.bindValue(":zone", zoneEquipe);
    query.bindValue(":chef", chefEquipe);
    query.bindValue(":nb", nbEquipe);
    query.bindValue(":statut", statutEquipe);
    query.bindValue(":id", id);

    if(!query.exec()){
        qDebug() << "Erreur modifier:" << query.lastError().text();
        return false;
    }
    return true;
}

QSqlQueryModel* Gestion_equipe::trier()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT ID_EQUIPE, NOM_EQUIPE, ID_ZONE, CHEF_EQUIPE, NOMBRE_MEMBRES, STATUT "
                    "FROM EQUIPE ORDER BY NOMBRE_MEMBRES ASC");

    if (model->lastError().isValid()) {
        qDebug() << "Erreur SQL lors du tri :" << model->lastError().text();
        delete model;
        return nullptr;
    }

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID Equipe"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom Equipe"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("ID Zone"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Chef Equipe"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Nb Membres"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Statut"));

    return model;
}

QSqlQueryModel* Gestion_equipe::rechercher(const int& idRecherche)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;
    query.prepare("SELECT ID_EQUIPE, NOM_EQUIPE, ID_ZONE, CHEF_EQUIPE, NOMBRE_MEMBRES, STATUT "
                  "FROM EQUIPE WHERE ID_EQUIPE = :id");
    query.bindValue(":id", idRecherche);

    if (!query.exec()) {
        qDebug() << "Erreur SQL (rechercher equipe):" << query.lastError().text();
        delete model;
        return nullptr;
    }

    model->setQuery(std::move(query));
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID Equipe"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom Equipe"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("ID Zone"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Chef Equipe"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Nb Membres"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Statut"));

    return model;
}
