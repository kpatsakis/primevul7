    //! Cumulate image values, optionally along specified axis \newinstance.
    CImg<Tlong> get_cumulate(const char axis=0) const {
      return CImg<Tlong>(*this,false).cumulate(axis);