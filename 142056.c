static inline Quantum GetPixela(const Image *magick_restrict image,
  const Quantum *magick_restrict pixel)
{
  return(pixel[image->channel_map[aPixelChannel].offset]);
}