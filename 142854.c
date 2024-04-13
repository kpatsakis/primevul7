     **/
    double trace() const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "trace(): Empty instance.",
                                    cimg_instance);
      double res = 0;
      cimg_forX(*this,k) res+=(double)(*this)(k,k);
      return res;