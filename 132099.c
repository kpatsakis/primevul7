irc_mode_user_add (struct t_irc_server *server, char mode)
{
    char str_mode[2], *nick_modes2;

    str_mode[0] = mode;
    str_mode[1] = '\0';

    if (server->nick_modes)
    {
        if (!strchr (server->nick_modes, mode))
        {
            nick_modes2 = realloc (server->nick_modes,
                                   strlen (server->nick_modes) + 1 + 1);
            if (!nick_modes2)
            {
                if (server->nick_modes)
                {
                    free (server->nick_modes);
                    server->nick_modes = NULL;
                }
                return;
            }
            server->nick_modes = nick_modes2;
            strcat (server->nick_modes, str_mode);
            weechat_bar_item_update ("input_prompt");
            weechat_bar_item_update ("irc_nick_modes");
        }
    }
    else
    {
        server->nick_modes = malloc (2);
        strcpy (server->nick_modes, str_mode);
        weechat_bar_item_update ("input_prompt");
        weechat_bar_item_update ("irc_nick_modes");
    }
}