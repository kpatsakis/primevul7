static inline Quantum GetPixelCompositeMask(const Image *magick_restrict image,
  const Quantum *magick_restrict pixel)
{
  if (image->channel_map[CompositeMaskPixelChannel].traits == UndefinedPixelTrait)
    return((Quantum) QuantumRange);
  return(pixel[image->channel_map[CompositeMaskPixelChannel].offset]);
}