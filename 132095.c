irc_mode_channel_set (struct t_irc_server *server,
                      struct t_irc_channel *channel,
                      const char *host,
                      const char *modes,
                      const char *modes_arguments)
{
    const char *pos, *ptr_arg;
    char set_flag, **argv, chanmode_type;
    int argc, current_arg, update_channel_modes, channel_modes_updated;
    int smart_filter, end_modes;
    struct t_irc_nick *ptr_nick;
    struct t_irc_modelist *ptr_modelist;
    struct t_irc_modelist_item *ptr_item;

    if (!server || !channel || !modes)
        return 0;

    channel_modes_updated = 0;
    argc = 0;
    argv = NULL;
    if (modes_arguments)
    {
        argv = weechat_string_split (modes_arguments, " ", NULL,
                                     WEECHAT_STRING_SPLIT_STRIP_LEFT
                                     | WEECHAT_STRING_SPLIT_STRIP_RIGHT
                                     | WEECHAT_STRING_SPLIT_COLLAPSE_SEPS,
                                     0, &argc);
    }

    current_arg = 0;

    smart_filter = (weechat_config_boolean (irc_config_look_smart_filter)
                    && weechat_config_string (irc_config_look_smart_filter_mode)
                    && weechat_config_string (irc_config_look_smart_filter_mode)[0]) ? 1 : 0;

    end_modes = 0;
    set_flag = '+';
    pos = modes;
    while (pos[0])
    {
        switch (pos[0])
        {
            case ':':
                break;
            case ' ':
                end_modes = 1;
                break;
            case '+':
                set_flag = '+';
                break;
            case '-':
                set_flag = '-';
                break;
            default:
                update_channel_modes = 1;
                chanmode_type = irc_mode_get_chanmode_type (server, pos[0]);
                ptr_arg = NULL;
                switch (chanmode_type)
                {
                    case 'A': /* always argument */
                        update_channel_modes = 0;
                        ptr_arg = (current_arg < argc) ?
                            argv[current_arg] : NULL;
                        break;
                    case 'B': /* always argument */
                        ptr_arg = (current_arg < argc) ?
                            argv[current_arg] : NULL;
                        break;
                    case 'C': /* argument if set */
                        ptr_arg = ((set_flag == '+') && (current_arg < argc)) ?
                            argv[current_arg] : NULL;
                        break;
                    case 'D': /* no argument */
                        break;
                }
                if (ptr_arg)
                {
                    if (ptr_arg[0] == ':')
                        ptr_arg++;
                    current_arg++;
                }

                if (smart_filter
                    && !irc_mode_smart_filtered (server, pos[0]))
                {
                    smart_filter = 0;
                }

                if (pos[0] == 'k')
                {
                    /* channel key */
                    if (set_flag == '-')
                    {
                        if (channel->key)
                        {
                            free (channel->key);
                            channel->key = NULL;
                        }
                    }
                    else if ((set_flag == '+')
                             && ptr_arg && (strcmp (ptr_arg, "*") != 0))
                    {
                        /* replace key for +k, but ignore "*" as new key */
                        if (channel->key)
                            free (channel->key);
                        channel->key = strdup (ptr_arg);
                    }
                }
                else if (pos[0] == 'l')
                {
                    /* channel limit */
                    if (set_flag == '-')
                        channel->limit = 0;
                    if ((set_flag == '+') && ptr_arg)
                    {
                        channel->limit = atoi (ptr_arg);
                    }
                }
                else if ((chanmode_type != 'A')
                         && (irc_server_get_prefix_mode_index (server,
                                                               pos[0]) >= 0))
                {
                    /* mode for nick */
                    update_channel_modes = 0;
                    if (ptr_arg)
                    {
                        ptr_nick = irc_nick_search (server, channel,
                                                    ptr_arg);
                        if (ptr_nick)
                        {
                            irc_nick_set_mode (server, channel, ptr_nick,
                                               (set_flag == '+'), pos[0]);
                            /*
                             * disable smart filtering if mode is sent
                             * to me, or based on the nick speaking time
                             */
                            if (smart_filter
                                && ((irc_server_strcasecmp (server,
                                                            ptr_nick->name,
                                                            server->nick) == 0)
                                    || irc_channel_nick_speaking_time_search (server,
                                                                              channel,
                                                                              ptr_nick->name,
                                                                              1)))
                            {
                                smart_filter = 0;
                            }
                        }
                    }
                }
                else if (chanmode_type == 'A')
                {
                    /* modelist modes */
                    if (ptr_arg)
                    {
                        ptr_modelist = irc_modelist_search (channel, pos[0]);
                        if (ptr_modelist)
                        {
                            if (set_flag == '+')
                            {
                                irc_modelist_item_new (ptr_modelist, ptr_arg,
                                                       host, time (NULL));
                            }
                            else if (set_flag == '-')
                            {
                                ptr_item = irc_modelist_item_search_mask (
                                    ptr_modelist, ptr_arg);
                                if (ptr_item)
                                    irc_modelist_item_free (ptr_modelist, ptr_item);
                            }
                        }
                    }
                }

                if (update_channel_modes)
                {
                    irc_mode_channel_update (server, channel, set_flag,
                                             pos[0], ptr_arg);
                    channel_modes_updated = 1;
                }
                break;
        }
        if (end_modes)
            break;
        pos++;
    }

    if (argv)
        weechat_string_free_split (argv);

    if (channel_modes_updated)
        weechat_bar_item_update ("buffer_modes");

    return smart_filter;
}