    //! Convert pixel values from YUV to RGB color spaces \newinstance.
    CImg<Tuchar> get_YUVtoRGB() const {
      return CImg< Tuchar>(*this,false).YUVtoRGB();