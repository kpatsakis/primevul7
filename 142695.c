usm_set_password(const char *token, char *line)
{
    char           *cp;
    char            nameBuf[SNMP_MAXBUF];
    u_char         *engineID = NULL;
    size_t          engineIDLen = 0;
    struct usmUser *user;

    cp = copy_nword(line, nameBuf, sizeof(nameBuf));
    if (cp == NULL) {
        config_perror("invalid name specifier");
        return;
    }

    DEBUGMSGTL(("usm", "comparing: %s and %s\n", cp, WILDCARDSTRING));
    if (strncmp(cp, WILDCARDSTRING, strlen(WILDCARDSTRING)) == 0) {
        /*
         * match against all engineIDs we know about 
         */
        cp = skip_token(cp);
        for (user = userList; user != NULL; user = user->next) {
            if (user->secName && strcmp(user->secName, nameBuf) == 0) {
                usm_set_user_password(user, token, cp);
            }
        }
    } else {
        cp = read_config_read_octet_string(cp, &engineID, &engineIDLen);
        if (cp == NULL) {
            config_perror("invalid engineID specifier");
            SNMP_FREE(engineID);
            return;
        }

        user = usm_get_user(engineID, engineIDLen, nameBuf);
        if (user == NULL) {
            config_perror("not a valid user/engineID pair");
            SNMP_FREE(engineID);
            return;
        }
        usm_set_user_password(user, token, cp);
        SNMP_FREE(engineID);
    }
}