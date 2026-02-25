#ifndef GESTION_EQUIPE_H
#define GESTION_EQUIPE_H
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QString>
#include <QDate>
class Gestion_equipe
{
private:
    QString nomEquipe,zoneEquipe,chefEquipe,statutEquipe;
    QDate datemission;
    int idEquipe,nbEquipe;

public:
    Gestion_equipe();
    Gestion_equipe(int,QString,QString,QString,int,QDate,QString);
    //get
    QString getNomEquipe() const {return nomEquipe; }
    QString getZoneEquipe() const {return zoneEquipe; }
    QString getChefEquipe() const {return chefEquipe; }
    QString getStatutEquipe() const {return statutEquipe; }
    QDate getDateMission() const {return datemission; }
    int getIdEquipe() const {return idEquipe; }
    int getNbEquipe() const {return nbEquipe; }
    // Set
    void setNomEquipe( QString x) {nomEquipe=x; }
    void setZoneEquipe( QString x) {zoneEquipe=x; }
    void setChefEquipe( QString x) {chefEquipe=x; }
    void setStatutEquipe( QString x) {statutEquipe=x; }
    void setDateMission( QDate x) {datemission=x; }
    void setIdEquipe(int x) { idEquipe=x; }
    void setNbEquipe(int x) { nbEquipe=x; }
    //crud
    bool ajouter();
    QSqlQueryModel* afficher();
    bool supprimer(int);
    bool modifier(int , QString , QString , QString , int , QDate , QString );
    QSqlQueryModel* trier();
    QSqlQueryModel* rechercher(const int& idRecherche);
};

#endif // GESTION_EQUIPE_H
