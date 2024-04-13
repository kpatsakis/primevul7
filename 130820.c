print_table_data_xml(MYSQL_RES *result)
{
  MYSQL_ROW   cur;
  MYSQL_FIELD *fields;

  mysql_field_seek(result,0);

  tee_fputs("<?xml version=\"1.0\"?>\n\n<resultset statement=\"", PAGER);
  xmlencode_print(glob_buffer.ptr(), (int)strlen(glob_buffer.ptr()));
  tee_fputs("\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">",
            PAGER);

  fields = mysql_fetch_fields(result);
  while ((cur = mysql_fetch_row(result)))
  {
    if (interrupted_query)
      break;
    ulong *lengths=mysql_fetch_lengths(result);
    (void) tee_fputs("\n  <row>\n", PAGER);
    for (uint i=0; i < mysql_num_fields(result); i++)
    {
      tee_fprintf(PAGER, "\t<field name=\"");
      xmlencode_print(fields[i].name, (uint) strlen(fields[i].name));
      if (cur[i])
      {
        tee_fprintf(PAGER, "\">");
        if (opt_binhex && is_binary_field(&fields[i]))
          print_as_hex(PAGER, cur[i], lengths[i], lengths[i]);
        else
          xmlencode_print(cur[i], lengths[i]);
        tee_fprintf(PAGER, "</field>\n");
      }
      else
        tee_fprintf(PAGER, "\" xsi:nil=\"true\" />\n");
    }
    (void) tee_fputs("  </row>\n", PAGER);
  }
  (void) tee_fputs("</resultset>\n", PAGER);
}