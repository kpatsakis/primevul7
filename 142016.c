static inline PixelTrait GetPixelReadMaskTraits(
  const Image *magick_restrict image)
{
  return(image->channel_map[ReadMaskPixelChannel].traits);
}