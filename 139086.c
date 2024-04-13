static void fetch_result_tinyint(MYSQL_BIND *param, MYSQL_FIELD *field,
                                 uchar **row)
{
  my_bool field_is_unsigned= test(field->flags & UNSIGNED_FLAG);
  uchar data= **row;
  *(uchar *)param->buffer= data;
  *param->error= param->is_unsigned != field_is_unsigned && data > INT_MAX8;
  (*row)++;
}