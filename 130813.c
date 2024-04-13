com_status(String *buffer __attribute__((unused)),
	   char *line __attribute__((unused)))
{
  const char *status_str;
  char buff[40];
  ulonglong id;
  MYSQL_RES *result;
  LINT_INIT(result);

  if (mysql_real_query_for_lazy(
        C_STRING_WITH_LEN("select DATABASE(), USER() limit 1")))
    return 0;

  tee_puts("--------------", stdout);
  usage(1);					/* Print version */
  tee_fprintf(stdout, "\nConnection id:\t\t%lu\n",mysql_thread_id(&mysql));
  /*
    Don't remove "limit 1",
    it is protection againts SQL_SELECT_LIMIT=0
  */
  if (!mysql_store_result_for_lazy(&result))
  {
    MYSQL_ROW cur=mysql_fetch_row(result);
    if (cur)
    {
      tee_fprintf(stdout, "Current database:\t%s\n", cur[0] ? cur[0] : "");
      tee_fprintf(stdout, "Current user:\t\t%s\n", cur[1]);
    }
    mysql_free_result(result);
  }

#if defined(HAVE_OPENSSL) && !defined(EMBEDDED_LIBRARY)
  if ((status_str= mysql_get_ssl_cipher(&mysql)))
    tee_fprintf(stdout, "SSL:\t\t\tCipher in use is %s\n",
                status_str);
  else
#endif /* HAVE_OPENSSL && !EMBEDDED_LIBRARY */
    tee_puts("SSL:\t\t\tNot in use", stdout);

  if (skip_updates)
  {
    vidattr(A_BOLD);
    tee_fprintf(stdout, "\nAll updates ignored to this database\n");
    vidattr(A_NORMAL);
  }
#ifdef USE_POPEN
  tee_fprintf(stdout, "Current pager:\t\t%s\n", pager);
  tee_fprintf(stdout, "Using outfile:\t\t'%s'\n", opt_outfile ? outfile : "");
#endif
  tee_fprintf(stdout, "Using delimiter:\t%s\n", delimiter);
  tee_fprintf(stdout, "Server version:\t\t%s\n", server_version_string(&mysql));
  tee_fprintf(stdout, "Protocol version:\t%d\n", mysql_get_proto_info(&mysql));
  tee_fprintf(stdout, "Connection:\t\t%s\n", mysql_get_host_info(&mysql));
  if ((id= mysql_insert_id(&mysql)))
    tee_fprintf(stdout, "Insert id:\t\t%s\n", llstr(id, buff));

  /* "limit 1" is protection against SQL_SELECT_LIMIT=0 */
  if (mysql_real_query_for_lazy(C_STRING_WITH_LEN(
        "select @@character_set_client, @@character_set_connection, "
        "@@character_set_server, @@character_set_database limit 1")))
  {
    if (mysql_errno(&mysql) == CR_SERVER_GONE_ERROR)
      return 0;
  }
  if (!mysql_store_result_for_lazy(&result))
  {
    MYSQL_ROW cur=mysql_fetch_row(result);
    if (cur)
    {
      tee_fprintf(stdout, "Server characterset:\t%s\n", cur[2] ? cur[2] : "");
      tee_fprintf(stdout, "Db     characterset:\t%s\n", cur[3] ? cur[3] : "");
      tee_fprintf(stdout, "Client characterset:\t%s\n", cur[0] ? cur[0] : "");
      tee_fprintf(stdout, "Conn.  characterset:\t%s\n", cur[1] ? cur[1] : "");
    }
    mysql_free_result(result);
  }
  else
  {
    /* Probably pre-4.1 server */
    tee_fprintf(stdout, "Client characterset:\t%s\n", charset_info->csname);
    tee_fprintf(stdout, "Server characterset:\t%s\n", mysql.charset->csname);
  }

#ifndef EMBEDDED_LIBRARY
  if (strstr(mysql_get_host_info(&mysql),"TCP/IP") || ! mysql.unix_socket)
    tee_fprintf(stdout, "TCP port:\t\t%d\n", mysql.port);
  else
    tee_fprintf(stdout, "UNIX socket:\t\t%s\n", mysql.unix_socket);
  if (mysql.net.compress)
    tee_fprintf(stdout, "Protocol:\t\tCompressed\n");
#endif

  if ((status_str= mysql_stat(&mysql)) && !mysql_error(&mysql)[0])
  {
    ulong sec;
    const char *pos= strchr(status_str,' ');
    /* print label */
    tee_fprintf(stdout, "%.*s\t\t\t", (int) (pos-status_str), status_str);
    if ((status_str= str2int(pos,10,0,LONG_MAX,(long*) &sec)))
    {
      nice_time((double) sec,buff,0);
      tee_puts(buff, stdout);			/* print nice time */
      while (*status_str == ' ')
        status_str++;  /* to next info */
      tee_putc('\n', stdout);
      tee_puts(status_str, stdout);
    }
  }
  if (safe_updates)
  {
    vidattr(A_BOLD);
    tee_fprintf(stdout, "\nNote that you are running in safe_update_mode:\n");
    vidattr(A_NORMAL);
    tee_fprintf(stdout, "\
UPDATEs and DELETEs that don't use a key in the WHERE clause are not allowed.\n\
(One can force an UPDATE/DELETE by adding LIMIT # at the end of the command.)\n\
SELECT has an automatic 'LIMIT %lu' if LIMIT is not used.\n\
Max number of examined row combination in a join is set to: %lu\n\n",
select_limit, max_join_size);
  }
  tee_puts("--------------\n", stdout);
  return 0;
}