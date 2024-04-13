static MagickBooleanType ReadPSDChannelPixels(Image *image,
  const size_t channels,const size_t row,const ssize_t type,
  const unsigned char *pixels,ExceptionInfo *exception)
{
  Quantum
    pixel;

  register const unsigned char
    *p;

  register Quantum
    *q;

  register ssize_t
    x;

  size_t
    packet_size;

  p=pixels;
  q=GetAuthenticPixels(image,0,row,image->columns,1,exception);
  if (q == (Quantum *) NULL)
    return MagickFalse;
  packet_size=GetPSDPacketSize(image);
  for (x=0; x < (ssize_t) image->columns; x++)
  {
    if (packet_size == 1)
      pixel=ScaleCharToQuantum(*p++);
    else if (packet_size == 2)
      {
        unsigned short
          nibble;

        p=PushShortPixel(MSBEndian,p,&nibble);
        pixel=ScaleShortToQuantum(nibble);
      }
    else
      {
        MagickFloatType
          nibble;

        p=PushFloatPixel(MSBEndian,p,&nibble);
        pixel=ClampToQuantum((MagickRealType)QuantumRange*nibble);
      }
    if (image->depth > 1)
      {
        SetPSDPixel(image,channels,type,packet_size,pixel,q,exception);
        q+=GetPixelChannels(image);
      }
    else
      {
        ssize_t
          bit,
          number_bits;

        number_bits=image->columns-x;
        if (number_bits > 8)
          number_bits=8;
        for (bit = 0; bit < number_bits; bit++)
        {
          SetPSDPixel(image,channels,type,packet_size,(((unsigned char) pixel)
            & (0x01 << (7-bit))) != 0 ? 0 : QuantumRange,q,exception);
          q+=GetPixelChannels(image);
          x++;
        }
        if (x != (ssize_t) image->columns)
          x--;
        continue;
      }
  }
  return(SyncAuthenticPixels(image,exception));
}