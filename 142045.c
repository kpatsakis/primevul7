static inline PixelTrait GetPixelRedTraits(const Image *magick_restrict image)
{
  return(image->channel_map[RedPixelChannel].traits);
}