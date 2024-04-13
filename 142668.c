init_usm_conf(const char *app)
{
    register_config_handler(app, "usmUser",
                                  usm_parse_config_usmUser, NULL, NULL);
    register_config_handler(app, "createUser",
                                  usm_parse_create_usmUser, NULL,
                                  "username [-e ENGINEID] (MD5|SHA|SHA-512|SHA-384|SHA-256|SHA-224|default) authpassphrase [(DES|AES|default) [privpassphrase]]");

    /*
     * we need to be called back later
     */
    snmp_register_callback(SNMP_CALLBACK_LIBRARY, SNMP_CALLBACK_STORE_DATA,
                           usm_store_users, NULL);
}