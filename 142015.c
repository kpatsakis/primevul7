static inline void SetPixelRed(const Image *magick_restrict image,
  const Quantum red,Quantum *magick_restrict pixel)
{
  pixel[image->channel_map[RedPixelChannel].offset]=red;
}