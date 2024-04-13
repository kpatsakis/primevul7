static inline void SetPixelb(const Image *magick_restrict image,
  const Quantum b,Quantum *magick_restrict pixel)
{
  if (image->channel_map[bPixelChannel].traits != UndefinedPixelTrait)
    pixel[image->channel_map[bPixelChannel].offset]=b;
}