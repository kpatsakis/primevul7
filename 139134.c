mysql_get_proto_info(MYSQL *mysql)
{
  return (mysql->protocol_version);
}