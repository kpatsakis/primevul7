print_table_data_vertically(MYSQL_RES *result)
{
  MYSQL_ROW	cur;
  uint		max_length=0;
  MYSQL_FIELD	*field;

  while ((field = mysql_fetch_field(result)))
  {
    uint length= field->name_length;
    if (length > max_length)
      max_length= length;
    field->max_length=length;
  }

  mysql_field_seek(result,0);
  for (uint row_count=1; (cur= mysql_fetch_row(result)); row_count++)
  {
    if (interrupted_query)
      break;
    mysql_field_seek(result,0);
    tee_fprintf(PAGER, 
		"*************************** %d. row ***************************\n", row_count);

    ulong *lengths= mysql_fetch_lengths(result);

    for (uint off=0; off < mysql_num_fields(result); off++)
    {
      field= mysql_fetch_field(result);
      if (column_names)
        tee_fprintf(PAGER, "%*s: ",(int) max_length,field->name);
      if (cur[off])
      {
        unsigned int i;
        const char *p;
        if (opt_binhex && is_binary_field(field))
           fprintf(PAGER, "0x");
        for (i= 0, p= cur[off]; i < lengths[off]; i+= 1, p+= 1)
        {
          if (opt_binhex && is_binary_field(field))
            fprintf(PAGER, "%02X", *((uchar*)p));
          else
          {
            if (*p == '\0')
              tee_putc((int)' ', PAGER);
            else
              tee_putc((int)*p, PAGER);
          }
        }
        tee_putc('\n', PAGER);
      }
       else
        tee_fprintf(PAGER, "NULL\n");
    }
  }
}