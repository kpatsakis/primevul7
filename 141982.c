static inline MagickRealType GetPixelInfoLuma(
  const PixelInfo *magick_restrict pixel)
{
  MagickRealType
    intensity;

  if (pixel->colorspace == sRGBColorspace)
    {
      intensity=(MagickRealType) (0.212656f*pixel->red+0.715158f*pixel->green+
        0.072186f*pixel->blue);
      return(intensity);
    }
  intensity=(MagickRealType) (0.212656f*EncodePixelGamma(pixel->red)+
    0.715158f*EncodePixelGamma(pixel->green)+
    0.072186f*EncodePixelGamma(pixel->blue));
  return(intensity);
}