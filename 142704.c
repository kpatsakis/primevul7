usm_parse_create_usmUser(const char *token, char *line)
{
    const char *error = NULL;
    usm_create_usmUser_from_string(line, &error);
    if (error)
        config_perror(error);
}