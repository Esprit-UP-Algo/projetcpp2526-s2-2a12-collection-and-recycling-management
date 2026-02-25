#include "gestion_equipe.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

Gestion_equipe::Gestion_equipe() {}

Gestion_equipe::Gestion_equipe(int id,QString nom,QString zone,QString chef,int nb,QDate date,QString statut)
{
    nomEquipe=nom;
    idEquipe=id;
    zoneEquipe=zone;
    chefEquipe=chef;
    statutEquipe=statut;
    datemission=date;
    nbEquipe=nb;
}
bool Gestion_equipe::ajouter()
{
    QSqlQuery query;
    query.prepare("INSERT INTO EQUIPES (ID_EQUIPE, NOM_EQUIPE, ZONE_COUVERTE, CHEF_EQUIPE, NOMBRE_MEMBRES, DATE_MISSION, STATUT) "
                  "VALUES (:id, :nomEquipe, :zoneEquipe, :chefEquipe, :nbEquipe, TO_DATE(:datemission, 'YYYY-MM-DD'), :statutEquipe)");

    query.bindValue(":id",idEquipe);
    query.bindValue(":nomEquipe",nomEquipe);
    query.bindValue(":zoneEquipe",zoneEquipe);
    query.bindValue(":chefEquipe",chefEquipe);
    query.bindValue(":nbEquipe", nbEquipe);
    query.bindValue(":datemission",datemission.toString("yyyy-MM-dd"));
    query.bindValue(":statutEquipe",statutEquipe);

    if (!query.exec()) {
        QString erreur = query.lastError().text();
        qDebug() << "SQL Error:" << erreur;
        if (erreur.contains("ORA-00001")) {
            QMessageBox::critical(nullptr, "Erreur", " Cette équipe existe déjà !");
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
    model->setQuery("SELECT * FROM EQUIPES");
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
    QSqlQuery query;
    QString res=QString::number(id);
    query.prepare("Delete from EQUIPES where ID_EQUIPE=:id");
    query.bindValue(":id",res);
    return query.exec();
}
bool Gestion_equipe::modifier(int id, QString nom, QString zone, QString chef, int nb, QDate date, QString statut)
{
    QSqlQuery query;
    query.prepare("UPDATE EQUIPES SET NOM_EQUIPE = :nom, ZONE_COUVERTE = :zone, CHEF_EQUIPE = :chef, NOMBRE_MEMBRES = :nb, DATE_MISSION = TO_DATE(:date, 'YYYY-MM-DD'), STATUT = :statut WHERE ID_EQUIPE = :id");

    query.bindValue(":nom",    nom);
    query.bindValue(":zone",   zone);
    query.bindValue(":chef",   chef);
    query.bindValue(":nb",     nb);
    query.bindValue(":date",   date.toString("yyyy-MM-dd"));
    query.bindValue(":statut", statut);
    query.bindValue(":id",     id);

    if(!query.exec()){
        qDebug() << "❌ Erreur modifier:" << query.lastError().text();
        return false;
    }
    return true;
}
QSqlQueryModel* Gestion_equipe::trier()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM EQUIPES ORDER BY NOMBRE_MEMBRES ASC");

    if (model->lastError().isValid()) {
        qDebug() << "Erreur SQL lors du tri :" << model->lastError().text();
        delete model;
        return nullptr;
    }

    model->setHeaderData(0, Qt::Horizontal, "ID Equipe");
    model->setHeaderData(1, Qt::Horizontal, "Nom Equipe");
    model->setHeaderData(2, Qt::Horizontal, "Zone Couverte");
    model->setHeaderData(3, Qt::Horizontal, "Chef Equipe");
    model->setHeaderData(4, Qt::Horizontal, "Nombre Membres");
    model->setHeaderData(5, Qt::Horizontal, "Date Mission");
    model->setHeaderData(6, Qt::Horizontal, "Statut");

    return model;
}
QSqlQueryModel* Gestion_equipe::rechercher(const int& idRecherche)
{
    QSqlQueryModel* model=new QSqlQueryModel();
    QSqlQuery query;
    query.prepare("SELECT * FROM EQUIPES WHERE ID_EQUIPE = :id");
    query.bindValue(":id", idRecherche);
    if (query.exec()) {
        model->setQuery(std::move(query));
        model->setHeaderData(0, Qt::Horizontal, "ID Equipe");
        model->setHeaderData(1, Qt::Horizontal, "Nom Equipe");
        model->setHeaderData(2, Qt::Horizontal, "Zone Couverte");
        model->setHeaderData(3, Qt::Horizontal, "Chef Equipe");
        model->setHeaderData(4, Qt::Horizontal, "Nombre Membres");
        model->setHeaderData(5, Qt::Horizontal, "Date Mission");
        model->setHeaderData(6, Qt::Horizontal, "Statut");
        return model;
    } else {
        qDebug() << "Erreur SQL lors de la recherche :" << query.lastError().text();
        delete model;
        return nullptr;
    }
}
