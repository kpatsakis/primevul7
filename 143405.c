    template<typename t>
    double variance_mean(const unsigned int variance_method, t& mean) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "variance_mean(): Empty instance.",
                                    cimg_instance);

      double variance = 0, average = 0;
      const ulongT siz = size();
      switch (variance_method) {
      case 0 : { // Least mean square (standard definition)
        double S = 0, S2 = 0;
        cimg_for(*this,ptrs,T) { const double val = (double)*ptrs; S+=val; S2+=val*val; }
        variance = (S2 - S*S/siz)/siz;
        average = S;
      } break;
      case 1 : { // Least mean square (robust definition)
        double S = 0, S2 = 0;
        cimg_for(*this,ptrs,T) { const double val = (double)*ptrs; S+=val; S2+=val*val; }
        variance = siz>1?(S2 - S*S/siz)/(siz - 1):0;
        average = S;
      } break;
      case 2 : { // Least Median of Squares (MAD)
        CImg<Tfloat> buf(*this,false);
        buf.sort();
        const ulongT siz2 = siz>>1;
        const double med_i = (double)buf[siz2];
        cimg_for(buf,ptrs,Tfloat) {
          const double val = (double)*ptrs; *ptrs = (Tfloat)cimg::abs(val - med_i); average+=val;
        }
        buf.sort();
        const double sig = (double)(1.4828*buf[siz2]);
        variance = sig*sig;
      } break;
      default : { // Least trimmed of Squares
        CImg<Tfloat> buf(*this,false);
        const ulongT siz2 = siz>>1;
        cimg_for(buf,ptrs,Tfloat) {
          const double val = (double)*ptrs; (*ptrs)=(Tfloat)((*ptrs)*val); average+=val;
        }
        buf.sort();
        double a = 0;
        const Tfloat *ptrs = buf._data;
        for (ulongT j = 0; j<siz2; ++j) a+=(double)*(ptrs++);
        const double sig = (double)(2.6477*std::sqrt(a/siz2));
        variance = sig*sig;
      }
      }
      mean = (t)(average/siz);
      return variance>0?variance:0;