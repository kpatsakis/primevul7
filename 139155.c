const char *cli_read_statistics(MYSQL *mysql)
{
  mysql->net.read_pos[mysql->packet_length]=0;	/* End of stat string */
  if (!mysql->net.read_pos[0])
  {
    set_mysql_error(mysql, CR_WRONG_HOST_INFO, unknown_sqlstate);
    return mysql->net.last_error;
  }
  return (char*) mysql->net.read_pos;
}