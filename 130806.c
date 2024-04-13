static int com_server_help(String *buffer __attribute__((unused)),
			   char *line __attribute__((unused)), char *help_arg)
{
  MYSQL_ROW cur;
  const char *server_cmd;
  char cmd_buf[100 + 1];
  MYSQL_RES *result;
  int error;
  
  if (help_arg[0] != '\'')
  {
	char *end_arg= strend(help_arg);
	if(--end_arg)
	{
		while (my_isspace(charset_info,*end_arg))
          end_arg--;
		*++end_arg= '\0';
	}
	(void) strxnmov(cmd_buf, sizeof(cmd_buf), "help '", help_arg, "'", NullS);
  }
  else
    (void) strxnmov(cmd_buf, sizeof(cmd_buf), "help ", help_arg, NullS);

  server_cmd= cmd_buf;

  if (!status.batch)
  {
    old_buffer= *buffer;
    old_buffer.copy();
  }

  if (!connected && reconnect())
    return 1;

  if ((error= mysql_real_query_for_lazy(server_cmd,(int)strlen(server_cmd))) ||
      (error= mysql_store_result_for_lazy(&result)))
    return error;

  if (result)
  {
    unsigned int num_fields= mysql_num_fields(result);
    my_ulonglong num_rows= mysql_num_rows(result);
    mysql_fetch_fields(result);
    if (num_fields==3 && num_rows==1)
    {
      if (!(cur= mysql_fetch_row(result)))
      {
	error= -1;
	goto err;
      }

      init_pager();
      tee_fprintf(PAGER,   "Name: \'%s\'\n", cur[0]);
      tee_fprintf(PAGER,   "Description:\n%s", cur[1]);
      if (cur[2] && *((char*)cur[2]))
	tee_fprintf(PAGER, "Examples:\n%s", cur[2]);
      tee_fprintf(PAGER,   "\n");
      end_pager();
    }
    else if (num_fields >= 2 && num_rows)
    {
      init_pager();
      char last_char= 0;

      int num_name= 0, num_cat= 0;
      LINT_INIT(num_name);
      LINT_INIT(num_cat);

      if (num_fields == 2)
      {
	put_info("Many help items for your request exist.", INFO_INFO);
	put_info("To make a more specific request, please type 'help <item>',\nwhere <item> is one of the following", INFO_INFO);
	num_name= 0;
	num_cat= 1;
      }
      else if ((cur= mysql_fetch_row(result)))
      {
	tee_fprintf(PAGER, "You asked for help about help category: \"%s\"\n", cur[0]);
	put_info("For more information, type 'help <item>', where <item> is one of the following", INFO_INFO);
	num_name= 1;
	num_cat= 2;
	print_help_item(&cur,1,2,&last_char);
      }

      while ((cur= mysql_fetch_row(result)))
	print_help_item(&cur,num_name,num_cat,&last_char);
      tee_fprintf(PAGER, "\n");
      end_pager();
    }
    else
    {
      put_info("\nNothing found", INFO_INFO);
      if (strncasecmp(server_cmd, "help 'contents'", 15) == 0)
      {
         put_info("\nPlease check if 'help tables' are loaded.\n", INFO_INFO); 
         goto err;
      }
      put_info("Please try to run 'help contents' for a list of all accessible topics\n", INFO_INFO);
    }
  }

err:
  mysql_free_result(result);
  return error;
}