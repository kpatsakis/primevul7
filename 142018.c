static inline PixelTrait GetPixelCrTraits(const Image *magick_restrict image)
{
  return(image->channel_map[CrPixelChannel].traits);
}