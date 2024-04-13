    **/
    CImgList<T>& assign(const unsigned int n, const unsigned int width, const unsigned int height,
                        const unsigned int depth, const unsigned int spectrum, const int val0, const int val1, ...) {
      _CImgList_stdarg(int);
      return *this;