#include "connection.h"
#include <QSqlError>
#include <QDebug>

// Initialisation du pointeur d'instance
Connection* Connection::p_instance = nullptr;

// Constructeur privé
Connection::Connection()
{
    // Initialisation de la base de données
    db = QSqlDatabase::addDatabase("QODBC");
}

// Méthode statique pour obtenir l'instance unique
Connection* Connection::instance()
{
    if (p_instance == nullptr) {
        p_instance = new Connection();
    }
    return p_instance;
}

// Méthode pour établir la connexion
bool Connection::createConnect()
{
    bool test = false;

    db.setDatabaseName("Source_Projet");//inserer le nom de la source de données
    db.setUserName("SAFWCARM");//inserer nom de l'utilisateur
    db.setPassword("projet");//inserer mot de passe de cet utilisateur

    if (db.open()) {
        test = true;
        qDebug() << "Connexion à la base de données réussie";
    } else {
        qDebug() << "Erreur de connexion:" << db.lastError().text();
    }

    return test;
}

// Fermer la connexion
void Connection::closeConnection()
{
    if (db.isOpen()) {
        db.close();
    }
}

// Destructeur privé
Connection::~Connection()
{
    closeConnection();
}

// Créer les tables dans la base de données (Oracle)
bool Connection::createTables()
{
    QSqlQuery query(db);
    
    // Créer table Employes
    if (!query.exec("CREATE TABLE Employes ("
                    "id_employe NUMBER PRIMARY KEY, "
                    "nom VARCHAR2(100) NOT NULL, "
                    "prenom VARCHAR2(100) NOT NULL, "
                    "email VARCHAR2(100), "
                    "username VARCHAR2(100) UNIQUE, "
                    "password VARCHAR2(100), "
                    "telephone VARCHAR2(20), "
                    "poste VARCHAR2(100), "
                    "date_embauche DATE)")) {
        qDebug() << "Erreur création table Employes:" << query.lastError().text();
    } else {
        qDebug() << "Table Employes créée";
    }
    
    // Créer séquence pour Employes
    query.exec("CREATE SEQUENCE seq_employes START WITH 1 INCREMENT BY 1");
    
    // Créer table Equipes
    if (!query.exec("CREATE TABLE Equipes ("
                    "id_equipe NUMBER PRIMARY KEY, "
                    "nom_equipe VARCHAR2(100) NOT NULL, "
                    "description VARCHAR2(255), "
                    "chef_equipe NUMBER, "
                    "nombre_membres NUMBER)")) {
        qDebug() << "Erreur création table Equipes:" << query.lastError().text();
    } else {
        qDebug() << "Table Equipes créée";
    }
    
    // Créer séquence pour Equipes
    query.exec("CREATE SEQUENCE seq_equipes START WITH 1 INCREMENT BY 1");
    
    // Créer table Zones
    if (!query.exec("CREATE TABLE Zones ("
                    "id_zone NUMBER PRIMARY KEY, "
                    "nom_zone VARCHAR2(100) NOT NULL, "
                    "localisation VARCHAR2(255), "
                    "priorite NUMBER, "
                    "status VARCHAR2(50))")) {
        qDebug() << "Erreur création table Zones:" << query.lastError().text();
    } else {
        qDebug() << "Table Zones créée";
    }
    
    // Créer séquence pour Zones
    query.exec("CREATE SEQUENCE seq_zones START WITH 1 INCREMENT BY 1");
    
    // Créer table Poubelles
    if (!query.exec("CREATE TABLE Poubelles ("
                    "id_poubelle NUMBER PRIMARY KEY, "
                    "localisation VARCHAR2(255) NOT NULL, "
                    "niveau_remplissage NUMBER, "
                    "capacite NUMBER, "
                    "type_dechets VARCHAR2(100), "
                    "id_zone NUMBER)")) {
        qDebug() << "Erreur création table Poubelles:" << query.lastError().text();
    } else {
        qDebug() << "Table Poubelles créée";
    }
    
    // Créer séquence pour Poubelles
    query.exec("CREATE SEQUENCE seq_poubelles START WITH 1 INCREMENT BY 1");
    
    // Créer table Missions
    if (!query.exec("CREATE TABLE Missions ("
                    "id_mission NUMBER PRIMARY KEY, "
                    "titre VARCHAR2(100) NOT NULL, "
                    "description VARCHAR2(255), "
                    "date_debut DATE, "
                    "date_fin DATE, "
                    "status VARCHAR2(50), "
                    "id_equipe NUMBER, "
                    "id_zone NUMBER)")) {
        qDebug() << "Erreur création table Missions:" << query.lastError().text();
    } else {
        qDebug() << "Table Missions créée";
    }
    
    // Créer séquence pour Missions
    query.exec("CREATE SEQUENCE seq_missions START WITH 1 INCREMENT BY 1");
    
    qDebug() << "Tables et séquences créées avec succès";
    return true;
}

// Récupérer tous les employés
QSqlQuery Connection::getEmployes()
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM Employes");
    return query;
}

// Récupérer toutes les équipes
QSqlQuery Connection::getEquipes()
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM Equipes");
    return query;
}

// Récupérer toutes les zones
QSqlQuery Connection::getZones()
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM Zones");
    return query;
}

// Récupérer toutes les poubelles
QSqlQuery Connection::getPoubelles()
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM Poubelles");
    return query;
}

// Récupérer toutes les missions
QSqlQuery Connection::getMissions()
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM Missions");
    return query;
}

// Accéder à la base de données
QSqlDatabase Connection::getDatabase()
{
    return db;
}

// Insérer un employé
bool Connection::insertEmploye(const QString& nom, const QString& prenom, const QString& email)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO Employes (id_employe, nom, prenom, email, username, password) "
                  "VALUES (seq_employes.NEXTVAL, :nom, :prenom, :email, :username, :password)");
    query.addBindValue(nom);
    query.addBindValue(prenom);
    query.addBindValue(email);
    query.addBindValue(nom.toLower() + prenom.toLower()); // username par défaut
    query.addBindValue("123456"); // password par défaut
    
    if (!query.exec()) {
        qDebug() << "Erreur insertion employé:" << query.lastError().text();
        return false;
    }
    return true;
}

// Insérer une équipe
bool Connection::insertEquipe(const QString& nom, const QString& description)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO Equipes (id_equipe, nom_equipe, description) "
                  "VALUES (seq_equipes.NEXTVAL, :nom, :description)");
    query.addBindValue(nom);
    query.addBindValue(description);
    
    if (!query.exec()) {
        qDebug() << "Erreur insertion équipe:" << query.lastError().text();
        return false;
    }
    return true;
}

// Insérer une zone
bool Connection::insertZone(const QString& nom, const QString& localisation)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO Zones (id_zone, nom_zone, localisation, priorite) "
                  "VALUES (seq_zones.NEXTVAL, :nom, :localisation, 1)");
    query.addBindValue(nom);
    query.addBindValue(localisation);
    
    if (!query.exec()) {
        qDebug() << "Erreur insertion zone:" << query.lastError().text();
        return false;
    }
    return true;
}

// Insérer une poubelle
bool Connection::insertPoubelle(const QString& localisation, int niveau, int capacite)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO Poubelles (id_poubelle, localisation, niveau_remplissage, capacite) "
                  "VALUES (seq_poubelles.NEXTVAL, :localisation, :niveau, :capacite)");
    query.addBindValue(localisation);
    query.addBindValue(niveau);
    query.addBindValue(capacite);
    
    if (!query.exec()) {
        qDebug() << "Erreur insertion poubelle:" << query.lastError().text();
        return false;
    }
    return true;
}

// Insérer une mission
bool Connection::insertMission(const QString& titre, const QString& description)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO Missions (id_mission, titre, description, status) "
                  "VALUES (seq_missions.NEXTVAL, :titre, :description, 'En cours')");
    query.addBindValue(titre);
    query.addBindValue(description);
    
    if (!query.exec()) {
        qDebug() << "Erreur insertion mission:" << query.lastError().text();
        return false;
    }
    return true;
}

// Authentifier un employé
bool Connection::authenticateEmploye(const QString& username, const QString& password)
{
    QSqlQuery query(db);
    query.prepare("SELECT * FROM Employes WHERE username = :username AND password = :password");
    query.addBindValue(username);
    query.addBindValue(password);
    
    if (!query.exec()) {
        qDebug() << "Erreur authentification:" << query.lastError().text();
        return false;
    }
    
    // Si la requête retourne au moins une ligne, l'authentification est réussie
    return query.next();
}

// Récupérer les informations d'un employé
QSqlQuery Connection::getEmployeByUsername(const QString& username)
{
    QSqlQuery query(db);
    query.prepare("SELECT * FROM Employes WHERE username = :username");
    query.addBindValue(username);
    query.exec();
    return query;
}





