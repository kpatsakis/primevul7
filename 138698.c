GetDataBlock (FILE   *fd,
              guchar *buf)
{
  guchar count;

  if (! ReadOK (fd, &count, 1))
    {
      g_message ("Error in getting DataBlock size");
      return -1;
    }

  ZeroDataBlock = count == 0;

  if ((count != 0) && (! ReadOK (fd, buf, count)))
    {
      g_message ("Error in reading DataBlock");
      return -1;
    }

  return count;
}