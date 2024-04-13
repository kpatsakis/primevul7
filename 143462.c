  template<typename T>
  inline CImg<_cimg_Tfloat> pseudoinvert(const CImg<T>& instance) {
    return instance.get_pseudoinvert();