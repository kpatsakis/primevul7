openSSLHandle(int sock, char *hostname, char **p_cert)
{
    SSL *handle = NULL;
    static char *old_ssl_forbid_method = NULL;
#ifdef USE_SSL_VERIFY
    static int old_ssl_verify_server = -1;
#endif

    if (old_ssl_forbid_method != ssl_forbid_method
	&& (!old_ssl_forbid_method || !ssl_forbid_method ||
	    strcmp(old_ssl_forbid_method, ssl_forbid_method))) {
	old_ssl_forbid_method = ssl_forbid_method;
#ifdef USE_SSL_VERIFY
	ssl_path_modified = 1;
#else
	free_ssl_ctx();
#endif
    }
#ifdef USE_SSL_VERIFY
    if (old_ssl_verify_server != ssl_verify_server) {
	old_ssl_verify_server = ssl_verify_server;
	ssl_path_modified = 1;
    }
    if (ssl_path_modified) {
	free_ssl_ctx();
	ssl_path_modified = 0;
    }
#endif				/* defined(USE_SSL_VERIFY) */
    if (ssl_ctx == NULL) {
	int option;
#if SSLEAY_VERSION_NUMBER < 0x0800
	ssl_ctx = SSL_CTX_new();
	X509_set_default_verify_paths(ssl_ctx->cert);
#else				/* SSLEAY_VERSION_NUMBER >= 0x0800 */
	SSLeay_add_ssl_algorithms();
	SSL_load_error_strings();
	if (!(ssl_ctx = SSL_CTX_new(SSLv23_client_method())))
	    goto eend;
	SSL_CTX_set_cipher_list(ssl_ctx, "DEFAULT:!LOW:!RC4:!EXP");
	option = SSL_OP_ALL;
	if (ssl_forbid_method) {
	    if (strchr(ssl_forbid_method, '2'))
		option |= SSL_OP_NO_SSLv2;
	    if (strchr(ssl_forbid_method, '3'))
		option |= SSL_OP_NO_SSLv3;
	    if (strchr(ssl_forbid_method, 't'))
		option |= SSL_OP_NO_TLSv1;
	    if (strchr(ssl_forbid_method, 'T'))
		option |= SSL_OP_NO_TLSv1;
	}
#ifdef SSL_OP_NO_COMPRESSION
	option |= SSL_OP_NO_COMPRESSION;
#endif
	SSL_CTX_set_options(ssl_ctx, option);

#ifdef SSL_MODE_RELEASE_BUFFERS
	SSL_CTX_set_mode (ssl_ctx, SSL_MODE_RELEASE_BUFFERS);
#endif

#ifdef USE_SSL_VERIFY
	/* derived from openssl-0.9.5/apps/s_{client,cb}.c */
#if 1				/* use SSL_get_verify_result() to verify cert */
	SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_NONE, NULL);
#else
	SSL_CTX_set_verify(ssl_ctx,
			   ssl_verify_server ? SSL_VERIFY_PEER :
			   SSL_VERIFY_NONE, NULL);
#endif
	if (ssl_cert_file != NULL && *ssl_cert_file != '\0') {
	    int ng = 1;
	    if (SSL_CTX_use_certificate_file
		(ssl_ctx, ssl_cert_file, SSL_FILETYPE_PEM) > 0) {
		char *key_file = (ssl_key_file == NULL
				  || *ssl_key_file ==
				  '\0') ? ssl_cert_file : ssl_key_file;
		if (SSL_CTX_use_PrivateKey_file
		    (ssl_ctx, key_file, SSL_FILETYPE_PEM) > 0)
		    if (SSL_CTX_check_private_key(ssl_ctx))
			ng = 0;
	    }
	    if (ng) {
		free_ssl_ctx();
		goto eend;
	    }
	}
	if ((!ssl_ca_file && !ssl_ca_path)
	    || SSL_CTX_load_verify_locations(ssl_ctx, ssl_ca_file, ssl_ca_path))
#endif				/* defined(USE_SSL_VERIFY) */
	    SSL_CTX_set_default_verify_paths(ssl_ctx);
#endif				/* SSLEAY_VERSION_NUMBER >= 0x0800 */
    }
    handle = SSL_new(ssl_ctx);
    SSL_set_fd(handle, sock);
#if SSLEAY_VERSION_NUMBER >= 0x00905100
    init_PRNG();
#endif				/* SSLEAY_VERSION_NUMBER >= 0x00905100 */
#if (SSLEAY_VERSION_NUMBER >= 0x00908070) && !defined(OPENSSL_NO_TLSEXT)
    SSL_set_tlsext_host_name(handle,hostname);
#endif				/* (SSLEAY_VERSION_NUMBER >= 0x00908070) && !defined(OPENSSL_NO_TLSEXT) */
    if (SSL_connect(handle) > 0) {
	Str serv_cert = ssl_get_certificate(handle, hostname);
	if (serv_cert) {
	    *p_cert = serv_cert->ptr;
	    return handle;
	}
	close(sock);
	SSL_free(handle);
	return NULL;
    }
  eend:
    close(sock);
    if (handle)
	SSL_free(handle);
    /* FIXME: gettextize? */
    disp_err_message(Sprintf
		     ("SSL error: %s",
		      ERR_error_string(ERR_get_error(), NULL))->ptr, FALSE);
    return NULL;
}