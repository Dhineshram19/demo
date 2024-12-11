#include "fileprocessor.h"
#include <QDataStream>
#include <QFile>
#include <QUdpSocket>
#include <QHostAddress>
#include <QDebug>

FileProcessor::FileProcessor(const QString &commandFilePath, const QString &iqFilePath,
                             const QPair<QString, quint16> &ipPort, QObject *parent)
    : QObject(parent), m_commandFilePath(commandFilePath), m_iqFilePath(iqFilePath), m_ipPort(ipPort)
{
    if (!m_commandFilePath.isEmpty()) {
        m_commandFile.setFileName(m_commandFilePath);
        m_commandFile.open(QIODevice::ReadOnly);
        m_commandStream.setDevice(&m_commandFile);
    }

    if (!m_iqFilePath.isEmpty()) {
        m_iqFile.setFileName(m_iqFilePath);
        m_iqFile.open(QIODevice::ReadOnly);
        m_iqStream.setDevice(&m_iqFile);
    }
}

bool FileProcessor::processCommandData(int dwellIndex) {
    if (!m_commandFile.isOpen()) {
        qWarning() << "Command file is not open.";
        return false;
    }

    // Move the stream to the correct position based on the dwell index
    m_commandFile.seek(dwellIndex * sizeof(int) * 3);  // Each entry is 3 integers (dwellID, pulseID, rangeCellsID)

    int dwellID, pulseID, rangeCellsID;
    if (!m_commandStream.atEnd()) {
        m_commandStream >> dwellID >> pulseID >> rangeCellsID;
        sendCommandData(dwellID, pulseID, rangeCellsID);
        return true;
    }
    return false;  // Return false when no more data is available
}

void FileProcessor::processIQData(int dwellIndex) {
    if (!m_iqFile.isOpen()) {
        qWarning() << "IQ file is not open.";
        return;
    }

    // Seek to the corresponding position for this dwell index in the IQ file
    // (Assumes each IQ data chunk is the same size; modify as needed for your data structure)
    m_iqFile.seek(dwellIndex * sizeof(float) * 100);  // Assuming 100 float values per dwell data

    QByteArray iqData;
    iqData.resize(sizeof(float) * 100);
    m_iqStream.readRawData(iqData.data(), iqData.size());

    sendIQData(dwellIndex);  // Send IQ data for this dwell index
}

void FileProcessor::sendCommandData(int dwellID, int pulseID, int rangeCellsID) {
    QByteArray commandData;
    QDataStream commandStream(&commandData, QIODevice::WriteOnly);
    commandStream << dwellID << pulseID << rangeCellsID;

    // Send the command data using UDP
    QUdpSocket udpSocket;
    QHostAddress hostAddress(m_ipPort.first);
    udpSocket.writeDatagram(commandData, hostAddress, m_ipPort.second);
}

void FileProcessor::sendIQData(int dwellID) {
    QByteArray iqData;
    QUdpSocket udpSocket;
    QHostAddress hostAddress(m_ipPort.first);
    udpSocket.writeDatagram(iqData, hostAddress, m_ipPort.second);  // Send data for this dwellID
}
