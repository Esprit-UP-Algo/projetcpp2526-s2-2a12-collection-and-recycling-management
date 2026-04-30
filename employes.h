#ifndef EMPLOYES_H
#define EMPLOYES_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>

class Employe
{
public:
    // Constructeurs
    Employe();
    Employe(int id, QString nom, QString email, QString poste,
            QString disponibilite, int heuresTravail,
            double salaire, QString sexe, QString rfidUid, int idEquipe = 1);

    // CRUD
    bool ajouter();
    bool supprimer(int id);
    bool modifier(int id);
    QSqlQueryModel* afficher();
    QSqlQueryModel* rechercher(const QString &valeur, const QString &critere);
    QSqlQueryModel* trier(const QString &critere, const QString &ordre);

    // Getters
    int     getId()            const { return id; }
    QString getNom()           const { return nom; }
    QString getEmail()         const { return email; }
    QString getPoste()         const { return poste; }
    QString getDisponibilite() const { return disponibilite; }
    int     getHeuresTravail() const { return heuresTravail; }
    double  getSalaire()       const { return salaire; }
    QString getSexe()          const { return sexe; }
    QString getRfidUid()       const { return rfid_uid; }
    int     getIdEquipe()      const { return idEquipe; }

    // Setters
    void setNom(const QString &n)           { nom = n; }
    void setEmail(const QString &e)         { email = e; }
    void setPoste(const QString &p)         { poste = p; }
    void setDisponibilite(const QString &d) { disponibilite = d; }
    void setHeuresTravail(int h)            { heuresTravail = h; }
    void setSalaire(double s)               { salaire = s; }
    void setSexe(const QString &s)          { sexe = s; }
    void setRfidUid(const QString &r)       { rfid_uid = r; }
    void setIdEquipe(int id)                { idEquipe = id; }

private:
    int     id;
    QString nom;
    QString email;
    QString poste;
    QString disponibilite;
    int     heuresTravail;
    double  salaire;
    QString sexe;
    QString rfid_uid;
    int     idEquipe;
};

#endif // EMPLOYES_H
