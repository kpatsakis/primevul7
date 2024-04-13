static void skip_result_with_length(MYSQL_BIND *param __attribute__((unused)),
				    MYSQL_FIELD *field __attribute__((unused)),
				    uchar **row)

{
  ulong length= net_field_length(row);
  (*row)+= length;
}