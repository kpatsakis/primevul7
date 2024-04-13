Gif_ReadRecord(const Gif_Record *gifrec)
{
  return Gif_FullReadRecord(gifrec, GIF_READ_UNCOMPRESSED, 0, 0);
}