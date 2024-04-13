    **/
    CImgList<Tfloat> get_FFT(const bool is_inverse=false) const {
      CImgList<Tfloat> res(*this,CImg<Tfloat>());
      CImg<Tfloat>::FFT(res[0],res[1],is_inverse);
      return res;