static inline PixelTrait GetPixelYTraits(const Image *magick_restrict image)
{
  return(image->channel_map[YPixelChannel].traits);
}