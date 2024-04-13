static void fetch_result_date(MYSQL_BIND *param,
                              MYSQL_FIELD *field __attribute__((unused)),
                              uchar **row)
{
  MYSQL_TIME *tm= (MYSQL_TIME *)param->buffer;
  read_binary_date(tm, row);
}