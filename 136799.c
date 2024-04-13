PHP_METHOD(Phar, getSignature)
{
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (phar_obj->archive->signature) {
		zend_string *unknown;

		array_init(return_value);
		add_assoc_stringl(return_value, "hash", phar_obj->archive->signature, phar_obj->archive->sig_len);
		switch(phar_obj->archive->sig_flags) {
			case PHAR_SIG_MD5:
				add_assoc_stringl(return_value, "hash_type", "MD5", 3);
				break;
			case PHAR_SIG_SHA1:
				add_assoc_stringl(return_value, "hash_type", "SHA-1", 5);
				break;
			case PHAR_SIG_SHA256:
				add_assoc_stringl(return_value, "hash_type", "SHA-256", 7);
				break;
			case PHAR_SIG_SHA512:
				add_assoc_stringl(return_value, "hash_type", "SHA-512", 7);
				break;
			case PHAR_SIG_OPENSSL:
				add_assoc_stringl(return_value, "hash_type", "OpenSSL", 7);
				break;
			default:
				unknown = strpprintf(0, "Unknown (%u)", phar_obj->archive->sig_flags);
				add_assoc_str(return_value, "hash_type", unknown);
				break;
		}
	} else {
		RETURN_FALSE;
	}
}