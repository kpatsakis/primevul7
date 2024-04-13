com_help(String *buffer __attribute__((unused)),
	 char *line __attribute__((unused)))
{
  reg1 int i, j;
  char * help_arg= strchr(line,' '), buff[32], *end;
  if (help_arg)
  {
    while (my_isspace(charset_info,*help_arg))
      help_arg++;
	if (*help_arg)	  
	  return com_server_help(buffer,line,help_arg);
  }

  put_info("\nFor information about MySQL products and services, visit:\n"
           "   http://www.mysql.com/\n"
           "For developer information, including the MySQL Reference Manual, "
           "visit:\n"
           "   http://dev.mysql.com/\n"
           "To buy MySQL Enterprise support, training, or other products, visit:\n"
           "   https://shop.mysql.com/\n", INFO_INFO);
  put_info("List of all MySQL commands:", INFO_INFO);
  if (!named_cmds)
    put_info("Note that all text commands must be first on line and end with ';'",INFO_INFO);
  for (i = 0; commands[i].name; i++)
  {
    end= strmov(buff, commands[i].name);
    for (j= (int)strlen(commands[i].name); j < 10; j++)
      end= strmov(end, " ");
    if (commands[i].func)
      tee_fprintf(stdout, "%s(\\%c) %s\n", buff,
		  commands[i].cmd_char, commands[i].doc);
  }
  if (connected && mysql_get_server_version(&mysql) >= 40100)
    put_info("\nFor server side help, type 'help contents'\n", INFO_INFO);
  return 0;
}