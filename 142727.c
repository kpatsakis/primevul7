usm_save_user(struct usmUser *user, const char *token, const char *type)
{
    char            line[4096];
    char           *cptr;

    memset(line, 0, sizeof(line));

    sprintf(line, "%s %d %d ", token, user->userStatus,
            user->userStorageType);
    cptr = &line[strlen(line)]; /* the NULL */
    cptr =
        read_config_save_octet_string(cptr, user->engineID,
                                      user->engineIDLen);
    *cptr++ = ' ';
    cptr = read_config_save_octet_string(cptr, (u_char *) user->name,
                                         (user->name == NULL) ? 0 :
                                         strlen(user->name));
    *cptr++ = ' ';
    cptr = read_config_save_octet_string(cptr, (u_char *) user->secName,
                                         (user->secName == NULL) ? 0 :
                                         strlen(user->secName));
    *cptr++ = ' ';
    cptr =
        read_config_save_objid(cptr, user->cloneFrom, user->cloneFromLen);
    *cptr++ = ' ';
    cptr = read_config_save_objid(cptr, user->authProtocol,
                                  user->authProtocolLen);
    *cptr++ = ' ';
    cptr =
        read_config_save_octet_string(cptr, user->authKey,
                                      user->authKeyLen);
    *cptr++ = ' ';
    cptr = read_config_save_objid(cptr, user->privProtocol,
                                  user->privProtocolLen);
    *cptr++ = ' ';
    cptr =
        read_config_save_octet_string(cptr, user->privKey,
                                      user->privKeyLen);
    *cptr++ = ' ';
    cptr = read_config_save_octet_string(cptr, user->userPublicString,
                                         user->userPublicStringLen);

    read_config_store(type, line);
}