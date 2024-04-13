static inline MagickBooleanType IsPixelInfoGray(
  const PixelInfo *magick_restrict pixel)
{
  if ((AbsolutePixelValue(pixel->red-pixel->green) < MagickEpsilon) &&
      (AbsolutePixelValue(pixel->green-pixel->blue) < MagickEpsilon))
    return(MagickTrue);
  return(MagickFalse);
}