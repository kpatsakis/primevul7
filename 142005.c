static inline ssize_t GetPixelLabel(const Image *magick_restrict image,
  const Quantum *magick_restrict pixel)
{
  return((ssize_t) pixel[image->channel_map[LabelPixelChannel].offset]);
}