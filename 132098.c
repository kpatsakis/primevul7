irc_mode_smart_filtered (struct t_irc_server *server, char mode)
{
    const char *ptr_modes;

    ptr_modes = weechat_config_string (irc_config_look_smart_filter_mode);

    /* if empty value, there's no smart filtering on mode messages */
    if (!ptr_modes || !ptr_modes[0])
        return 0;

    /* if var is "*", ALL modes are smart filtered */
    if (strcmp (ptr_modes, "*") == 0)
        return 1;

    /* if var is "+", modes from server prefixes are filtered */
    if (strcmp (ptr_modes, "+") == 0)
        return strchr (irc_server_get_prefix_modes (server), mode) ? 1 : 0;

    /*
     * if var starts with "-", smart filter all modes except following modes
     * example: "-kl": smart filter all modes but not k/l
     */
    if (ptr_modes[0] == '-')
        return (strchr (ptr_modes + 1, mode)) ? 0 : 1;

    /*
     * explicit list of modes to smart filter
     * example: "ovh": smart filter modes o/v/h
     */
    return (strchr (ptr_modes, mode)) ? 1 : 0;
}