static inline void SetPixelChannel(const Image *magick_restrict image,
  const PixelChannel channel,const Quantum quantum,
  Quantum *magick_restrict pixel)
{
  if (image->channel_map[channel].traits != UndefinedPixelTrait)
    pixel[image->channel_map[channel].offset]=quantum;
}