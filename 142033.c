static inline PixelChannel GetPixelChannelChannel(
  const Image *magick_restrict image,const ssize_t offset)
{
  return(image->channel_map[offset].channel);
}