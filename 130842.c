xmlencode_print(const char *src, uint length)
{
  if (!src)
    tee_fputs("NULL", PAGER);
  else
  {
    for (const char *p = src; length; p++, length--)
    {
      const char *t;
      if ((t = array_value(xmlmeta, *p)))
	tee_fputs(t, PAGER);
      else
	tee_putc(*p, PAGER);
    }
  }
}