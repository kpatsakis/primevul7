irc_mode_user_set (struct t_irc_server *server, const char *modes,
                   int reset_modes)
{
    char set_flag;
    int end;

    if (reset_modes)
    {
        if (server->nick_modes)
        {
            free (server->nick_modes);
            server->nick_modes = NULL;
        }
    }
    set_flag = '+';
    end = 0;
    while (modes && modes[0])
    {
        switch (modes[0])
        {
            case ' ':
                end = 1;
                break;
            case ':':
                break;
            case '+':
                set_flag = '+';
                break;
            case '-':
                set_flag = '-';
                break;
            default:
                if (set_flag == '+')
                    irc_mode_user_add (server, modes[0]);
                else
                    irc_mode_user_remove (server, modes[0]);
                break;
        }
        if (end)
            break;
        modes++;
    }
    weechat_bar_item_update ("input_prompt");
    weechat_bar_item_update ("irc_nick_modes");
}