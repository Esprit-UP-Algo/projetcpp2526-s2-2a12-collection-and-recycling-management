#ifndef EQUIPE_H
#define EQUIPE_H

#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QString>
#include <QDate>

class Gestion_equipe
{
private:
    QString nomEquipe, zoneEquipe, chefEquipe, statutEquipe;
    QDate datemission;
    int idEquipe, nbEquipe;

public:
    Gestion_equipe();
    Gestion_equipe(int id, QString nom, QString zone, QString chef, int nb, QDate date, QString statut);

    // Getters
    QString getNomEquipe() const { return nomEquipe; }
    QString getZoneEquipe() const { return zoneEquipe; }
    QString getChefEquipe() const { return chefEquipe; }
    QString getStatutEquipe() const { return statutEquipe; }
    QDate getDateMission() const { return datemission; }
    int getIdEquipe() const { return idEquipe; }
    int getNbEquipe() const { return nbEquipe; }

    // Setters
    void setNomEquipe(QString x) { nomEquipe = x; }
    void setZoneEquipe(QString x) { zoneEquipe = x; }
    void setChefEquipe(QString x) { chefEquipe = x; }
    void setStatutEquipe(QString x) { statutEquipe = x; }
    void setDateMission(QDate x) { datemission = x; }
    void setIdEquipe(int x) { idEquipe = x; }
    void setNbEquipe(int x) { nbEquipe = x; }

    // CRUD
    bool ajouter();
    QSqlQueryModel* afficher();
    bool supprimer(int id);
    bool modifier(int id, QString nom, QString zone, QString chef, int nb, QDate date, QString statut);
    QSqlQueryModel* trier();
    QSqlQueryModel* rechercher(QString val);
};

#endif // EQUIPE_H
