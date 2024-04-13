      \param y1 Ending Y-coordinates of image regions to read (~0U for max).
      \param z1 Ending Z-coordinates of image regions to read (~0U for max).
      \param c1 Ending C-coordinates of image regions to read (~0U for max).
    **/
    CImgList<T>& load_cimg(const char *const filename,
                           const unsigned int n0, const unsigned int n1,
                           const unsigned int x0, const unsigned int y0,
                           const unsigned int z0, const unsigned int c0,