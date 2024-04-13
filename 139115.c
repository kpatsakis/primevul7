static void fetch_result_float(MYSQL_BIND *param,
                               MYSQL_FIELD *field __attribute__((unused)),
                               uchar **row)
{
  float value;
  float4get(value,*row);
  floatstore(param->buffer, value);
  *row+= 4;
}