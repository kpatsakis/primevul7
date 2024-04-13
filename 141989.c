static inline PixelTrait GetPixelIndexTraits(
  const Image *magick_restrict image)
{
  return(image->channel_map[IndexPixelChannel].traits);
}