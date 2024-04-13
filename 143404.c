    template<typename t>
    CImg<doubleT> eval(const char *const expression, const CImg<t>& xyzc,
                       const CImgList<T> *const list_inputs=0, CImgList<T> *const list_outputs=0) {
      return _eval(this,expression,xyzc,list_inputs,list_outputs);