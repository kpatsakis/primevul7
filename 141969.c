static inline void SetPixela(const Image *magick_restrict image,
  const Quantum a,Quantum *magick_restrict pixel)
{
  if (image->channel_map[aPixelChannel].traits != UndefinedPixelTrait)
    pixel[image->channel_map[aPixelChannel].offset]=a;
}