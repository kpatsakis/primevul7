static void store_param_tinyint(NET *net, MYSQL_BIND *param)
{
  *(net->write_pos++)= *(uchar *) param->buffer;
}