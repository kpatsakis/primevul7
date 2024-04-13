ex_options(
    exarg_T	*eap UNUSED)
{
    char_u  buf[500];
    int	    multi_mods = 0;

    buf[0] = NUL;
    (void)add_win_cmd_modifers(buf, &cmdmod, &multi_mods);

    vim_setenv((char_u *)"OPTWIN_CMD", buf);
    cmd_source((char_u *)SYS_OPTWIN_FILE, NULL);
}