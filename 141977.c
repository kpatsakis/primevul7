static inline Quantum GetPixelAlpha(const Image *magick_restrict image,
  const Quantum *magick_restrict pixel)
{
  if (image->channel_map[AlphaPixelChannel].traits == UndefinedPixelTrait)
    return(OpaqueAlpha);
  return(pixel[image->channel_map[AlphaPixelChannel].offset]);
}