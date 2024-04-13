int STDCALL mysql_server_init(int argc __attribute__((unused)),
			      char **argv __attribute__((unused)),
			      char **groups __attribute__((unused)))
{
  int result= 0;
  if (!mysql_client_init)
  {
    mysql_client_init=1;
    org_my_init_done=my_init_done;
    if (my_init())				/* Will init threads */
      return 1;
    init_client_errs();
    if (mysql_client_plugin_init())
      return 1;
    if (!mysql_port)
    {
      char *env;
      struct servent *serv_ptr __attribute__((unused));

      mysql_port = MYSQL_PORT;

      /*
        if builder specifically requested a default port, use that
        (even if it coincides with our factory default).
        only if they didn't do we check /etc/services (and, failing
        on that, fall back to the factory default of 3306).
        either default can be overridden by the environment variable
        MYSQL_TCP_PORT, which in turn can be overridden with command
        line options.
      */

#if MYSQL_PORT_DEFAULT == 0
      if ((serv_ptr= getservbyname("mysql", "tcp")))
        mysql_port= (uint) ntohs((ushort) serv_ptr->s_port);
#endif
      if ((env= getenv("MYSQL_TCP_PORT")))
        mysql_port=(uint) atoi(env);
    }

    if (!mysql_unix_port)
    {
      char *env;
#ifdef __WIN__
      mysql_unix_port = (char*) MYSQL_NAMEDPIPE;
#else
      mysql_unix_port = (char*) MYSQL_UNIX_ADDR;
#endif
      if ((env = getenv("MYSQL_UNIX_PORT")))
	mysql_unix_port = env;
    }
    mysql_debug(NullS);
#if defined(SIGPIPE) && !defined(__WIN__)
    (void) signal(SIGPIPE, SIG_IGN);
#endif
#ifdef EMBEDDED_LIBRARY
    if (argc > -1)
       result= init_embedded_server(argc, argv, groups);
#endif
  }
  else
    result= (int)my_thread_init();         /* Init if new thread */
  return result;
}