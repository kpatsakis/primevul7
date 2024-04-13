static inline PixelTrait GetPixelBlackTraits(
  const Image *magick_restrict image)
{
  return(image->channel_map[BlackPixelChannel].traits);
}