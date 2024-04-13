usm_get_user_from_list(u_char * engineID, size_t engineIDLen,
                       char *name, struct usmUser *puserList,
                       int use_default)
{
    struct usmUser *ptr;
    char            noName[] = "";
    if (name == NULL)
        name = noName;
    for (ptr = puserList; ptr != NULL; ptr = ptr->next) {
        if (ptr->name && !strcmp(ptr->name, name)) {
          DEBUGMSGTL(("usm", "match on user %s\n", ptr->name));
          if (ptr->engineIDLen == engineIDLen &&
            ((ptr->engineID == NULL && engineID == NULL) ||
             (ptr->engineID != NULL && engineID != NULL &&
              memcmp(ptr->engineID, engineID, engineIDLen) == 0)))
            return ptr;
          DEBUGMSGTL(("usm", "no match on engineID ("));
          if (engineID) {
              DEBUGMSGHEX(("usm", engineID, engineIDLen));
          } else {
              DEBUGMSGTL(("usm", "Empty EngineID"));
          }
          DEBUGMSG(("usm", ")\n"));
        }
    }

    /*
     * return "" user used to facilitate engineID discovery
     */
    if (use_default && !strcmp(name, ""))
        return noNameUser;
    return NULL;
}