static inline void SetPixelChannelAttributes(
  const Image *magick_restrict image,const PixelChannel channel,
  const PixelTrait traits,const ssize_t offset)
{
  assert((ssize_t) channel < MaxPixelChannels);
  assert(offset < MaxPixelChannels);
  image->channel_map[offset].channel=channel;
  image->channel_map[channel].offset=offset;
  image->channel_map[channel].traits=traits;
}