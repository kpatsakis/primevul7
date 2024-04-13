    //! Return a pointer to a located pixel value \const.
    const T* data(const unsigned int x, const unsigned int y=0, const unsigned int z=0, const unsigned int c=0) const {
      return const_cast<CImg<T>*>(this)->data(x,y,z,c);