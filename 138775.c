unload_preauth_plugins(krb5_context context)
{
    size_t i;

    for (i = 0; i < n_preauth_systems; i++) {
        if (preauth_systems[i].fini)
            preauth_systems[i].fini(context, preauth_systems[i].moddata);
    }
    free(preauth_systems);
    preauth_systems = NULL;
    n_preauth_systems = 0;
}