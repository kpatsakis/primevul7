    //! Save image as a sub-image into an existing .cimg file \overloading.
    const CImg<T>& save_cimg(std::FILE *const file,
			     const unsigned int n0,
			     const unsigned int x0, const unsigned int y0,
			     const unsigned int z0, const unsigned int c0) const {
      CImgList<T>(*this,true).save_cimg(file,n0,x0,y0,z0,c0);
      return *this;