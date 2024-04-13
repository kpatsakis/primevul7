get_plugin_vtables(krb5_context context,
                   struct krb5_kdcpreauth_vtable_st **vtables_out,
                   size_t *n_tables_out, size_t *n_systems_out)
{
    krb5_plugin_initvt_fn *plugins = NULL, *pl;
    struct krb5_kdcpreauth_vtable_st *vtables;
    size_t count, n_tables, n_systems, i;

    *vtables_out = NULL;
    *n_tables_out = *n_systems_out = 0;

    /* Auto-register encrypted challenge and (if possible) pkinit. */
    k5_plugin_register_dyn(context, PLUGIN_INTERFACE_KDCPREAUTH, "pkinit",
                           "preauth");
    k5_plugin_register_dyn(context, PLUGIN_INTERFACE_KDCPREAUTH, "otp",
                           "preauth");
    k5_plugin_register_dyn(context, PLUGIN_INTERFACE_KDCPREAUTH, "spake",
                           "preauth");
    k5_plugin_register(context, PLUGIN_INTERFACE_KDCPREAUTH,
                       "encrypted_challenge",
                       kdcpreauth_encrypted_challenge_initvt);
    k5_plugin_register(context, PLUGIN_INTERFACE_KDCPREAUTH,
                       "encrypted_timestamp",
                       kdcpreauth_encrypted_timestamp_initvt);

    if (k5_plugin_load_all(context, PLUGIN_INTERFACE_KDCPREAUTH, &plugins))
        return;
    for (count = 0; plugins[count]; count++);
    vtables = calloc(count + 1, sizeof(*vtables));
    if (vtables == NULL)
        goto cleanup;
    for (pl = plugins, n_tables = 0; *pl != NULL; pl++) {
        if ((*pl)(context, 1, 2, (krb5_plugin_vtable)&vtables[n_tables]) == 0)
            n_tables++;
    }
    for (i = 0, n_systems = 0; i < n_tables; i++) {
        for (count = 0; vtables[i].pa_type_list[count] != 0; count++);
        n_systems += count;
    }
    *vtables_out = vtables;
    *n_tables_out = n_tables;
    *n_systems_out = n_systems;

cleanup:
    k5_plugin_free_modules(context, plugins);
}