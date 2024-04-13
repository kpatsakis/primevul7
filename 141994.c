static inline Quantum GetPixelCb(const Image *magick_restrict image,
  const Quantum *magick_restrict pixel)
{
  return(pixel[image->channel_map[CbPixelChannel].offset]);
}