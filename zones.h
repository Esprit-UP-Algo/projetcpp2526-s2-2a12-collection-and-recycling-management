#ifndef ZONES_H
#define ZONES_H

#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QString>

class zones
{
private:
    int id_zone;
    int nombre_poubelle;
    int population;
    QString nom_zone;
    QString localisation;
    QString priorite;
    double superficie;

public:
    zones();
    zones(int, QString, QString, QString, int, double, int);

    // Getters
    int getId_zone() const { return id_zone; }
    QString getNom_zone() const { return nom_zone; }
    QString getLocalisation() const { return localisation; }
    QString getPriorite() const { return priorite; }
    int getPopulation() const { return population; }
    double getSuperficie() const { return superficie; }
    int getNombre_poubelle() const { return nombre_poubelle; }

    // Setters
    void setId_zone(int x) { id_zone = x; }
    void setNom_zone(QString n) { nom_zone = n; }
    void setLocalisation(QString l) { localisation = l; }
    void setPriorite(QString p) { priorite = p; }
    void setPopulation(int p) { population = p; }
    void setSuperficie(double s) { superficie = s; }
    void setNombre_poubelle(int n) { nombre_poubelle = n; }

    // Fonctions CRUD
    bool ajouter();
    QSqlQueryModel* afficher();
    bool supprimer(int);
    bool modifier(int);
    QSqlQueryModel* trier();
    QSqlQueryModel* rechercher(const int &);
};

#endif // ZONES_H
