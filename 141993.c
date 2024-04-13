static inline PixelTrait GetPixelYellowTraits(
  const Image *magick_restrict image)
{
  return(image->channel_map[YellowPixelChannel].traits);
}