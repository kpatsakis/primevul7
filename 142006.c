static inline PixelTrait GetPixelMagentaTraits(
  const Image *magick_restrict image)
{
  return(image->channel_map[MagentaPixelChannel].traits);
}