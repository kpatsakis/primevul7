static inline void openssl_global_init(void)
{
    static GOnce openssl_once = G_ONCE_INIT;
    g_once(&openssl_once, openssl_global_init_once, NULL);
}