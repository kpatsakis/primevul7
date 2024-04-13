void license_get_server_rsa_public_key(rdpLicense* license)
{
	BYTE* Exponent;
	BYTE* Modulus;
	int ModulusLength;

	if (license->ServerCertificate->length < 1)
	{
		certificate_read_server_certificate(license->certificate,
				license->rdp->settings->ServerCertificate,
				license->rdp->settings->ServerCertificateLength);
	}

	Exponent = license->certificate->cert_info.exponent;
	Modulus = license->certificate->cert_info.Modulus;
	ModulusLength = license->certificate->cert_info.ModulusLength;

	CopyMemory(license->Exponent, Exponent, 4);

	license->ModulusLength = ModulusLength;
	license->Modulus = (BYTE*) malloc(ModulusLength);
	memcpy(license->Modulus, Modulus, ModulusLength);
}