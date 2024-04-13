get_bson (const char *filename)
{
   ssize_t len;
   uint8_t buf[4096];
   bson_t *b;
   char real_filename[256];
   int fd;

   bson_snprintf (
      real_filename, sizeof real_filename, BINARY_DIR "/%s", filename);
   real_filename[sizeof real_filename - 1] = '\0';

   if (-1 == (fd = bson_open (real_filename, O_RDONLY))) {
      fprintf (stderr, "Failed to bson_open: %s\n", real_filename);
      abort ();
   }
   len = bson_read (fd, buf, sizeof buf);
   BSON_ASSERT (len > 0);
   b = bson_new_from_data (buf, (uint32_t) len);
   bson_close (fd);

   return b;
}