    //! Resize image to become a diagonal matrix \newinstance.
    CImg<T> get_diagonal() const {
      if (is_empty()) return *this;
      const unsigned int siz = (unsigned int)size();
      CImg<T> res(siz,siz,1,1,0);
      cimg_foroff(*this,off) res((unsigned int)off,(unsigned int)off) = (*this)[off];
      return res;