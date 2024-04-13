static inline Quantum GetPixelWriteMask(const Image *magick_restrict image,
  const Quantum *magick_restrict pixel)
{
  if (image->channel_map[WriteMaskPixelChannel].traits == UndefinedPixelTrait)
    return((Quantum) QuantumRange);
  return(pixel[image->channel_map[WriteMaskPixelChannel].offset]);
}