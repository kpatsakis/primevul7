SPICE_GNUC_VISIBLE int spice_server_set_tls(SpiceServer *s, int port,
                                            const char *ca_cert_file, const char *certs_file,
                                            const char *private_key_file, const char *key_passwd,
                                            const char *dh_key_file, const char *ciphersuite)
{
    if (port == 0 || ca_cert_file == NULL || certs_file == NULL ||
        private_key_file == NULL) {
        return -1;
    }
    if (port < 0 || port > 0xffff) {
        return -1;
    }
    memset(&s->config->ssl_parameters, 0, sizeof(s->config->ssl_parameters));

    s->config->spice_secure_port = port;
    g_strlcpy(s->config->ssl_parameters.ca_certificate_file, ca_cert_file,
              sizeof(s->config->ssl_parameters.ca_certificate_file));
    g_strlcpy(s->config->ssl_parameters.certs_file, certs_file,
              sizeof(s->config->ssl_parameters.certs_file));
    g_strlcpy(s->config->ssl_parameters.private_key_file, private_key_file,
              sizeof(s->config->ssl_parameters.private_key_file));

    if (key_passwd) {
        g_strlcpy(s->config->ssl_parameters.keyfile_password, key_passwd,
                  sizeof(s->config->ssl_parameters.keyfile_password));
    }
    if (ciphersuite) {
        g_strlcpy(s->config->ssl_parameters.ciphersuite, ciphersuite,
                  sizeof(s->config->ssl_parameters.ciphersuite));
    }
    if (dh_key_file) {
        g_strlcpy(s->config->ssl_parameters.dh_key_file, dh_key_file,
                  sizeof(s->config->ssl_parameters.dh_key_file));
    }
    return 0;
}