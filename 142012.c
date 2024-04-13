static inline PixelTrait GetPixelAlphaTraits(
  const Image *magick_restrict image)
{
  return(image->channel_map[AlphaPixelChannel].traits);
}