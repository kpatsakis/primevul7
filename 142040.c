static inline void SetPixelGreen(const Image *magick_restrict image,
  const Quantum green,Quantum *magick_restrict pixel)
{
  pixel[image->channel_map[GreenPixelChannel].offset]=green;
}