    //! Access to a pixel value \const.
    const T& operator()(const unsigned int x, const unsigned int y, const unsigned int z, const unsigned int c,
                        const ulongT wh, const ulongT whd=0) const {
      cimg::unused(wh,whd);
      return (*this)(x,y,z,c);