    **/
    CImgList<T>& assign(const unsigned int n, const unsigned int width, const unsigned int height,
                        const unsigned int depth, const unsigned int spectrum,
                        const double val0, const double val1, ...) {
      _CImgList_stdarg(double);
      return *this;