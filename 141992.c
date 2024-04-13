static inline void SetPixelIndex(const Image *magick_restrict image,
  const Quantum index,Quantum *magick_restrict pixel)
{
  if (image->channel_map[IndexPixelChannel].traits != UndefinedPixelTrait)
    pixel[image->channel_map[IndexPixelChannel].offset]=index;
}