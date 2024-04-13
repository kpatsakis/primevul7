safe_put_field(const char *pos,ulong length)
{
  if (!pos)
    tee_fputs("NULL", PAGER);
  else
  {
    if (opt_raw_data)
    {
      unsigned long i;
      /* Can't use tee_fputs(), it stops with NUL characters. */
      for (i= 0; i < length; i++, pos++)
        tee_putc(*pos, PAGER);
    }
    else for (const char *end=pos+length ; pos != end ; pos++)
    {
#ifdef USE_MB
      int l;
      if (use_mb(charset_info) &&
          (l = my_ismbchar(charset_info, pos, end)))
      {
	  while (l--)
	    tee_putc(*pos++, PAGER);
	  pos--;
	  continue;
      }
#endif
      if (!*pos)
	tee_fputs("\\0", PAGER); // This makes everything hard
      else if (*pos == '\t')
	tee_fputs("\\t", PAGER); // This would destroy tab format
      else if (*pos == '\n')
	tee_fputs("\\n", PAGER); // This too
      else if (*pos == '\\')
	tee_fputs("\\\\", PAGER);
	else
	tee_putc(*pos, PAGER);
    }
  }
}