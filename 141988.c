static inline void SetPixelBlue(const Image *magick_restrict image,
  const Quantum blue,Quantum *magick_restrict pixel)
{
  pixel[image->channel_map[BluePixelChannel].offset]=blue;
}