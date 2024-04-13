static inline MagickBooleanType IsPixelEquivalent(
  const Image *magick_restrict image,const Quantum *magick_restrict p,
  const PixelInfo *magick_restrict q)
{
  MagickRealType
    alpha,
    beta,
    color;

  color=(MagickRealType) p[image->channel_map[AlphaPixelChannel].offset];
  alpha=image->alpha_trait == UndefinedPixelTrait ? (MagickRealType)
    OpaqueAlpha : color;
  beta=q->alpha_trait == UndefinedPixelTrait ? (MagickRealType) OpaqueAlpha :
    q->alpha;
  if (AbsolutePixelValue(alpha-beta) >= MagickEpsilon)
    return(MagickFalse);
  if ((AbsolutePixelValue(alpha-TransparentAlpha) < MagickEpsilon) ||
      (AbsolutePixelValue(beta-TransparentAlpha) < MagickEpsilon))
    return(MagickTrue);  /* no color component if pixel is transparent */
  color=(MagickRealType) p[image->channel_map[RedPixelChannel].offset];
  if (AbsolutePixelValue(color-q->red) >= MagickEpsilon)
    return(MagickFalse);
  color=(MagickRealType) p[image->channel_map[GreenPixelChannel].offset];
  if (AbsolutePixelValue(color-q->green) >= MagickEpsilon)
    return(MagickFalse);
  color=(MagickRealType) p[image->channel_map[BluePixelChannel].offset];
  if (AbsolutePixelValue(color-q->blue) >= MagickEpsilon)
    return(MagickFalse);
  if (image->colorspace == CMYKColorspace)
    {
      color=(MagickRealType) p[image->channel_map[BlackPixelChannel].offset];
      if (AbsolutePixelValue(color-q->black) >= MagickEpsilon)
        return(MagickFalse);
    }
  return(MagickTrue);
}