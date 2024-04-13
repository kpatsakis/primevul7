    //! Access to a pixel value \const.
    const T& operator()(const unsigned int x, const unsigned int y=0,
                        const unsigned int z=0, const unsigned int c=0) const {
      return const_cast<CImg<T>*>(this)->operator()(x,y,z,c);