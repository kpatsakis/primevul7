get_one_option(int optid, const struct my_option *opt __attribute__((unused)),
	       char *argument)
{
  switch(optid) {
  case OPT_CHARSETS_DIR:
    strmake(mysql_charsets_dir, argument, sizeof(mysql_charsets_dir) - 1);
    charsets_dir = mysql_charsets_dir;
    break;
  case OPT_DELIMITER:
    if (argument == disabled_my_option) 
    {
      strmov(delimiter, DEFAULT_DELIMITER);
    }
    else 
    {
      /* Check that delimiter does not contain a backslash */
      if (!strstr(argument, "\\")) 
      {
        strmake(delimiter, argument, sizeof(delimiter) - 1);
      }
      else 
      {
        put_info("DELIMITER cannot contain a backslash character", INFO_ERROR);
        return 0;
      } 
    }
    delimiter_length= (uint)strlen(delimiter);
    delimiter_str= delimiter;
    break;
  case OPT_LOCAL_INFILE:
    using_opt_local_infile=1;
    break;
  case OPT_ENABLE_CLEARTEXT_PLUGIN:
    using_opt_enable_cleartext_plugin= TRUE;
    break;
  case OPT_TEE:
    if (argument == disabled_my_option)
    {
      if (opt_outfile)
	end_tee();
    }
    else
      init_tee(argument);
    break;
  case OPT_PAGER:
    if (argument == disabled_my_option)
      opt_nopager= 1;
    else
    {
      opt_nopager= 0;
      if (argument && strlen(argument))
      {
	default_pager_set= 1;
	strmake(pager, argument, sizeof(pager) - 1);
	strmov(default_pager, pager);
      }
      else if (default_pager_set)
	strmov(pager, default_pager);
      else
	opt_nopager= 1;
    }
    break;
  case OPT_MYSQL_PROTOCOL:
#ifndef EMBEDDED_LIBRARY
    opt_protocol= find_type_or_exit(argument, &sql_protocol_typelib,
                                    opt->name);
#endif
    break;
  case OPT_SERVER_ARG:
#ifdef EMBEDDED_LIBRARY
    /*
      When the embedded server is being tested, the client needs to be
      able to pass command-line arguments to the embedded server so it can
      locate the language files and data directory.
    */
    if (!embedded_server_arg_count)
    {
      embedded_server_arg_count= 1;
      embedded_server_args[0]= (char*) "";
    }
    if (embedded_server_arg_count == MAX_SERVER_ARGS-1 ||
        !(embedded_server_args[embedded_server_arg_count++]=
          my_strdup(argument, MYF(MY_FAE))))
    {
        put_info("Can't use server argument", INFO_ERROR);
        return 0;
    }
#else /*EMBEDDED_LIBRARY */
    printf("WARNING: --server-arg option not supported in this configuration.\n");
#endif
    break;
  case 'A':
    opt_rehash= 0;
    break;
  case 'N':
    column_names= 0;
    break;
  case 'e':
    status.batch= 1;
    status.add_to_history= 0;
    if (!status.line_buff)
      ignore_errors= 0;                         // do it for the first -e only
    if (!(status.line_buff= batch_readline_command(status.line_buff, argument)))
      return 1;
    break;
  case 'o':
    if (argument == disabled_my_option)
      one_database= 0;
    else
      one_database= skip_updates= 1;
    break;
  case 'p':
    if (argument == disabled_my_option)
      argument= (char*) "";			// Don't require password
    if (argument)
    {
      char *start= argument;
      my_free(opt_password);
      opt_password= my_strdup(argument, MYF(MY_FAE));
      while (*argument) *argument++= 'x';		// Destroy argument
      if (*start)
	start[1]=0 ;
      tty_password= 0;
    }
    else
      tty_password= 1;
    break;
  case '#':
    DBUG_PUSH(argument ? argument : default_dbug_option);
    debug_info_flag= 1;
    break;
  case 's':
    if (argument == disabled_my_option)
      opt_silent= 0;
    else
      opt_silent++;
    break;
  case 'v':
    if (argument == disabled_my_option)
      verbose= 0;
    else
      verbose++;
    break;
  case 'B':
    status.batch= 1;
    status.add_to_history= 0;
    set_if_bigger(opt_silent,1);                         // more silent
    break;
  case 'W':
#ifdef __WIN__
    opt_protocol = MYSQL_PROTOCOL_PIPE;
#endif
    break;
#include <sslopt-case.h>
  case 'V':
    usage(1);
    exit(0);
  case 'I':
  case '?':
    usage(0);
    exit(0);
  }
  return 0;
}