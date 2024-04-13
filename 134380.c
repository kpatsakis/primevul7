MagickExport ChannelType SetImageChannelMask(Image *image,
  const ChannelType channel_mask)
{
  return(SetPixelChannelMask(image,channel_mask));
}