#include "userscertificates.h"
#include <QDebug>
#include <QCoreApplication>

const QString USER_CERTS = "user_certificate/%1";

UsersCertificates::UsersCertificates() : m_settings("RBT", "QSslServer")
{
    int i = 0;
    while(true)
    {
        if (m_settings.contains(USER_CERTS.arg(i)))
        {
            i++;
            m_clientsNum = i;
            qDebug() << Q_FUNC_INFO << m_clientsNum;
        }
        else
        {
            break;
        }
    }
}

void UsersCertificates::add(const QByteArray &cert)
{
    m_settings.setValue(USER_CERTS.arg(m_clientsNum++), cert);
    qDebug() << Q_FUNC_INFO << m_clientsNum;
}

bool UsersCertificates::contains(const QSslCertificate &cert)
{
    for (int i = 0; i < m_clientsNum; i++)
    {
        if (m_settings.value(USER_CERTS.arg(i)).value<QByteArray>() == cert.toPem())
            return true;
    }
    return false;
}




