is_binary_field(MYSQL_FIELD *field)
{
  if ((field->charsetnr == 63) &&
      (field->type == MYSQL_TYPE_BIT ||
       field->type == MYSQL_TYPE_BLOB ||
       field->type == MYSQL_TYPE_LONG_BLOB ||
       field->type == MYSQL_TYPE_MEDIUM_BLOB ||
      field->type == MYSQL_TYPE_TINY_BLOB ||
       field->type == MYSQL_TYPE_VAR_STRING ||
       field->type == MYSQL_TYPE_STRING ||
       field->type == MYSQL_TYPE_VARCHAR ||
       field->type == MYSQL_TYPE_GEOMETRY))
    return 1;
  return 0;
}