#include "userscertificates.h"
#include <QDebug>
#include <QCoreApplication>

const QString USER_CERTS = "user_certificate/%1";

//Users certificates path HOME$/.config/RBT/QSslServer.conf
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
    if (!contains(QSslCertificate(cert)))
        m_settings.setValue(USER_CERTS.arg(m_clientsNum++), cert);
    qDebug() << Q_FUNC_INFO << m_clientsNum;
}

//certificate must be in Pem encoding.
bool UsersCertificates::contains(const QSslCertificate &cert)
{
    for (int i = 0; i < m_clientsNum; i++)
    {
        if (m_settings.value(USER_CERTS.arg(i)).value<QByteArray>() == cert.toPem())
            return true;
    }
    return false;
}

void UsersCertificates::remove(const QByteArray &cert)
{
    for (int i = 0; i < m_clientsNum; i++)
    {
        if (m_settings.value(USER_CERTS.arg(i)).value<QByteArray>() == cert)
        {
            //If clients num > 3, then replace last of certificates on this place.
            if (m_clientsNum >= 3 && i != m_clientsNum)
            {
                m_settings.setValue(USER_CERTS.arg(i), m_settings.value(USER_CERTS.arg(m_clientsNum-2)).value<QByteArray>());
                m_settings.remove(USER_CERTS.arg(m_clientsNum-2));
            }
            else
                m_settings.remove(USER_CERTS.arg(i));
        }
    }
}
