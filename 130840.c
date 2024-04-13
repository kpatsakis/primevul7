static int get_field_disp_length(MYSQL_FIELD *field)
{
  uint length= column_names ? field->name_length : 0;

  if (quick)
    length= max(length, field->length);
  else
    length= max(length, field->max_length);

  if (length < 4 && !IS_NOT_NULL(field->flags))
    length= 4;				/* Room for "NULL" */

  return length;
}