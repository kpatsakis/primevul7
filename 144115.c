    **/
    const CImg<T>& save_cimg(const char *const filename, const bool is_compressed=false) const {
      CImgList<T>(*this,true).save_cimg(filename,is_compressed);
      return *this;