#ifndef ARDUINO_H
#define ARDUINO_H

#include <QSerialPort>
#include <QString>
#include <QDebug>

class Arduino
{
public:
    Arduino();
    ~Arduino();
    
    QString getarduino_port_name();
    QSerialPort *getserial();
    int connect_arduino();
    int close_arduino();
    int read_number_from_arduino();
    QByteArray read_from_arduino();
    void write_to_arduino(const QString &d);

private:
    QString data;
    QString arduino_port_name;
    bool arduino_is_available;
    QSerialPort *serial;
    
    static const quint16 arduino_uno_vendor_id = 9025; // Arduino LLC
    static const quint16 arduino_uno_product_id = 67;  // Arduino Uno
};

#endif // ARDUINO_H