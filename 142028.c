static inline void SetPixelL(const Image *magick_restrict image,const Quantum L,
  Quantum *magick_restrict pixel)
{
  if (image->channel_map[LPixelChannel].traits != UndefinedPixelTrait)
    pixel[image->channel_map[LPixelChannel].offset]=L;
}