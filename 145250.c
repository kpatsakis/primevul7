ntpqmain(
	int argc,
	char *argv[]
	)
{
	u_int ihost;
	int icmd;


#ifdef SYS_VXWORKS
	clear_globals();
	taskPrioritySet(taskIdSelf(), 100 );
#endif

	delay_time.l_ui = 0;
	delay_time.l_uf = DEFDELAY;

	init_lib();	/* sets up ipv4_works, ipv6_works */
	ssl_applink();
	init_auth();

	/* Check to see if we have IPv6. Otherwise default to IPv4 */
	if (!ipv6_works)
		ai_fam_default = AF_INET;

	/* Fixup keytype's help based on available digest names */

	{
	    char *list;
	    char *msg, *fmt;

	    list = list_digest_names();
	    for (icmd = 0; icmd < sizeof(builtins)/sizeof(builtins[0]); icmd++) {
		if (strcmp("keytype", builtins[icmd].keyword) == 0)
		    break;
	    }

	    /* CID: 1295478 */
	    /* This should only "trip" if "keytype" is removed from builtins */
	    INSIST(icmd < sizeof(builtins)/sizeof(builtins[0]));

#ifdef OPENSSL
	    builtins[icmd].desc[0] = "digest-name";
	    fmt = "set key type to use for authenticated requests, one of:%s";
#else
	    builtins[icmd].desc[0] = "md5";
	    fmt = "set key type to use for authenticated requests (%s)";
#endif
	    msg = emalloc(strlen(fmt) + strlen(list) - strlen("%s") +1);
	    sprintf(msg, fmt, list);
	    builtins[icmd].comment = msg;
	    free(list);
	}

	progname = argv[0];

	{
		int optct = ntpOptionProcess(&ntpqOptions, argc, argv);
		argc -= optct;
		argv += optct;
	}

	/*
	 * Process options other than -c and -p, which are specially
	 * handled by ntpq_custom_opt_handler().
	 */

	debug = OPT_VALUE_SET_DEBUG_LEVEL;

	if (HAVE_OPT(IPV4))
		ai_fam_templ = AF_INET;
	else if (HAVE_OPT(IPV6))
		ai_fam_templ = AF_INET6;
	else
		ai_fam_templ = ai_fam_default;

	if (HAVE_OPT(INTERACTIVE))
		interactive = 1;

	if (HAVE_OPT(NUMERIC))
		showhostnames = 0;

	if (HAVE_OPT(WIDE))
		wideremote = 1;

	old_rv = HAVE_OPT(OLD_RV);

	if (0 == argc) {
		ADDHOST(DEFHOST);
	} else {
		for (ihost = 0; ihost < (u_int)argc; ihost++) {
			if ('-' == *argv[ihost]) {
				//
				// If I really cared I'd also check:
				// 0 == argv[ihost][2]
				//
				// and there are other cases as well...
				//
				if ('4' == argv[ihost][1]) {
					ai_fam_templ = AF_INET;
					continue;
				} else if ('6' == argv[ihost][1]) {
					ai_fam_templ = AF_INET6;
					continue;
				} else {
					// XXX Throw a usage error
				}
			}
			ADDHOST(argv[ihost]);
		}
	}

	if (numcmds == 0 && interactive == 0
	    && isatty(fileno(stdin)) && isatty(fileno(stderr))) {
		interactive = 1;
	}

#ifndef SYS_WINNT /* Under NT cannot handle SIGINT, WIN32 spawns a handler */
	if (interactive)
	    (void) signal_no_reset(SIGINT, abortcmd);
#endif /* SYS_WINNT */

	if (numcmds == 0) {
		(void) openhost(chosts[0].name, chosts[0].fam);
		getcmds();
	} else {
		for (ihost = 0; ihost < numhosts; ihost++) {
			if (openhost(chosts[ihost].name, chosts[ihost].fam))
				for (icmd = 0; icmd < numcmds; icmd++)
					docmd(ccmds[icmd]);
		}
	}
#ifdef SYS_WINNT
	WSACleanup();
#endif /* SYS_WINNT */
	return 0;
}