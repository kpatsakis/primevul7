static void skip_result_string(MYSQL_BIND *param __attribute__((unused)),
			       MYSQL_FIELD *field,
			       uchar **row)

{
  ulong length= net_field_length(row);
  (*row)+= length;
  if (field->max_length < length)
    field->max_length= length;
}