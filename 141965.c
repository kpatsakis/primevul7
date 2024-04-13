static inline MagickRealType GetPixelInfoChannel(
  const PixelInfo *magick_restrict pixel_info,const PixelChannel channel)
{
  switch (channel)
  {
    case RedPixelChannel: return(pixel_info->red);
    case GreenPixelChannel: return(pixel_info->green);
    case BluePixelChannel: return(pixel_info->blue);
    case BlackPixelChannel: return(pixel_info->black);
    case AlphaPixelChannel: return(pixel_info->alpha);
    case IndexPixelChannel: return(pixel_info->index);
    default: return((MagickRealType) 0.0);
  }
}