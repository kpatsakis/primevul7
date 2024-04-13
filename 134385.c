static inline Quantum PushColormapIndex(Image *image,const Quantum index,
  MagickBooleanType *range_exception)
{
  if ((size_t) index < image->colors)
    return(index);
  *range_exception=MagickTrue;
  return((Quantum) 0);
}