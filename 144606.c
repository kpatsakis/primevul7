    //! Return a reference to one pixel value of one image of the list \const.
    const T& operator()(const unsigned int pos, const unsigned int x, const unsigned int y=0,
                        const unsigned int z=0, const unsigned int c=0) const {
      return (*this)[pos](x,y,z,c);