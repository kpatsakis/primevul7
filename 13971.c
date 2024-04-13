static void openssl_thread_setup(void)
{
    int i;

    /* Somebody else already setup threading for OpenSSL,
     * don't do it twice to avoid possible races.
     */
    if (CRYPTO_get_locking_callback() != NULL) {
        red_dump_openssl_errors();
        return;
    }

    lock_cs = (pthread_mutex_t*) OPENSSL_malloc(CRYPTO_num_locks() * sizeof(pthread_mutex_t));

    for (i = 0; i < CRYPTO_num_locks(); i++) {
        pthread_mutex_init(&(lock_cs[i]), NULL);
    }

    CRYPTO_THREADID_set_callback(pthreads_thread_id);
    CRYPTO_set_locking_callback(pthreads_locking_callback);
}