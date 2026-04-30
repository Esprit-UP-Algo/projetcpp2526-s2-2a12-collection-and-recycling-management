#ifndef ZONES_H
#define ZONES_H
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QString>

class zones
{
private:
    int id_zone, nombre_poubelle,population;
    QString nom_zone, localisation, priorite,  superficie;

public:
    zones();
    zones(int, QString, QString, QString, int, QString, int);

    // Getters
    int getId_zone()          { return id_zone; }
    QString getNom_zone()       { return nom_zone; }
    QString getLocalisation()   { return localisation; }
    QString getPriorite()       { return priorite; }
    int getPopulation()     { return population; }
    QString getSuperficie()     { return superficie; }
    int getNombre_poubelle()    { return nombre_poubelle; }

    // Setters
    void setId_zone(int x)           { id_zone = x; }
    void setNom_zone(QString n)      { nom_zone = n; }
    void setLocalisation(QString l)  { localisation = l; }
    void setPriorite(QString p)      { priorite = p; }
    void setPopulation(int p)    { population = p; }
    void setSuperficie(QString s)    { superficie = s; }
    void setNombre_poubelle(int n)   { nombre_poubelle = n; }

    // Fonctions CRUD
    bool ajouter();
    QSqlQueryModel* afficher();
    bool supprimer(int);
    bool modifier(int, QString, QString, QString, int, QString, int);
    QSqlQueryModel* trier();
    QSqlQueryModel*rechercher(const int &);


};

#endif // ZONES_H
