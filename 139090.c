static void store_param_date(NET *net, MYSQL_BIND *param)
{
  MYSQL_TIME tm= *((MYSQL_TIME *) param->buffer);
  tm.hour= tm.minute= tm.second= tm.second_part= 0;
  net_store_datetime(net, &tm);
}