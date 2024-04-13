static MagickBooleanType IsCIN(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(MagickFalse);
  if (memcmp(magick,"\200\052\137\327",4) == 0)
    return(MagickTrue);
  return(MagickFalse);
}