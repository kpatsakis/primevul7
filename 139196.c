mysql_ping(MYSQL *mysql)
{
  int res;
  DBUG_ENTER("mysql_ping");
  res= simple_command(mysql,COM_PING,0,0,0);
  if (res == CR_SERVER_LOST && mysql->reconnect)
    res= simple_command(mysql,COM_PING,0,0,0);
  DBUG_RETURN(res);
}