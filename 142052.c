static inline void SetPixelYellow(const Image *magick_restrict image,
  const Quantum yellow,Quantum *magick_restrict pixel)
{
  pixel[image->channel_map[YellowPixelChannel].offset]=yellow;
}