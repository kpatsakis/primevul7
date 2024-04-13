irc_mode_get_arguments (const char *arguments)
{
    char **argv, **argv2, *new_arguments;
    int argc, i;

    if (!arguments || !arguments[0])
        return strdup ("");

    argv = weechat_string_split (arguments, " ", NULL,
                                 WEECHAT_STRING_SPLIT_STRIP_LEFT
                                 | WEECHAT_STRING_SPLIT_STRIP_RIGHT
                                 | WEECHAT_STRING_SPLIT_COLLAPSE_SEPS,
                                 0, &argc);
    if (!argv)
        return strdup ("");

    argv2 = malloc (sizeof (*argv) * (argc + 1));
    if (!argv2)
    {
        weechat_string_free_split (argv);
        return strdup ("");;
    }

    for (i = 0; i < argc; i++)
    {
        argv2[i] = (argv[i][0] == ':') ? argv[i] + 1 : argv[i];
    }
    argv2[argc] = NULL;

    new_arguments = weechat_string_build_with_split_string (
        (const char **)argv2, " ");

    weechat_string_free_split (argv);
    free (argv2);

    return new_arguments;
}