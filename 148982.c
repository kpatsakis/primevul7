static ssize_t TIFFReadCustomStream(unsigned char *data,const size_t count,
  void *user_data)
{
  PhotoshopProfile
    *profile;

  size_t
    total;

  ssize_t
    remaining;

  if (count == 0)
    return(0);
  profile=(PhotoshopProfile *) user_data;
  remaining=(MagickOffsetType) profile->length-profile->offset;
  if (remaining <= 0)
    return(-1);
  total=MagickMin(count, (size_t) remaining);
  (void) memcpy(data,profile->data->datum+profile->offset,total);
  profile->offset+=total;
  return(total);
}