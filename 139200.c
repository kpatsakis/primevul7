const char *STDCALL mysql_sqlstate(MYSQL *mysql)
{
  return mysql ? mysql->net.sqlstate : cant_connect_sqlstate;
}