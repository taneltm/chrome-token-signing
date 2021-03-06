/*
* Chrome Token Signing Native Host
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "PKCS11CertificateSelector.h"
#include "PKCS11CardManager.h"
#include "Logger.h"

PKCS11CertificateSelector::PKCS11CertificateSelector(std::string _driverPath)
	: CertificateSelector()
	, driverPath(std::move(_driverPath))
{}

std::vector<unsigned char> PKCS11CertificateSelector::getCert() const {
	for (const auto &token : PKCS11CardManager(driverPath).tokens()) {
		PCCERT_CONTEXT cert = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, token.cert.data(), token.cert.size());
		if (!cert)
			continue;
		_log("new certificate handle created.");

		if (!isValid(cert)) {
			CertFreeCertificateContext(cert);
			continue;
		}
		if (CertAddCertificateContextToStore(store, cert, CERT_STORE_ADD_USE_EXISTING, nullptr))
			_log("Certificate added to the memory store.");
		else
			_log("Could not add the certificate to the memory store.");
	}
	return showDialog();
}
