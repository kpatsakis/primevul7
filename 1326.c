static MagickBooleanType IsWMF(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(MagickFalse);
  if (memcmp(magick,"\327\315\306\232",4) == 0)
    return(MagickTrue);
  if (memcmp(magick,"\001\000\011\000",4) == 0)
    return(MagickTrue);
  return(MagickFalse);
}