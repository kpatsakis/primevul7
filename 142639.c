usm_get_user(u_char * engineID, size_t engineIDLen, char *name)
{
    DEBUGMSGTL(("usm", "getting user %s\n", name));
    return usm_get_user_from_list(engineID, engineIDLen, name, userList,
                                  1);
}