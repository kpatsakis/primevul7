static MagickBooleanType CLAHE(const RectangleInfo *clahe_info,
  const RectangleInfo *tile_info,const RangeInfo *range_info,
  const size_t number_bins,const double clip_limit,unsigned short *pixels)
{
  MemoryInfo
    *tile_cache;

  register unsigned short
    *p;

  size_t
    limit,
    *tiles;

  ssize_t
    y;

  unsigned short
    lut[NumberCLAHEGrays];

  /*
    Constrast limited adapted histogram equalization.
  */
  if (clip_limit == 1.0)
    return(MagickTrue);
  tile_cache=AcquireVirtualMemory((size_t) clahe_info->x*clahe_info->y,
    number_bins*sizeof(*tiles));
  if (tile_cache == (MemoryInfo *) NULL)
    return(MagickFalse);
  tiles=(size_t *) GetVirtualMemoryBlob(tile_cache);
  limit=(size_t) (clip_limit*(tile_info->width*tile_info->height)/number_bins);
  if (limit < 1UL)
    limit=1UL;
  /*
    Generate greylevel mappings for each tile.
  */
  GenerateCLAHELut(range_info,number_bins,lut);
  p=pixels;
  for (y=0; y < (ssize_t) clahe_info->y; y++)
  {
    register ssize_t
      x;

    for (x=0; x < (ssize_t) clahe_info->x; x++)
    {
      size_t
        *histogram;

      histogram=tiles+(number_bins*(y*clahe_info->x+x));
      GenerateCLAHEHistogram(clahe_info,tile_info,number_bins,lut,p,histogram);
      ClipCLAHEHistogram((double) limit,number_bins,histogram);
      MapCLAHEHistogram(range_info,number_bins,tile_info->width*
        tile_info->height,histogram);
      p+=tile_info->width;
    }
    p+=clahe_info->width*(tile_info->height-1);
  }
  /*
    Interpolate greylevel mappings to get CLAHE image.
  */
  p=pixels;
  for (y=0; y <= (ssize_t) clahe_info->y; y++)
  {
    OffsetInfo
      offset;

    RectangleInfo
      tile;

    register ssize_t
      x;

    tile.height=tile_info->height;
    tile.y=y-1;
    offset.y=tile.y+1;
    if (y == 0)
      {
        /*
          Top row.
        */
        tile.height=tile_info->height >> 1;
        tile.y=0;
        offset.y=0;
      }
    else
      if (y == (ssize_t) clahe_info->y)
        {
          /*
            Bottom row.
          */
          tile.height=(tile_info->height+1) >> 1;
          tile.y=clahe_info->y-1;
          offset.y=tile.y;
        }
    for (x=0; x <= (ssize_t) clahe_info->x; x++)
    {
      tile.width=tile_info->width;
      tile.x=x-1;
      offset.x=tile.x+1;
      if (x == 0)
        {
          /*
            Left column.
          */
          tile.width=tile_info->width >> 1;
          tile.x=0;
          offset.x=0;
        }
      else
        if (x == (ssize_t) clahe_info->x)
          {
            /*
              Right column.
            */
            tile.width=(tile_info->width+1) >> 1;
            tile.x=clahe_info->x-1;
            offset.x=tile.x;
          }
      InterpolateCLAHE(clahe_info,
        tiles+(number_bins*(tile.y*clahe_info->x+tile.x)),     /* Q12 */
        tiles+(number_bins*(tile.y*clahe_info->x+offset.x)),   /* Q22 */
        tiles+(number_bins*(offset.y*clahe_info->x+tile.x)),   /* Q11 */
        tiles+(number_bins*(offset.y*clahe_info->x+offset.x)), /* Q21 */
        &tile,lut,p);
      p+=tile.width;
    }
    p+=clahe_info->width*(tile.height-1);
  }
  tile_cache=RelinquishVirtualMemory(tile_cache);
  return(MagickTrue);
}