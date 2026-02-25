-- =====================================================
-- EXPORT COMPLET - BASE DE DONNÉES OURLABIB
-- =====================================================
-- Script pour recréer toutes les tables et les données

-- =====================================================
-- 1. SUPPRESSION DES SÉQUENCES (si elles existent)
-- =====================================================
BEGIN
   EXECUTE IMMEDIATE 'DROP SEQUENCE seq_employes';
EXCEPTION
   WHEN OTHERS THEN NULL;
END;
/

BEGIN
   EXECUTE IMMEDIATE 'DROP SEQUENCE seq_equipes';
EXCEPTION
   WHEN OTHERS THEN NULL;
END;
/

BEGIN
   EXECUTE IMMEDIATE 'DROP SEQUENCE seq_zones';
EXCEPTION
   WHEN OTHERS THEN NULL;
END;
/

BEGIN
   EXECUTE IMMEDIATE 'DROP SEQUENCE seq_poubelles';
EXCEPTION
   WHEN OTHERS THEN NULL;
END;
/

BEGIN
   EXECUTE IMMEDIATE 'DROP SEQUENCE seq_missions';
EXCEPTION
   WHEN OTHERS THEN NULL;
END;
/

-- =====================================================
-- 2. SUPPRESSION DES TABLES (si elles existent)
-- =====================================================
BEGIN
   EXECUTE IMMEDIATE 'DROP TABLE Missions';
EXCEPTION
   WHEN OTHERS THEN NULL;
END;
/

BEGIN
   EXECUTE IMMEDIATE 'DROP TABLE Poubelles';
EXCEPTION
   WHEN OTHERS THEN NULL;
END;
/

BEGIN
   EXECUTE IMMEDIATE 'DROP TABLE Zones';
EXCEPTION
   WHEN OTHERS THEN NULL;
END;
/

BEGIN
   EXECUTE IMMEDIATE 'DROP TABLE Equipes';
EXCEPTION
   WHEN OTHERS THEN NULL;
END;
/

BEGIN
   EXECUTE IMMEDIATE 'DROP TABLE Employes';
EXCEPTION
   WHEN OTHERS THEN NULL;
END;
/

-- =====================================================
-- 3. CRÉATION DES TABLES
-- =====================================================

-- Table Employés
CREATE TABLE Employes (
    id_employe NUMBER PRIMARY KEY,
    nom VARCHAR2(100) NOT NULL,
    prenom VARCHAR2(100) NOT NULL,
    email VARCHAR2(100),
    username VARCHAR2(100) UNIQUE,
    password VARCHAR2(100),
    telephone VARCHAR2(20),
    poste VARCHAR2(100),
    date_embauche DATE
);

-- Table Équipes
CREATE TABLE Equipes (
    id_equipe NUMBER PRIMARY KEY,
    nom_equipe VARCHAR2(100) NOT NULL,
    description VARCHAR2(255),
    chef_equipe NUMBER,
    nombre_membres NUMBER
);

-- Table Zones
CREATE TABLE Zones (
    id_zone NUMBER PRIMARY KEY,
    nom_zone VARCHAR2(100) NOT NULL,
    localisation VARCHAR2(255),
    priorite NUMBER,
    status VARCHAR2(50)
);

-- Table Poubelles
CREATE TABLE Poubelles (
    id_poubelle NUMBER PRIMARY KEY,
    localisation VARCHAR2(255) NOT NULL,
    niveau_remplissage NUMBER,
    capacite NUMBER,
    type_dechets VARCHAR2(100),
    id_zone NUMBER
);

-- Table Missions
CREATE TABLE Missions (
    id_mission NUMBER PRIMARY KEY,
    titre VARCHAR2(100) NOT NULL,
    description VARCHAR2(255),
    date_debut DATE,
    date_fin DATE,
    status VARCHAR2(50),
    id_equipe NUMBER,
    id_zone NUMBER
);

-- =====================================================
-- 4. CRÉATION DES SÉQUENCES
-- =====================================================

CREATE SEQUENCE seq_employes START WITH 4 INCREMENT BY 1;
CREATE SEQUENCE seq_equipes START WITH 3 INCREMENT BY 1;
CREATE SEQUENCE seq_zones START WITH 3 INCREMENT BY 1;
CREATE SEQUENCE seq_poubelles START WITH 3 INCREMENT BY 1;
CREATE SEQUENCE seq_missions START WITH 2 INCREMENT BY 1;

-- =====================================================
-- 5. INSERTION DES DONNÉES
-- =====================================================

-- Données Employés
INSERT INTO Employes (id_employe, nom, prenom, email, username, password, telephone, poste, date_embauche)
VALUES (1, 'Dupont', 'Jean', 'jean@email.com', 'dupantjean', '123456', NULL, NULL, NULL);

INSERT INTO Employes (id_employe, nom, prenom, email, username, password, telephone, poste, date_embauche)
VALUES (2, 'Martin', 'Marie', 'marie@email.com', 'martinmarie', '123456', NULL, NULL, NULL);

INSERT INTO Employes (id_employe, nom, prenom, email, username, password, telephone, poste, date_embauche)
VALUES (3, 'Durand', 'Pierre', 'pierre@email.com', 'durandpierre', '123456', NULL, NULL, NULL);

-- Données Équipes
INSERT INTO Equipes (id_equipe, nom_equipe, description, chef_equipe, nombre_membres)
VALUES (1, 'Équipe A', 'Nettoyage secteur nord', NULL, NULL);

INSERT INTO Equipes (id_equipe, nom_equipe, description, chef_equipe, nombre_membres)
VALUES (2, 'Équipe B', 'Nettoyage secteur sud', NULL, NULL);

-- Données Zones
INSERT INTO Zones (id_zone, nom_zone, localisation, priorite, status)
VALUES (1, 'Zone Nord', 'Bâtiment A', 1, NULL);

INSERT INTO Zones (id_zone, nom_zone, localisation, priorite, status)
VALUES (2, 'Zone Sud', 'Bâtiment B', 1, NULL);

-- Données Poubelles
INSERT INTO Poubelles (id_poubelle, localisation, niveau_remplissage, capacite, type_dechets, id_zone)
VALUES (1, 'Couloir 1', 50, 100, NULL, NULL);

INSERT INTO Poubelles (id_poubelle, localisation, niveau_remplissage, capacite, type_dechets, id_zone)
VALUES (2, 'Couloir 2', 75, 100, NULL, NULL);

-- Données Missions
INSERT INTO Missions (id_mission, titre, description, date_debut, date_fin, status, id_equipe, id_zone)
VALUES (1, 'Nettoyage', 'Nettoyer tous les poubelles', NULL, NULL, 'En cours', NULL, NULL);

-- =====================================================
-- 6. VALIDATION DES MODIFICATIONS
-- =====================================================

COMMIT;

-- =====================================================
-- 7. VÉRIFICATION
-- =====================================================

SELECT 'Employés' AS Table_Name, COUNT(*) AS NB_Lignes FROM Employes
UNION ALL
SELECT 'Équipes', COUNT(*) FROM Equipes
UNION ALL
SELECT 'Zones', COUNT(*) FROM Zones
UNION ALL
SELECT 'Poubelles', COUNT(*) FROM Poubelles
UNION ALL
SELECT 'Missions', COUNT(*) FROM Missions;
