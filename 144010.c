    **/
    const CImg<T>& save_cimg(const char *const filename,
                             const unsigned int n0,
                             const unsigned int x0, const unsigned int y0,
                             const unsigned int z0, const unsigned int c0) const {
      CImgList<T>(*this,true).save_cimg(filename,n0,x0,y0,z0,c0);
      return *this;