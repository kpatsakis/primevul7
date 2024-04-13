static inline MagickRealType GetPixelLuminance(
  const Image *magick_restrict image,const Quantum *magick_restrict pixel)
{
  MagickRealType
    intensity;

  if (image->colorspace != sRGBColorspace)
    {
      intensity=(MagickRealType) (
        0.212656f*pixel[image->channel_map[RedPixelChannel].offset]+
        0.715158f*pixel[image->channel_map[GreenPixelChannel].offset]+
        0.072186f*pixel[image->channel_map[BluePixelChannel].offset]);
      return(intensity);
    }
  intensity=(MagickRealType) (0.212656f*DecodePixelGamma((MagickRealType)
    pixel[image->channel_map[RedPixelChannel].offset])+0.715158f*
    DecodePixelGamma((MagickRealType)
    pixel[image->channel_map[GreenPixelChannel].offset])+0.072186f*
    DecodePixelGamma((MagickRealType)
    pixel[image->channel_map[BluePixelChannel].offset]));
  return(intensity);
}