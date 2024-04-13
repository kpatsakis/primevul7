    **/
    CImgList(const unsigned int n, const unsigned int width, const unsigned int height,
             const unsigned int depth, const unsigned int spectrum, const double val0, const double val1, ...):
      _width(0),_allocated_width(0),_data(0) {
      _CImgList_stdarg(double);