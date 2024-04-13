static gpointer openssl_global_init_once(gpointer arg)
{
    SSL_library_init();
    SSL_load_error_strings();

    openssl_thread_setup();

    return NULL;
}