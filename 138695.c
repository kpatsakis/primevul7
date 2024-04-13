load_image (const gchar  *filename,
            gboolean      thumbnail,
            GError      **error)
{
  FILE     *fd;
  guchar    buf[16];
  guchar    c;
  CMap      localColorMap;
  gint      grayScale;
  gboolean  useGlobalColormap;
  gint      bitPixel;
  gint      imageCount = 0;
  gchar     version[4];
  gint32    image_ID = -1;

  fd = g_fopen (filename, "rb");

  if (! fd)
    {
      g_set_error (error, G_FILE_ERROR, g_file_error_from_errno (errno),
                   _("Could not open '%s' for reading: %s"),
                   gimp_filename_to_utf8 (filename), g_strerror (errno));
      return -1;
    }

  gimp_progress_init_printf (_("Opening '%s'"),
                             gimp_filename_to_utf8 (filename));

  if (! ReadOK (fd, buf, 6))
    {
      g_message ("Error reading magic number");
      return -1;
    }

  if (strncmp ((gchar *) buf, "GIF", 3) != 0)
    {
      g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                   "%s", _("This is not a GIF file"));
      return -1;
    }

  strncpy (version, (gchar *) buf + 3, 3);
  version[3] = '\0';

  if ((strcmp (version, "87a") != 0) && (strcmp (version, "89a") != 0))
    {
      g_message ("Bad version number, not '87a' or '89a'");
      return -1;
    }

  if (! ReadOK (fd, buf, 7))
    {
      g_message ("Failed to read screen descriptor");
      return -1;
    }

  GifScreen.Width           = LM_to_uint (buf[0], buf[1]);
  GifScreen.Height          = LM_to_uint (buf[2], buf[3]);
  GifScreen.BitPixel        = 2 << (buf[4] & 0x07);
  GifScreen.ColorResolution = (((buf[4] & 0x70) >> 3) + 1);
  GifScreen.Background      = buf[5];
  GifScreen.AspectRatio     = buf[6];

  if (BitSet (buf[4], LOCALCOLORMAP))
    {
      /* Global Colormap */
      if (! ReadColorMap (fd, GifScreen.BitPixel, GifScreen.ColorMap,
                          &GifScreen.GrayScale))
        {
          g_message ("Error reading global colormap");
          return -1;
        }
    }

  if (GifScreen.AspectRatio != 0 && GifScreen.AspectRatio != 49)
    {
      g_message (_("Non-square pixels.  Image might look squashed."));
    }


  highest_used_index = 0;

  while (TRUE)
    {
      if (! ReadOK (fd, &c, 1))
        {
          g_message ("EOF / read error on image data");
          return image_ID; /* will be -1 if failed on first image! */
        }

      if (c == ';')
        {
          /* GIF terminator */
          return image_ID;
        }

      if (c == '!')
        {
          /* Extension */
          if (! ReadOK (fd, &c, 1))
            {
              g_message ("EOF / read error on extension function code");
              return image_ID; /* will be -1 if failed on first image! */
            }

          DoExtension (fd, c);
          continue;
        }

      if (c != ',')
        {
          /* Not a valid start character */
          g_printerr ("GIF: bogus character 0x%02x, ignoring.\n", (int) c);
          continue;
        }

      ++imageCount;

      if (! ReadOK (fd, buf, 9))
        {
          g_message ("Couldn't read left/top/width/height");
          return image_ID; /* will be -1 if failed on first image! */
        }

      useGlobalColormap = !BitSet (buf[8], LOCALCOLORMAP);

      bitPixel = 1 << ((buf[8] & 0x07) + 1);

      if (! useGlobalColormap)
        {
          if (! ReadColorMap (fd, bitPixel, localColorMap, &grayScale))
            {
              g_message ("Error reading local colormap");
              return image_ID; /* will be -1 if failed on first image! */
            }

          image_ID = ReadImage (fd, filename, LM_to_uint (buf[4], buf[5]),
                                LM_to_uint (buf[6], buf[7]),
                                localColorMap, bitPixel,
                                grayScale,
                                BitSet (buf[8], INTERLACE), imageCount,
                                (guint) LM_to_uint (buf[0], buf[1]),
                                (guint) LM_to_uint (buf[2], buf[3]),
                                GifScreen.Width,
                                GifScreen.Height);
        }
      else
        {
          image_ID = ReadImage (fd, filename, LM_to_uint (buf[4], buf[5]),
                                LM_to_uint (buf[6], buf[7]),
                                GifScreen.ColorMap, GifScreen.BitPixel,
                                GifScreen.GrayScale,
                                BitSet (buf[8], INTERLACE), imageCount,
                                (guint) LM_to_uint (buf[0], buf[1]),
                                (guint) LM_to_uint (buf[2], buf[3]),
                                GifScreen.Width,
                                GifScreen.Height);
        }

      if (comment_parasite != NULL)
        {
          if (! thumbnail)
            gimp_image_attach_parasite (image_ID, comment_parasite);

          gimp_parasite_free (comment_parasite);
          comment_parasite = NULL;
        }

      /* If we are loading a thumbnail, we stop after the first frame. */
      if (thumbnail)
        break;
    }

  return image_ID;
}