irc_mode_get_chanmode_type (struct t_irc_server *server, char chanmode)
{
    char chanmode_type, *pos;
    const char *chanmodes, *ptr_chanmodes;

    /*
     * assume it is type 'B' if mode is in prefix
     * (we first check that because some exotic servers like irc.webchat.org
     * include the prefix chars in chanmodes as type 'A', which is wrong)
     */
    if (irc_server_get_prefix_mode_index (server, chanmode) >= 0)
        return 'B';

    chanmodes = irc_server_get_chanmodes (server);
    pos = strchr (chanmodes, chanmode);
    if (pos)
    {
        chanmode_type = 'A';
        for (ptr_chanmodes = chanmodes; ptr_chanmodes < pos; ptr_chanmodes++)
        {
            if (ptr_chanmodes[0] == ',')
            {
                if (chanmode_type == 'D')
                    break;
                chanmode_type++;
            }
        }
        return chanmode_type;
    }

    /* unknown mode, type 'D' by default */
    return 'D';
}