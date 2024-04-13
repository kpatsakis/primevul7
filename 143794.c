    //! Convert pixel values from Lab to RGB color spaces \newinstance.
    CImg<Tuchar> get_LabtoRGB(const bool use_D65=true) const {
      return CImg<Tuchar>(*this,false).LabtoRGB(use_D65);