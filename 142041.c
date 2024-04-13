static inline PixelTrait GetPixelBlueTraits(const Image *magick_restrict image)
{
  return(image->channel_map[BluePixelChannel].traits);
}