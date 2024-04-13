static inline MagickRealType GetPixelInfoLuminance(
  const PixelInfo *magick_restrict pixel)
{
  MagickRealType
    intensity;

  if (pixel->colorspace != sRGBColorspace)
    {
      intensity=(MagickRealType) (0.212656f*pixel->red+0.715158f*pixel->green+
        0.072186f*pixel->blue);
      return(intensity);
    }
  intensity=(MagickRealType) (0.212656f*DecodePixelGamma(pixel->red)+
    0.715158f*DecodePixelGamma(pixel->green)+
    0.072186f*DecodePixelGamma(pixel->blue));
  return(intensity);
}