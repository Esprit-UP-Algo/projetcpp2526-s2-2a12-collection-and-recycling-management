#ifndef MISSIONS_H
#define MISSIONS_H

#include <QString>
#include <QDate>
#include <QTime>
#include <QSqlQuery>
#include <QSqlQueryModel>

class Missions {
    int id;
    QString type;
    QDate date;
    QTime duree;
    int equipe;
    int zone;
    QString etat;
    QString priorite;
    QString description;

public:
    // Constructeurs
    Missions();
    Missions(int, QString, QDate, QTime, int, int, QString, QString, QString);

    // Getters
    int getId() const { return id; }
    QString getType() const { return type; }
    QDate getDate() const { return date; }
    QTime getDuree() const { return duree; }
    int getEquipe() const { return equipe; }
    int getZone() const { return zone; }
    QString getEtat() const { return etat; }
    QString getPriorite() const { return priorite; }
    QString getDescription() const { return description; }

    // Setters
    void setId(int id) { this->id = id; }
    void setType(QString t) { type = t; }
    void setDate(QDate d) { date = d; }
    void setDuree(QTime d) { duree = d; }
    void setEquipe(int e) { equipe = e; }
    void setZone(int z) { zone = z; }
    void setEtat(QString e) { etat = e; }
    void setPriorite(QString p) { priorite = p; }
    void setDescription(QString d) { description = d; }

    // CRUD
    bool ajouter();
    QSqlQueryModel* afficher();
    bool supprimer(int);
    bool modifier();

    // Fonctions avancÃ©es
    QSqlQueryModel* trier(QString critere);
    QSqlQueryModel* rechercher(QString valeur);
};

#endif // MISSIONS_H
