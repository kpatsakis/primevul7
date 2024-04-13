static inline void SetPixelAlpha(const Image *magick_restrict image,
  const Quantum alpha,Quantum *magick_restrict pixel)
{
  if (image->channel_map[AlphaPixelChannel].traits != UndefinedPixelTrait)
    pixel[image->channel_map[AlphaPixelChannel].offset]=alpha;
}