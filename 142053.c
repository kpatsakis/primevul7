static inline void SetPixelY(const Image *magick_restrict image,
  const Quantum y,Quantum *magick_restrict pixel)
{
  pixel[image->channel_map[YPixelChannel].offset]=y;
}