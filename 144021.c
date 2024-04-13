    **/
    double magnitude(const int magnitude_type=2) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "magnitude(): Empty instance.",
                                    cimg_instance);
      double res = 0;
      switch (magnitude_type) {
      case -1 : {
        cimg_for(*this,ptrs,T) { const double val = (double)cimg::abs(*ptrs); if (val>res) res = val; }
      } break;
      case 1 : {
        cimg_for(*this,ptrs,T) res+=(double)cimg::abs(*ptrs);
      } break;
      default : {
        cimg_for(*this,ptrs,T) res+=(double)cimg::sqr(*ptrs);
        res = (double)std::sqrt(res);
      }
      }
      return res;