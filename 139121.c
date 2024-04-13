mysql_set_server_option(MYSQL *mysql, enum enum_mysql_set_option option)
{
  uchar buff[2];
  DBUG_ENTER("mysql_set_server_option");
  int2store(buff, (uint) option);
  DBUG_RETURN(simple_command(mysql, COM_SET_OPTION, buff, sizeof(buff), 0));
}