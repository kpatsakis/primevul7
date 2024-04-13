static inline void SetPixelBlack(const Image *magick_restrict image,
  const Quantum black,Quantum *magick_restrict pixel)
{
  if (image->channel_map[BlackPixelChannel].traits != UndefinedPixelTrait)
    pixel[image->channel_map[BlackPixelChannel].offset]=black;
}