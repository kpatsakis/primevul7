static inline MagickBooleanType IsPixelGray(const Image *magick_restrict image,
  const Quantum *magick_restrict pixel)
{
  MagickRealType
    green_blue,
    red_green;

  red_green=(MagickRealType) pixel[image->channel_map[RedPixelChannel].offset]-
    pixel[image->channel_map[GreenPixelChannel].offset];
  green_blue=(MagickRealType)
    pixel[image->channel_map[GreenPixelChannel].offset]-
    pixel[image->channel_map[BluePixelChannel].offset];
  if ((AbsolutePixelValue(red_green) < MagickEpsilon) &&
      (AbsolutePixelValue(green_blue) < MagickEpsilon))
    return(MagickTrue);
  return(MagickFalse);
}