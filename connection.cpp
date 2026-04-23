#include "connection.h"
#include <QDebug>
#include <QSqlError>

Connection* Connection::p_instance = nullptr;

Connection::Connection() {
    db = QSqlDatabase::addDatabase("QODBC");
}

Connection *Connection::instance() {
    if (p_instance == nullptr) {
        p_instance = new Connection();
    }
    return p_instance;
}

bool Connection::createConnect() {
    bool test = false;
    db.setDatabaseName("Ourlabib");
    db.setUserName("System");
    db.setPassword("123");

    if (db.open()) {
        test = true;
        qDebug() << "Connexion à la base de données réussie";
    } else {
        qDebug() << "Erreur de connexion:" << db.lastError().text();
    }

    return test;
}

void Connection::closeConnection() {
    if (db.isOpen()) {
        db.close();
    }
}

Connection::~Connection() { closeConnection(); }

QSqlDatabase Connection::getDatabase() { return db; }
