static inline void SetPixelMagenta(const Image *magick_restrict image,
  const Quantum magenta,Quantum *magick_restrict pixel)
{
  pixel[image->channel_map[MagentaPixelChannel].offset]=magenta;
}