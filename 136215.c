write16 (FILE     *f,
	 guint16  *data,
	 gint      count)
{
  gint i;

  for (i = 0; i < count; i++)
	  data[i] = GUINT16_TO_LE (data[i]);

  return write8 (f, (guint8*) data, count * 2);
}