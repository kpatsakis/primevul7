free_ssl_ctx()
{
    if (ssl_ctx != NULL)
	SSL_CTX_free(ssl_ctx);
    ssl_ctx = NULL;
    ssl_accept_this_site(NULL);
}