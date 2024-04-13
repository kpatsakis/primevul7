static void fetch_result_datetime(MYSQL_BIND *param,
                                  MYSQL_FIELD *field __attribute__((unused)),
                                  uchar **row)
{
  MYSQL_TIME *tm= (MYSQL_TIME *)param->buffer;
  read_binary_datetime(tm, row);
}