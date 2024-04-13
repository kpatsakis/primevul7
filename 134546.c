LibRaw_colormatrix_type LibRaw::camera_color_type()
{
  if(C.cmatrix[0][0] <= 0.125) return LIBRAW_CMATRIX_NONE;
  if(P1.dng_version) return LIBRAW_CMATRIX_DNG;
  if(C.digitalBack_color) return LIBRAW_CMATRIX_DIGBACK;
  return LIBRAW_CMATRIX_OTHER;

}