    //! Save image as a .cimg file \overloading.
    const CImg<T>& save_cimg(std::FILE *const file, const bool is_compressed=false) const {
      CImgList<T>(*this,true).save_cimg(file,is_compressed);
      return *this;