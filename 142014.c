static inline PixelTrait GetPixelCbTraits(const Image *magick_restrict image)
{
  return(image->channel_map[CbPixelChannel].traits);
}