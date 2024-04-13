do_shell(
    char_u	*cmd,
    int		flags)	// may be SHELL_DOOUT when output is redirected
{
    buf_T	*buf;
#if !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    int		save_nwr;
#endif
#ifdef MSWIN
    int		winstart = FALSE;
#endif
    int		keep_termcap = !termcap_active;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    if (check_restricted() || check_secure())
    {
	msg_end();
	return;
    }

#ifdef MSWIN
    /*
     * Check if ":!start" is used.  This implies not stopping termcap mode.
     */
    if (cmd != NULL)
	keep_termcap = winstart = (STRNICMP(cmd, "start ", 6) == 0);

# if defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // Don't stop termcap mode when using a terminal window for the shell.
    if (gui.in_use && vim_strchr(p_go, GO_TERMINAL) != NULL)
	keep_termcap = TRUE;
# endif
#endif

    /*
     * For autocommands we want to get the output on the current screen, to
     * avoid having to type return below.
     */
    msg_putchar('\r');			// put cursor at start of line
    if (!autocmd_busy)
    {
	if (!keep_termcap)
	    stoptermcap();
    }
#ifdef MSWIN
    if (!winstart)
#endif
	msg_putchar('\n');		// may shift screen one line up

    // warning message before calling the shell
    if (p_warn && !autocmd_busy && msg_silent == 0)
	FOR_ALL_BUFFERS(buf)
	    if (bufIsChangedNotTerm(buf))
	    {
#ifdef FEAT_GUI_MSWIN
		if (!keep_termcap)
		    starttermcap();	// don't want a message box here
#endif
		msg_puts(_("[No write since last change]\n"));
#ifdef FEAT_GUI_MSWIN
		if (!keep_termcap)
		    stoptermcap();
#endif
		break;
	    }

    // This windgoto is required for when the '\n' resulted in a "delete line
    // 1" command to the terminal.
    if (!swapping_screen())
	windgoto(msg_row, msg_col);
    cursor_on();
    (void)call_shell(cmd, SHELL_COOKED | flags);
    did_check_timestamps = FALSE;
    need_check_timestamps = TRUE;

    /*
     * put the message cursor at the end of the screen, avoids wait_return()
     * to overwrite the text that the external command showed
     */
    if (!swapping_screen())
    {
	msg_row = Rows - 1;
	msg_col = 0;
    }

    if (autocmd_busy)
    {
	if (msg_silent == 0)
	    redraw_later_clear();
    }
    else
    {
	/*
	 * For ":sh" there is no need to call wait_return(), just redraw.
	 * Also for the Win32 GUI (the output is in a console window).
	 * Otherwise there is probably text on the screen that the user wants
	 * to read before redrawing, so call wait_return().
	 */
#if !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
	if (!gui.in_use)
# endif
	{
	    if (cmd == NULL
# ifdef MSWIN
		    || (keep_termcap && !need_wait_return)
# endif
	       )
	    {
		if (msg_silent == 0)
		    redraw_later_clear();
		need_wait_return = FALSE;
	    }
	    else
	    {
		/*
		 * If we switch screens when starttermcap() is called, we
		 * really want to wait for "hit return to continue".
		 */
		save_nwr = no_wait_return;
		if (swapping_screen())
		    no_wait_return = FALSE;
# ifdef AMIGA
		wait_return(term_console ? -1 : msg_silent == 0); // see below
# else
		wait_return(msg_silent == 0);
# endif
		no_wait_return = save_nwr;
	    }
	}
#endif // FEAT_GUI_MSWIN

	if (!keep_termcap)	// if keep_termcap is TRUE didn't stop termcap
	    starttermcap();	// start termcap if not done by wait_return()

	/*
	 * In an Amiga window redrawing is caused by asking the window size.
	 * If we got an interrupt this will not work. The chance that the
	 * window size is wrong is very small, but we need to redraw the
	 * screen.  Don't do this if ':' hit in wait_return().	THIS IS UGLY
	 * but it saves an extra redraw.
	 */
#ifdef AMIGA
	if (skip_redraw)		// ':' hit in wait_return()
	{
	    if (msg_silent == 0)
		redraw_later_clear();
	}
	else if (term_console)
	{
	    OUT_STR("\033[0 q");	// get window size
	    if (got_int && msg_silent == 0)
		redraw_later_clear();	// if got_int is TRUE, redraw needed
	    else
		must_redraw = 0;	// no extra redraw needed
	}
#endif
    }

    // display any error messages now
    display_errors();

    apply_autocmds(EVENT_SHELLCMDPOST, NULL, NULL, FALSE, curbuf);
}