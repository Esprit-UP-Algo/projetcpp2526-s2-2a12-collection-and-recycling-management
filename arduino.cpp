#include "arduino.h"
#include <QSerialPortInfo>

Arduino::Arduino()
{
    data="";
    arduino_port_name="";
    arduino_is_available=false;
    serial=new QSerialPort;
}

Arduino::~Arduino()
{
    if (serial) {
        delete serial;
        serial = nullptr;
    }
}

QString Arduino::getarduino_port_name()
{
    return arduino_port_name;
}

QSerialPort *Arduino::getserial()
{
    return serial;
}
int Arduino::connect_arduino()
{   // recherche du port sur lequel la carte arduino identifée par  arduino_uno_vendor_id
    // est connectée
    foreach (const QSerialPortInfo &serial_port_info, QSerialPortInfo::availablePorts()){
        if(serial_port_info.hasVendorIdentifier() && serial_port_info.hasProductIdentifier()){
            if(serial_port_info.vendorIdentifier() == arduino_uno_vendor_id && serial_port_info.productIdentifier()
                                                                                      == arduino_uno_product_id) {
                arduino_is_available = true;
                arduino_port_name=serial_port_info.portName();
            }
        }
    }
    qDebug() << "arduino_port_name is :" << arduino_port_name;
    if(arduino_is_available){ // configuration de la communication ( débit...)
        serial->setPortName(arduino_port_name);
        if(serial->open(QSerialPort::ReadWrite)){
            serial->setBaudRate(QSerialPort::Baud9600); // Arduino uses 9600 baud
            serial->setDataBits(QSerialPort::Data8); //Longueur des données : 8 bits,
            serial->setParity(QSerialPort::NoParity); //1 bit de parité optionnel
            serial->setStopBits(QSerialPort::OneStop); //Nombre de bits de stop : 1
            serial->setFlowControl(QSerialPort::NoFlowControl);
            return 0;
        }
        return 1;
    }
    return -1;
}

int Arduino::close_arduino()

{

    if(serial->isOpen()){
        serial->close();
        return 0;
    }
    return 1;


}

int Arduino::read_number_from_arduino() {
    if (serial->isReadable()) {
        QByteArray rawData = serial->readAll(); // Read all available data
        QString dataString = QString::fromUtf8(rawData); // Convert to QString

        // Ensure the data is properly formatted
        dataString = dataString.trimmed(); // Remove whitespace or newline characters

        // Convert to integer
        bool ok;
        int number = dataString.toInt(&ok); // Convert to int, set ok flag

        if (ok) {
            return number; // Return the valid number
        } else {
            qDebug() << "Received data is not a valid number:" << dataString;
            return -1; // Return -1 if not a valid number
        }
    }

    qDebug() << "No data available to read.";
    return -1; // Return -1 if no data is available
}

QByteArray Arduino::read_from_arduino()
{
    if(serial->isReadable()){
        return serial->readAll(); //récupérer les données reçues
    }
    return QByteArray(); // Return empty byte array if no data available
}

//hnee nb3thou f stringgggggggggggggggggggggggggg///
void Arduino::write_to_arduino(const QString &d)
{
    if (serial->isWritable()) {

        QByteArray data = d.toUtf8();


        serial->write(data);
    } else {
        qDebug() << "ma7abechh yt7allll ";
    }
}
