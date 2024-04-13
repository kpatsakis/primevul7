mysql_stat(MYSQL *mysql)
{
  DBUG_ENTER("mysql_stat");
  if (simple_command(mysql,COM_STATISTICS,0,0,0))
    DBUG_RETURN(mysql->net.last_error);
  DBUG_RETURN((*mysql->methods->read_statistics)(mysql));
}