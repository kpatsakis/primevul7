static void fetch_result_double(MYSQL_BIND *param,
                                MYSQL_FIELD *field __attribute__((unused)),
                                uchar **row)
{
  double value;
  float8get(value,*row);
  doublestore(param->buffer, value);
  *row+= 8;
}