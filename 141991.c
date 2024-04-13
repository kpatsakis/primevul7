static inline void SetPixelWriteMask(const Image *magick_restrict image,
  const Quantum mask,Quantum *magick_restrict pixel)
{
  if (image->channel_map[WriteMaskPixelChannel].traits != UndefinedPixelTrait)
    pixel[image->channel_map[WriteMaskPixelChannel].offset]=mask;
}