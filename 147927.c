gdev_mjc_paper_size(gx_device *dev)
{
  int width = dev->MediaSize[0];
  int height = dev->MediaSize[1];

  if (width == 1190 && height == 1684)
    return PAPER_SIZE_A2;
  else
    return PAPER_SIZE_A4;
}