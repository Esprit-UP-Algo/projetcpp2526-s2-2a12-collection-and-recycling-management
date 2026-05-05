#ifndef POUBELLES_H
#define POUBELLES_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDate>

class Poubelle {
private:
    int id_poubelle;
    QString localisation;
    int niveau_remplissage;
    int capacite;
    QString type_dechets;
    int id_zone;
    QString etat;
    QDate date_vidange;

public:
    // Constructeurs
    Poubelle();
    Poubelle(int, QString, int, int, QString, int, QString, QDate);

    // Getters
    int getId() const { return id_poubelle; }
    QString getLocalisation() const { return localisation; }
    int getNiveauRemplissage() const { return niveau_remplissage; }
    int getCapacite() const { return capacite; }
    QString getTypeDechets() const { return type_dechets; }
    int getIdZone() const { return id_zone; }
    QString getEtat() const { return etat; }
    QDate getDateVidange() const { return date_vidange; }

    // Setters
    void setId(int id) { id_poubelle = id; }
    void setLocalisation(QString loc) { localisation = loc; }
    void setNiveauRemplissage(int niv) { niveau_remplissage = niv; }
    void setCapacite(int cap) { capacite = cap; }
    void setTypeDechets(QString type) { type_dechets = type; }
    void setIdZone(int zone) { id_zone = zone; }
    void setEtat(QString e) { etat = e; }
    void setDateVidange(QDate d) { date_vidange = d; }

    // Fonctionnalités CRUD
    bool ajouter();
    QSqlQueryModel* afficher();
    bool supprimer(int);
    bool modifier(int id);
};

#endif // POUBELLES_H
