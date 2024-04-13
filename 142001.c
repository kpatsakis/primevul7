static inline PixelTrait GetPixelCyanTraits(const Image *magick_restrict image)
{
  return(image->channel_map[CyanPixelChannel].traits);
}