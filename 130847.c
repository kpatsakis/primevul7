int main(int argc,char *argv[])
{
  char buff[80];

  MY_INIT(argv[0]);
  DBUG_ENTER("main");
  DBUG_PROCESS(argv[0]);
  
  delimiter_str= delimiter;
  default_prompt = my_strdup(getenv("MYSQL_PS1") ? 
			     getenv("MYSQL_PS1") : 
			     "mysql> ",MYF(MY_WME));
  current_prompt = my_strdup(default_prompt,MYF(MY_WME));
  prompt_counter=0;

  outfile[0]=0;			// no (default) outfile
  strmov(pager, "stdout");	// the default, if --pager wasn't given
  {
    char *tmp=getenv("PAGER");
    if (tmp && strlen(tmp))
    {
      default_pager_set= 1;
      strmov(default_pager, tmp);
    }
  }
  if (!isatty(0) || !isatty(1))
  {
    status.batch=1; opt_silent=1;
    ignore_errors=0;
  }
  else
    status.add_to_history=1;
  status.exit_status=1;

  {
    /* 
     The file descriptor-layer may be out-of-sync with the file-number layer,
     so we make sure that "stdout" is really open.  If its file is closed then
     explicitly close the FD layer. 
    */
    int stdout_fileno_copy;
    stdout_fileno_copy= dup(fileno(stdout)); /* Okay if fileno fails. */
    if (stdout_fileno_copy == -1)
      fclose(stdout);
    else
      close(stdout_fileno_copy);             /* Clean up dup(). */
  }

  if (load_defaults("my",load_default_groups,&argc,&argv))
  {
    my_end(0);
    exit(1);
  }
  defaults_argv=argv;
  if (get_options(argc, (char **) argv))
  {
    free_defaults(defaults_argv);
    my_end(0);
    exit(1);
  }
  if (status.batch && !status.line_buff &&
      !(status.line_buff= batch_readline_init(MAX_BATCH_BUFFER_SIZE, stdin)))
  {
    put_info("Can't initialize batch_readline - may be the input source is "
             "a directory or a block device.", INFO_ERROR, 0);
    free_defaults(defaults_argv);
    my_end(0);
    exit(1);
  }
  if (mysql_server_init(embedded_server_arg_count, embedded_server_args, 
                        (char**) embedded_server_groups))
  {
    put_error(NULL);
    free_defaults(defaults_argv);
    my_end(0);
    exit(1);
  }
  glob_buffer.realloc(512);
  completion_hash_init(&ht, 128);
  init_alloc_root(&hash_mem_root, 16384, 0);
  bzero((char*) &mysql, sizeof(mysql));
  if (sql_connect(current_host,current_db,current_user,opt_password,
		  opt_silent))
  {
    quick= 1;					// Avoid history
    status.exit_status= 1;
    mysql_end(-1);
  }
  if (!status.batch)
    ignore_errors=1;				// Don't abort monitor

  if (opt_sigint_ignore)
    signal(SIGINT, SIG_IGN);
  else
    signal(SIGINT, handle_sigint);              // Catch SIGINT to clean up
  signal(SIGQUIT, mysql_end);			// Catch SIGQUIT to clean up

#if defined(HAVE_TERMIOS_H) && defined(GWINSZ_IN_SYS_IOCTL)
  /* Readline will call this if it installs a handler */
  signal(SIGWINCH, window_resize);
  /* call the SIGWINCH handler to get the default term width */
  window_resize(0);
#endif

  put_info("Welcome to the MySQL monitor.  Commands end with ; or \\g.",
	   INFO_INFO);
  snprintf((char*) glob_buffer.ptr(), glob_buffer.alloced_length(),
	   "Your MySQL connection id is %lu\nServer version: %s\n",
	   mysql_thread_id(&mysql), server_version_string(&mysql));
  put_info((char*) glob_buffer.ptr(),INFO_INFO);

  put_info(ORACLE_WELCOME_COPYRIGHT_NOTICE("2000"), INFO_INFO);

#ifdef HAVE_READLINE
  initialize_readline((char*) my_progname);
  if (!status.batch && !quick && !opt_html && !opt_xml)
  {
    /* read-history from file, default ~/.mysql_history*/
    if (getenv("MYSQL_HISTFILE"))
      histfile=my_strdup(getenv("MYSQL_HISTFILE"),MYF(MY_WME));
    else if (getenv("HOME"))
    {
      histfile=(char*) my_malloc((uint) strlen(getenv("HOME"))
				 + (uint) strlen("/.mysql_history")+2,
				 MYF(MY_WME));
      if (histfile)
	sprintf(histfile,"%s/.mysql_history",getenv("HOME"));
      char link_name[FN_REFLEN];
      if (my_readlink(link_name, histfile, 0) == 0 &&
          strncmp(link_name, "/dev/null", 10) == 0)
      {
        /* The .mysql_history file is a symlink to /dev/null, don't use it */
        my_free(histfile);
        histfile= 0;
      }
    }

    /* We used to suggest setting MYSQL_HISTFILE=/dev/null. */
    if (histfile && strncmp(histfile, "/dev/null", 10) == 0)
      histfile= NULL;

    if (histfile && histfile[0])
    {
      if (verbose)
	tee_fprintf(stdout, "Reading history-file %s\n",histfile);
      read_history(histfile);
      if (!(histfile_tmp= (char*) my_malloc((uint) strlen(histfile) + 5,
					    MYF(MY_WME))))
      {
	fprintf(stderr, "Couldn't allocate memory for temp histfile!\n");
	exit(1);
      }
      sprintf(histfile_tmp, "%s.TMP", histfile);
    }
  }

#endif

  sprintf(buff, "%s",
	  "Type 'help;' or '\\h' for help. Type '\\c' to clear the current input statement.\n");
  put_info(buff,INFO_INFO);
  status.exit_status= read_and_execute(!status.batch);
  if (opt_outfile)
    end_tee();
  mysql_end(0);
#ifndef _lint
  DBUG_RETURN(0);				// Keep compiler happy
#endif
}