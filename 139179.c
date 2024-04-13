static void fetch_result_int64(MYSQL_BIND *param,
                               MYSQL_FIELD *field __attribute__((unused)),
                               uchar **row)
{
  my_bool field_is_unsigned= test(field->flags & UNSIGNED_FLAG);
  ulonglong data= (ulonglong) sint8korr(*row);
  *param->error= param->is_unsigned != field_is_unsigned && data > LONGLONG_MAX;
  longlongstore(param->buffer, data);
  *row+= 8;
}