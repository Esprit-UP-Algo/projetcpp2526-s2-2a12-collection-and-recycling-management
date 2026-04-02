#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSqlDatabase>

/**
 * @brief Singleton class to manage the database connection.
 */
class Connection {
private:
    Connection();
    ~Connection();
    static Connection* p_instance;
    QSqlDatabase db;

public:
    static Connection *instance();
    bool createConnect();
    void closeConnection();
    QSqlDatabase getDatabase();
};

#endif // CONNECTION_H
