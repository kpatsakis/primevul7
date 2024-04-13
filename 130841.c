print_table_data(MYSQL_RES *result)
{
  String separator(256);
  MYSQL_ROW	cur;
  MYSQL_FIELD	*field;
  bool		*num_flag;

  num_flag=(bool*) my_alloca(sizeof(bool)*mysql_num_fields(result));
  if (column_types_flag)
  {
    print_field_types(result);
    if (!mysql_num_rows(result))
      return;
    mysql_field_seek(result,0);
  }
  separator.copy("+",1,charset_info);
  while ((field = mysql_fetch_field(result)))
  {
    uint length= column_names ? field->name_length : 0;
    if (quick)
      length=max(length,field->length);
    else
      length=max(length,field->max_length);
    if (length < 4 && !IS_NOT_NULL(field->flags))
      length=4;					// Room for "NULL"
    if (opt_binhex && is_binary_field(field))
      length= 2 + length * 2;
    field->max_length=(ulong) length;
    separator.fill(separator.length()+length+2,'-');
    separator.append('+');
  }
  separator.append('\0');                       // End marker for \0
  tee_puts((char*) separator.ptr(), PAGER);
  if (column_names)
  {
    mysql_field_seek(result,0);
    (void) tee_fputs("|", PAGER);
    for (uint off=0; (field = mysql_fetch_field(result)) ; off++)
    {
      uint name_length= (uint) strlen(field->name);
      uint numcells= charset_info->cset->numcells(charset_info,
                                                  field->name,
                                                  field->name + name_length);
      uint display_length= field->max_length + name_length - numcells;
      tee_fprintf(PAGER, " %-*s |",(int) min(display_length,
                                            MAX_COLUMN_LENGTH),
                  field->name);
      num_flag[off]= IS_NUM(field->type);
    }
    (void) tee_fputs("\n", PAGER);
    tee_puts((char*) separator.ptr(), PAGER);
  }

  while ((cur= mysql_fetch_row(result)))
  {
    if (interrupted_query)
      break;
    ulong *lengths= mysql_fetch_lengths(result);
    (void) tee_fputs("| ", PAGER);
    mysql_field_seek(result, 0);
    for (uint off= 0; off < mysql_num_fields(result); off++)
    {
      const char *buffer;
      uint data_length;
      uint field_max_length;
      uint visible_length;
      uint extra_padding;

      if (off)
        (void) tee_fputs(" ", PAGER);

      if (cur[off] == NULL)
      {
        buffer= "NULL";
        data_length= 4;
      } 
      else 
      {
        buffer= cur[off];
        data_length= (uint) lengths[off];
      }

      field= mysql_fetch_field(result);
      field_max_length= field->max_length;

      /* 
       How many text cells on the screen will this string span?  If it contains
       multibyte characters, then the number of characters we occupy on screen
       will be fewer than the number of bytes we occupy in memory.

       We need to find how much screen real-estate we will occupy to know how 
       many extra padding-characters we should send with the printing function.
      */
      visible_length= charset_info->cset->numcells(charset_info, buffer, buffer + data_length);
      extra_padding= (uint) (data_length - visible_length);

      if (opt_binhex && is_binary_field(field))
        print_as_hex(PAGER, cur[off], lengths[off], field_max_length);
      else if (field_max_length > MAX_COLUMN_LENGTH)
        tee_print_sized_data(buffer, data_length, MAX_COLUMN_LENGTH+extra_padding, FALSE);
      else
      {
        if (num_flag[off] != 0) /* if it is numeric, we right-justify it */
          tee_print_sized_data(buffer, data_length, field_max_length+extra_padding, TRUE);
        else 
          tee_print_sized_data(buffer, data_length, field_max_length+extra_padding, FALSE);
      }
      tee_fputs(" |", PAGER);
    }
    (void) tee_fputs("\n", PAGER);
  }
  tee_puts((char*) separator.ptr(), PAGER);
  my_afree((uchar*) num_flag);
}