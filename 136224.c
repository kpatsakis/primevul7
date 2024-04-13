write32 (FILE     *f,
	 guint32  *data,
	 gint      count)
{
  gint i;

  for (i = 0; i < count; i++)
	  data[i] = GUINT32_TO_LE (data[i]);
  
  return write8 (f, (guint8*) data, count * 4);
}