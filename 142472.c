static int32 TIFFReadPixels(TIFF *tiff,const tsample_t sample,const ssize_t row,
  tdata_t scanline)
{
  int32
    status;

  status=TIFFReadScanline(tiff,scanline,(uint32) row,sample);
  return(status);
}