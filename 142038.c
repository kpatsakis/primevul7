static inline PixelTrait GetPixelChannelTraits(
  const Image *magick_restrict image,const PixelChannel channel)
{
  return(image->channel_map[channel].traits);
}