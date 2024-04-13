Gif_FullReadFile(FILE *f, int read_flags,
		 const char* landmark, Gif_ReadErrorHandler h)
{
  Gif_Reader grr;
  if (!f) return 0;
  grr.f = f;
  grr.pos = 0;
  grr.is_record = 0;
  grr.byte_getter = file_byte_getter;
  grr.block_getter = file_block_getter;
  grr.eofer = file_eofer;
  return read_gif(&grr, read_flags, landmark, h);
}