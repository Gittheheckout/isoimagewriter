/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2017  KDE neon <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QGpgME/Protocol>
#include <QGpgME/VerifyDetachedJob>
#include <gpgme++/verificationresult.h>

#include <QDebug>
#include <QFile>

#include <KLocalizedString>

#include "verifyneoniso.h"

VerifyNeonISO::VerifyNeonISO(QString filename) : VerifyISO(filename)
{
}

bool VerifyNeonISO::canVerify() {
    QStringList splits = m_filename.split('/');
    QString fileName = splits[splits.size()-1];
    if (!fileName.startsWith("neon-")) {
        m_error = i18n("Filename does not match KDE neon ISO files");
        return false;
    }
    return true;
}

bool VerifyNeonISO::isValid() {
    if (!verifyFilename()) {
        return false;
    }
    QStringList splits = m_filename.split('/');
    QString fileName = splits[splits.size()-1];
    if (!QFile::exists(m_filename+".sig")) {
        qDebug() << "does not exist .sig" << fileName+".sig";
        m_error = i18n("Could not find %1, please download PGP signature file to same directory.", fileName+".sig");
        return false;
    }
    QFile signatureFile(m_filename + ".sig");
    if (!signatureFile.open(QIODevice::ReadOnly)) {
        qDebug() << "error",signatureFile.errorString();
    }
    QByteArray signatureData = signatureFile.readAll();
    QFile isoFile(m_filename);
    if (!isoFile.open(QIODevice::ReadOnly)) {
        qDebug() << "error",isoFile.errorString();
    }
    QByteArray isoData = signatureFile.readAll();
    QGpgME::VerifyDetachedJob *job = QGpgME::openpgp()->verifyDetachedJob();
    GpgME::VerificationResult result = job->exec(signatureData, isoData);
    qDebug() << "numSignatures " << result.numSignatures();
    qDebug() << "filename " << result.fileName();
    GpgME::Signature signature = result.signature(0);
    qDebug() << "fingerprint " << signature.fingerprint();
    if (strcmp(signature.fingerprint(), "DEACEA00075E1D76") == 0) {
        qDebug() << "Uses right signature!";
    } else {
        qDebug() << "Uses wrong signature!!";
        m_error = i18n("Uses wrong signature.");
        return false;
    }
    if (signature.summary() & GpgME::Signature::KeyRevoked) {
        qDebug() << "Key is revoked" << signature.summary();
        m_error = i18n("Key is revoked.");
        return false;
    }
    return true;
}