#ifndef CONNECTION_H
#define CONNECTION_H

#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>

class Connection
{
public:
    static Connection* instance(); // Accès à l'instance unique
    bool createConnect(); // Méthode pour créer la connexion
    void closeConnection(); // Fermer la connexion
    bool createTables(); // Créer les tables
    QSqlQuery getEmployes(); // Récupérer les employés
    QSqlQuery getEquipes(); // Récupérer les équipes
    QSqlQuery getZones(); // Récupérer les zones
    QSqlQuery getPoubelles(); // Récupérer les poubelles
    QSqlQuery getMissions(); // Récupérer les missions
    QSqlDatabase getDatabase(); // Accéder à la base de données
    
    // Méthodes pour insérer des données
    bool insertEmploye(const QString& nom, const QString& prenom, const QString& email);
    bool insertEquipe(const QString& nom, const QString& description);
    bool insertZone(const QString& nom, const QString& localisation);
    bool insertPoubelle(const QString& localisation, int niveau, int capacite);
    bool insertMission(const QString& titre, const QString& description);
    
    // Authentification
    bool authenticateEmploye(const QString& username, const QString& password);
    QSqlQuery getEmployeByUsername(const QString& username);

private:
    Connection(); // Constructeur privé
    ~Connection(); // Destructeur privé
    Connection(const Connection&) = delete; // Supprimer le constructeur de copie
    Connection& operator=(const Connection&) = delete; // Supprimer l'opérateur d'affectation

    static Connection* p_instance; // Pointeur vers l'instance unique
    QSqlDatabase db;
};

#endif // CONNECTION_H

