static inline void SetPixelGray(const Image *magick_restrict image,
  const Quantum gray,Quantum *magick_restrict pixel)
{
  pixel[image->channel_map[GrayPixelChannel].offset]=gray;
}