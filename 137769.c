Gif_FullReadRecord(const Gif_Record *gifrec, int read_flags,
		   const char* landmark, Gif_ReadErrorHandler h)
{
  Gif_Reader grr;
  if (!gifrec) return 0;
  make_data_reader(&grr, gifrec->data, gifrec->length);
  if (read_flags & GIF_READ_CONST_RECORD)
    read_flags |= GIF_READ_COMPRESSED;
  return read_gif(&grr, read_flags, landmark, h);
}