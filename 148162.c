static void InterpolateCLAHE(const RectangleInfo *clahe_info,const size_t *Q12,
  const size_t *Q22,const size_t *Q11,const size_t *Q21,
  const RectangleInfo *tile,const unsigned short *lut,unsigned short *pixels)
{
  ssize_t
    y;

  unsigned short
    intensity;

  /*
    Bilinear interpolate four tiles to eliminate boundary artifacts.
  */
  for (y=(ssize_t) tile->height; y > 0; y--)
  {
    register ssize_t
      x;

    for (x=(ssize_t) tile->width; x > 0; x--)
    {
      intensity=lut[*pixels];
      *pixels++=(unsigned short ) (PerceptibleReciprocal((double) tile->width*
        tile->height)*(y*(x*Q12[intensity]+(tile->width-x)*Q22[intensity])+
        (tile->height-y)*(x*Q11[intensity]+(tile->width-x)*Q21[intensity])));
    }
    pixels+=(clahe_info->width-tile->width);
  }
}