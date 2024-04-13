sync_cookie_isvalid(Sync_Cookie *testcookie, Sync_Cookie *refcookie)
{
    /* client and server info must match */
    if (testcookie == NULL || refcookie == NULL) {
        return 0;
    }
    if ((testcookie->openldap_compat != refcookie->openldap_compat ||
         strcmp(testcookie->cookie_client_signature, refcookie->cookie_client_signature) ||
         testcookie->cookie_change_info == -1 ||
         testcookie->cookie_change_info > refcookie->cookie_change_info)) {
        return 0;
    }

    if (refcookie->openldap_compat) {
        if (testcookie->cookie_server_signature != NULL ||
            refcookie->cookie_server_signature != NULL) {
            return 0;
        }
    } else {
        if (strcmp(testcookie->cookie_server_signature, refcookie->cookie_server_signature)) {
            return 0;
        }
    }
    /* could add an additional check if the requested state in client cookie is still
     * available. Accept any state request for now.
     */
    return 1;
}