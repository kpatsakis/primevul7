static inline Quantum GetPixelReadMask(const Image *magick_restrict image,
  const Quantum *magick_restrict pixel)
{
  if (image->channel_map[ReadMaskPixelChannel].traits == UndefinedPixelTrait)
    return((Quantum) QuantumRange);
  return(pixel[image->channel_map[ReadMaskPixelChannel].offset]);
}