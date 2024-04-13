static inline PixelTrait GetPixelGreenTraits(
  const Image *magick_restrict image)
{
  return(image->channel_map[GreenPixelChannel].traits);
}