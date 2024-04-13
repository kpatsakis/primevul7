static inline Quantum GetPixelRed(const Image *magick_restrict image,
  const Quantum *magick_restrict pixel)
{
  return(pixel[image->channel_map[RedPixelChannel].offset]);
}