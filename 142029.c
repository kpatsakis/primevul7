static inline MagickRealType GetPixelLuma(const Image *magick_restrict image,
  const Quantum *magick_restrict pixel)
{
  MagickRealType
    intensity;

  intensity=(MagickRealType) (
    0.212656f*pixel[image->channel_map[RedPixelChannel].offset]+
    0.715158f*pixel[image->channel_map[GreenPixelChannel].offset]+
    0.072186f*pixel[image->channel_map[BluePixelChannel].offset]);
  return(intensity);
}