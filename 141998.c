static inline void SetPixelCr(const Image *magick_restrict image,
  const Quantum cr,Quantum *magick_restrict pixel)
{
  pixel[image->channel_map[CrPixelChannel].offset]=cr;
}