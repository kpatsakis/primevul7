     **/
    double sum() const {
      double res = 0;
      cimg_for(*this,ptrs,T) res+=(double)*ptrs;
      return res;