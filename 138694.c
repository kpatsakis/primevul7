DoExtension (FILE *fd,
             gint  label)
{
  static guchar  buf[256];
  gchar         *str;

  switch (label)
    {
    case 0x01:                  /* Plain Text Extension */
      str = "Plain Text Extension";
#ifdef notdef
      if (GetDataBlock (fd, (guchar *) buf) == 0)
        ;

      lpos       = LM_to_uint (buf[0], buf[1]);
      tpos       = LM_to_uint (buf[2], buf[3]);
      width      = LM_to_uint (buf[4], buf[5]);
      height     = LM_to_uint (buf[6], buf[7]);
      cellw      = buf[8];
      cellh      = buf[9];
      foreground = buf[10];
      background = buf[11];

      while (GetDataBlock (fd, (guchar *) buf) > 0)
        {
          PPM_ASSIGN (image[ypos][xpos],
                      cmap[CM_RED][v],
                      cmap[CM_GREEN][v],
                      cmap[CM_BLUE][v]);
          ++index;
        }

      return FALSE;
#else
      break;
#endif

    case 0xff:                  /* Application Extension */
      str = "Application Extension";
      break;
    case 0xfe:                  /* Comment Extension */
      str = "Comment Extension";
      while (GetDataBlock (fd, (guchar *) buf) > 0)
        {
          gchar *comment = (gchar *) buf;

          if (! g_utf8_validate (comment, -1, NULL))
            continue;

          if (comment_parasite)
            gimp_parasite_free (comment_parasite);

          comment_parasite = gimp_parasite_new ("gimp-comment",
                                                GIMP_PARASITE_PERSISTENT,
                                                strlen (comment) + 1, comment);
        }
      return TRUE;
      break;

    case 0xf9:                  /* Graphic Control Extension */
      str = "Graphic Control Extension";
      (void) GetDataBlock (fd, (guchar *) buf);
      Gif89.disposal  = (buf[0] >> 2) & 0x7;
      Gif89.inputFlag = (buf[0] >> 1) & 0x1;
      Gif89.delayTime = LM_to_uint (buf[1], buf[2]);
      if ((buf[0] & 0x1) != 0)
        Gif89.transparent = buf[3];
      else
        Gif89.transparent = -1;

      while (GetDataBlock (fd, (guchar *) buf) > 0);

      return FALSE;
      break;

    default:
      str = (gchar *)buf;
      sprintf ((gchar *)buf, "UNKNOWN (0x%02x)", label);
      break;
    }

#ifdef GIFDEBUG
  g_print ("GIF: got a '%s'\n", str);
#endif

  while (GetDataBlock (fd, (guchar *) buf) > 0);

  return FALSE;
}