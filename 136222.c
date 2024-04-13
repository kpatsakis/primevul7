write8 (FILE     *f,
	guint8   *data,
	gint      count)
{
  gint bytes;
  gint written;

  written = 0;
  while (count > 0)
    {
      bytes = fwrite ((char*) data, sizeof (char), count, f);
      if (bytes <= 0)
        break;
      count -= bytes;
      data += bytes;
      written += bytes;
    }

  return written;
}