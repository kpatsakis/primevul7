static inline MagickBooleanType IsPixelInfoMonochrome(
  const PixelInfo *magick_restrict pixel_info)
{
  MagickRealType
    green_blue,
    red_green;

  if ((AbsolutePixelValue(pixel_info->red) >= MagickEpsilon) ||
      (AbsolutePixelValue(pixel_info->red-QuantumRange) >= MagickEpsilon))
    return(MagickFalse);
  red_green=pixel_info->red-pixel_info->green;
  green_blue=pixel_info->green-pixel_info->blue;
  if ((AbsolutePixelValue(red_green) < MagickEpsilon) &&
      (AbsolutePixelValue(green_blue) < MagickEpsilon))
    return(MagickTrue);
  return(MagickFalse);
}