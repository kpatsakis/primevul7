static unsigned short ReadDCMShort(DCMStreamInfo *stream_info,Image *image)
{
  int
    byte,
    shift;

  unsigned short
    value;

  if (image->compression != RLECompression)
    return(ReadBlobLSBShort(image));
  shift=image->depth < 16 ? 4 : 8;
  value=(unsigned short) ReadDCMByte(stream_info,image);
  byte=ReadDCMByte(stream_info,image);
  if (byte < 0)
    return(0);
  value|=(unsigned short) (byte << shift);
  return(value);
}