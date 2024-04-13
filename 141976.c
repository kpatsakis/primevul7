static inline void SetPixelCb(const Image *magick_restrict image,
  const Quantum cb,Quantum *magick_restrict pixel)
{
  pixel[image->channel_map[CbPixelChannel].offset]=cb;
}