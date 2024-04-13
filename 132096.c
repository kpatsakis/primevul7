irc_mode_user_remove (struct t_irc_server *server, char mode)
{
    char *pos, *nick_modes2;
    int new_size;

    if (server->nick_modes)
    {
        pos = strchr (server->nick_modes, mode);
        if (pos)
        {
            new_size = strlen (server->nick_modes);
            memmove (pos, pos + 1, strlen (pos + 1) + 1);
            nick_modes2 = realloc (server->nick_modes, new_size);
            if (nick_modes2)
                server->nick_modes = nick_modes2;
            weechat_bar_item_update ("input_prompt");
            weechat_bar_item_update ("irc_nick_modes");
        }
    }
}