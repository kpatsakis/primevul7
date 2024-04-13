static inline MagickBooleanType IsPixelInfoEquivalent(
  const PixelInfo *magick_restrict p,const PixelInfo *magick_restrict q)
{
  MagickRealType
    alpha,
    beta;

  alpha=p->alpha_trait == UndefinedPixelTrait ? (MagickRealType) OpaqueAlpha :
    p->alpha;
  beta=q->alpha_trait == UndefinedPixelTrait ? (MagickRealType) OpaqueAlpha :
    q->alpha;
  if (AbsolutePixelValue(alpha-beta) >= MagickEpsilon)
    return(MagickFalse);
  if ((AbsolutePixelValue(alpha-TransparentAlpha) < MagickEpsilon) ||
      (AbsolutePixelValue(beta-TransparentAlpha) < MagickEpsilon))
    return(MagickTrue);  /* no color component if pixel is transparent */
  if (AbsolutePixelValue(p->red-q->red) >= MagickEpsilon)
    return(MagickFalse);
  if (AbsolutePixelValue(p->green-q->green) >= MagickEpsilon)
    return(MagickFalse);
  if (AbsolutePixelValue(p->blue-q->blue) >= MagickEpsilon)
    return(MagickFalse);
  if (p->colorspace == CMYKColorspace)
    {
      if (AbsolutePixelValue(p->black-q->black) >= MagickEpsilon)
        return(MagickFalse);
    }
  return(MagickTrue);
}