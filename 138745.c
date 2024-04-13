load_preauth_plugins(struct server_handle *handle, krb5_context context,
                     verto_ctx *ctx)
{
    krb5_error_code ret;
    struct krb5_kdcpreauth_vtable_st *vtables = NULL, *vt;
    size_t n_systems, n_tables, i, j;
    krb5_kdcpreauth_moddata moddata;
    const char **realm_names = NULL, *emsg;
    preauth_system *sys;

    /* Get all available kdcpreauth vtables. */
    get_plugin_vtables(context, &vtables, &n_tables, &n_systems);

    /* Allocate the list of static and plugin preauth systems. */
    preauth_systems = calloc(n_systems + 1, sizeof(preauth_system));
    if (preauth_systems == NULL)
        goto cleanup;

    if (get_realm_names(handle, &realm_names))
        goto cleanup;

    /* Add the dynamically-loaded mechanisms to the list. */
    n_systems = 0;
    for (i = 0; i < n_tables; i++) {
        /* Try to initialize this module. */
        vt = &vtables[i];
        moddata = NULL;
        if (vt->init) {
            ret = vt->init(context, &moddata, realm_names);
            if (ret) {
                emsg = krb5_get_error_message(context, ret);
                krb5_klog_syslog(LOG_ERR, _("preauth %s failed to "
                                            "initialize: %s"), vt->name, emsg);
                krb5_free_error_message(context, emsg);
                continue;
            }
        }

        if (vt->loop) {
            ret = vt->loop(context, moddata, ctx);
            if (ret) {
                emsg = krb5_get_error_message(context, ret);
                krb5_klog_syslog(LOG_ERR, _("preauth %s failed to setup "
                                            "loop: %s"), vt->name, emsg);
                krb5_free_error_message(context, emsg);
                if (vt->fini)
                    vt->fini(context, moddata);
                continue;
            }
        }

        /* Add this module to the systems list once for each pa type. */
        for (j = 0; vt->pa_type_list[j] != 0; j++) {
            sys = &preauth_systems[n_systems];
            sys->name = vt->name;
            sys->type = vt->pa_type_list[j];
            sys->flags = (vt->flags) ? vt->flags(context, sys->type) : 0;
            sys->moddata = moddata;
            sys->init = vt->init;
            /* Only call fini once for each plugin. */
            sys->fini = (j == 0) ? vt->fini : NULL;
            sys->get_edata = vt->edata;
            sys->verify_padata = vt->verify;
            sys->return_padata = vt->return_padata;
            sys->free_modreq = vt->free_modreq;
            sys->loop = vt->loop;
            n_systems++;
        }
    }
    n_preauth_systems = n_systems;
    /* Add the end-of-list marker. */
    preauth_systems[n_systems].name = "[end]";
    preauth_systems[n_systems].type = -1;

cleanup:
    free(vtables);
    free(realm_names);
}