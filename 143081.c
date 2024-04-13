    //! Convert pixel values from xyY pixels to XYZ color spaces \newinstance.
    CImg<Tfloat> get_xyYtoXYZ() const {
      return CImg<Tfloat>(*this,false).xyYtoXYZ();