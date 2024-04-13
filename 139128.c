mysql_shutdown(MYSQL *mysql, enum mysql_enum_shutdown_level shutdown_level)
{
  uchar level[1];
  DBUG_ENTER("mysql_shutdown");
  level[0]= (uchar) shutdown_level;
  DBUG_RETURN(simple_command(mysql, COM_SHUTDOWN, level, 1, 0));
}