static inline void SetPixelCompositeMask(const Image *magick_restrict image,
  const Quantum mask,Quantum *magick_restrict pixel)
{
  if (image->channel_map[CompositeMaskPixelChannel].traits != UndefinedPixelTrait)
    pixel[image->channel_map[CompositeMaskPixelChannel].offset]=mask;
}