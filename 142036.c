static inline void SetPixelReadMask(const Image *magick_restrict image,
  const Quantum mask,Quantum *magick_restrict pixel)
{
  if (image->channel_map[ReadMaskPixelChannel].traits != UndefinedPixelTrait)
    pixel[image->channel_map[ReadMaskPixelChannel].offset]=mask;
}