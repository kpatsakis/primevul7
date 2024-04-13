static inline void SetPixelCyan(const Image *magick_restrict image,
  const Quantum cyan,Quantum *magick_restrict pixel)
{
  pixel[image->channel_map[CyanPixelChannel].offset]=cyan;
}