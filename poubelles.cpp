#include "poubelles.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>


Poubelle::Poubelle() {
  id_poubelle = 0;
  localisation = "";
  niveau_remplissage = 0;
  capacite = 0;
  type_dechets = "";
  id_zone = 0;
  etat = "";
  date_vidange = QDate::currentDate();
}

Poubelle::Poubelle(int id, QString loc, int niv, int cap, QString type,
                   int zone, QString et, QDate date) {
  this->id_poubelle = id;
  this->localisation = loc;
  this->niveau_remplissage = niv;
  this->capacite = cap;
  this->type_dechets = type;
  this->id_zone = zone;
  this->etat = et;
  this->date_vidange = date;
}

bool Poubelle::ajouter() {
  QSqlQuery query;
  query.prepare("INSERT INTO POUBELLE (TYPE, CAPACITE_MAX, NIVEAU_REMPLISSAGE, "
                "ETAT, LOCALISATION, DATE_VIDANGE, ID_ZONE) "
                "VALUES (:type, :cap, :niv, :etat, :loc, :date, :zone)");
  query.bindValue(":type", type_dechets);
  query.bindValue(":cap", capacite);
  query.bindValue(":niv", niveau_remplissage);
  query.bindValue(":etat", etat);
  query.bindValue(":loc", localisation);
  query.bindValue(":date", date_vidange);
  query.bindValue(":zone", id_zone);

  if (!query.exec()) {
    qDebug() << "Erreur SQL (Ajout) :" << query.lastError().text();
    return false;
  }
  return true;
}

bool Poubelle::supprimer(int id) {
  QSqlQuery query;
  query.prepare("DELETE FROM POUBELLE WHERE ID_POUBELLE = :id");
  query.bindValue(":id", id);

  if (!query.exec()) {
    qDebug() << "Erreur SQL (Suppression) :" << query.lastError().text();
    return false;
  }
  return true;
}

bool Poubelle::modifier(int id) {
  QSqlQuery query;
  query.prepare("UPDATE POUBELLE SET TYPE = :type, "
                "NIVEAU_REMPLISSAGE = :niv, "
                "CAPACITE_MAX = :cap, "
                "LOCALISATION = :loc, "
                "ETAT = :etat, "
                "DATE_VIDANGE = :date "
                "WHERE ID_POUBELLE = :id");

  query.bindValue(":id", id);
  query.bindValue(":loc", localisation);
  query.bindValue(":niv", niveau_remplissage);
  query.bindValue(":cap", capacite);
  query.bindValue(":type", type_dechets);
  query.bindValue(":etat", etat);
  query.bindValue(":date", date_vidange);

  if (!query.exec()) {
    qDebug() << "Erreur SQL (Modification) :" << query.lastError().text();
    return false;
  }
  return true;
}

QSqlQueryModel *Poubelle::afficher() {
  QSqlQueryModel *model = new QSqlQueryModel();
  model->setQuery("SELECT ID_POUBELLE, TYPE, CAPACITE_MAX, NIVEAU_REMPLISSAGE, "
                  "LOCALISATION, ETAT, DATE_VIDANGE, ID_ZONE "
                  "FROM POUBELLE ORDER BY ID_POUBELLE");

  model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
  model->setHeaderData(1, Qt::Horizontal, QObject::tr("Type"));
  model->setHeaderData(2, Qt::Horizontal, QObject::tr("Capacité Max (L)"));
  model->setHeaderData(3, Qt::Horizontal, QObject::tr("Remplissage (%)"));
  model->setHeaderData(4, Qt::Horizontal, QObject::tr("Localisation"));
  model->setHeaderData(5, Qt::Horizontal, QObject::tr("Statut"));
  model->setHeaderData(6, Qt::Horizontal, QObject::tr("Collecte"));
  model->setHeaderData(7, Qt::Horizontal, QObject::tr("ID Zone"));

  if (model->lastError().isValid())
    qDebug() << "SQL Error (afficher poubelle):" << model->lastError().text();

  return model;
}
