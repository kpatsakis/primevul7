static inline Quantum GetPixelIndex(const Image *magick_restrict image,
  const Quantum *magick_restrict pixel)
{
  if (image->channel_map[IndexPixelChannel].traits == UndefinedPixelTrait)
    return((Quantum) 0);
  return(pixel[image->channel_map[IndexPixelChannel].offset]);
}