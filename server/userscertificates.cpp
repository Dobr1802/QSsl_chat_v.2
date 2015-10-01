#include "userscertificates.h"
#include <QDebug>
#include <QCoreApplication>

const QString USER_CERTS = "user_certificate/%1";
const QString USER_ID = "usernum";

//Users certificates path HOME$/.config/RBT/QSslServer.conf
UsersCertificates::UsersCertificates() : m_settings("RBT", "QSslServer")
{
    if (m_settings.contains(USER_ID))
        m_unicID = m_settings.value(USER_ID).toInt();
    else
        m_settings.setValue(USER_ID, 0);
    qDebug() << Q_FUNC_INFO << m_unicID << "  |||  " << m_settings.allKeys();
}

void UsersCertificates::add(const QByteArray &cert)
{
    if (!contains(cert))
    {
        m_settings.setValue(USER_CERTS.arg(m_unicID++), cert);
        m_settings.setValue(USER_ID, m_unicID);
    }
    qDebug() << Q_FUNC_INFO << m_unicID;
}

//certificate must be in Pem encoding.
bool UsersCertificates::contains(const QByteArray &cert)
{
    QStringList keys = m_settings.allKeys();
    for (auto k : keys)
    {
        if (m_settings.value(k).value<QByteArray>() == cert)
            return true;
    }
    return false;
}

void UsersCertificates::removeByCertificate(const QByteArray &cert)
{
    QStringList keys = m_settings.allKeys();
    for (auto k : keys)
    {
        if (m_settings.value(k).value<QByteArray>() == cert)
        {
            m_settings.remove(k);
            break;
        }
    }
}

void UsersCertificates::removeByKey(const QByteArray &key)
{
    removeByCertificate(m_settings.value(key).value<QByteArray>());
}

QStringList UsersCertificates::list()
{
    QStringList list = m_settings.allKeys();
    list.removeOne(USER_ID);
    return list;
}
