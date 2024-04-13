     **/
    double product() const {
      if (is_empty()) return 0;
      double res = 1;
      cimg_for(*this,ptrs,T) res*=(double)*ptrs;
      return res;