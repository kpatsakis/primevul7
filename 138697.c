ReadColorMap (FILE *fd,
              gint  number,
              CMap  buffer,
              gint *format)
{
  guchar rgb[3];
  gint   flag;
  gint   i;

  flag = TRUE;

  for (i = 0; i < number; ++i)
    {
      if (! ReadOK (fd, rgb, sizeof (rgb)))
        return FALSE;

      buffer[CM_RED][i] = rgb[0];
      buffer[CM_GREEN][i] = rgb[1];
      buffer[CM_BLUE][i] = rgb[2];

      flag &= (rgb[0] == rgb[1] && rgb[1] == rgb[2]);
    }

  *format = (flag) ? GRAYSCALE : COLOR;

  return TRUE;
}