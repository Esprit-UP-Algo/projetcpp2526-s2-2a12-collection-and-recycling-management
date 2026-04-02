#ifndef EQUIPES_H
#define EQUIPES_H

#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QString>
#include <QDate>

class Gestion_equipe
{
private:
    int idEquipe;
    QString nomEquipe;
    QString zoneEquipe;
    QString chefEquipe;
    int nbEquipe;
    QDate datemission;
    QString statutEquipe;

public:
    Gestion_equipe();
    Gestion_equipe(int, QString, QString, QString, int, QDate, QString);

    // Getters
    int getIdEquipe() const { return idEquipe; }
    QString getNomEquipe() const { return nomEquipe; }
    QString getZoneEquipe() const { return zoneEquipe; }
    QString getChefEquipe() const { return chefEquipe; }
    int getNbEquipe() const { return nbEquipe; }
    QDate getDateMission() const { return datemission; }
    QString getStatutEquipe() const { return statutEquipe; }

    // Setters
    void setIdEquipe(int x) { idEquipe = x; }
    void setNomEquipe(QString x) { nomEquipe = x; }
    void setZoneEquipe(QString x) { zoneEquipe = x; }
    void setChefEquipe(QString x) { chefEquipe = x; }
    void setNbEquipe(int x) { nbEquipe = x; }
    void setDateMission(QDate x) { datemission = x; }
    void setStatutEquipe(QString x) { statutEquipe = x; }

    // CRUD
    bool ajouter();
    QSqlQueryModel* afficher();
    bool supprimer(int);
    bool modifier(int);
    QSqlQueryModel* trier();
    QSqlQueryModel* rechercher(const int& idRecherche);
};

#endif // EQUIPES_H
