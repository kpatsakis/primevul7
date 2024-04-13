mysql_get_server_info(MYSQL *mysql)
{
  return((char*) mysql->server_version);
}