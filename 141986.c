static inline PixelTrait GetPixelGrayTraits(const Image *magick_restrict image)
{
  return(image->channel_map[GrayPixelChannel].traits);
}