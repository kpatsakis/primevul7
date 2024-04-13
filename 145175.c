PHPAPI int php_handle_auth_data(const char *auth)
{
	int ret = -1;

	if (auth && auth[0] != '\0' && strncmp(auth, "Basic ", 6) == 0) {
		char *pass;
		zend_string *user;

		user = php_base64_decode((const unsigned char*)auth + 6, strlen(auth) - 6);
		if (user) {
			pass = strchr(ZSTR_VAL(user), ':');
			if (pass) {
				*pass++ = '\0';
				SG(request_info).auth_user = estrndup(ZSTR_VAL(user), ZSTR_LEN(user));
				SG(request_info).auth_password = estrdup(pass);
				ret = 0;
			}
			zend_string_free(user);
		}
	}

	if (ret == -1) {
		SG(request_info).auth_user = SG(request_info).auth_password = NULL;
	} else {
		SG(request_info).auth_digest = NULL;
	}

	if (ret == -1 && auth && auth[0] != '\0' && strncmp(auth, "Digest ", 7) == 0) {
		SG(request_info).auth_digest = estrdup(auth + 7);
		ret = 0;
	}

	if (ret == -1) {
		SG(request_info).auth_digest = NULL;
	}

	return ret;
}