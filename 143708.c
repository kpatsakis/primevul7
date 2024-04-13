    **/
    CImg<Tdouble> get_stats(const unsigned int variance_method=1) const {
      if (is_empty()) return CImg<doubleT>();
      const ulongT siz = size();
      const longT off_end = (longT)siz;
      double S = 0, S2 = 0, P = 1;
      longT offm = 0, offM = 0;
      T m = *_data, M = m;

      cimg_pragma_openmp(parallel reduction(+:S,S2) reduction(*:P) cimg_openmp_if_size(siz,131072)) {
        longT loffm = 0, loffM = 0;
        T lm = *_data, lM = lm;
        cimg_pragma_openmp(for)
        for (longT off = 0; off<off_end; ++off) {
          const T val = _data[off];
          const double _val = (double)val;
          if (val<lm) { lm = val; loffm = off; }
          if (val>lM) { lM = val; loffM = off; }
          S+=_val;
          S2+=_val*_val;
          P*=_val;
        }
        cimg_pragma_openmp(critical(get_stats)) {
          if (lm<m || (lm==m && loffm<offm)) { m = lm; offm = loffm; }
          if (lM>M || (lM==M && loffM<offM)) { M = lM; offM = loffM; }
        }
      }

      const double
        mean_value = S/siz,
        _variance_value = variance_method==0?(S2 - S*S/siz)/siz:
        (variance_method==1?(siz>1?(S2 - S*S/siz)/(siz - 1):0):
         variance(variance_method)),
        variance_value = _variance_value>0?_variance_value:0;
      int
        xm = 0, ym = 0, zm = 0, cm = 0,
        xM = 0, yM = 0, zM = 0, cM = 0;
      contains(_data[offm],xm,ym,zm,cm);
      contains(_data[offM],xM,yM,zM,cM);
      return CImg<Tdouble>(1,14).fill((double)m,(double)M,mean_value,variance_value,
                                      (double)xm,(double)ym,(double)zm,(double)cm,
                                      (double)xM,(double)yM,(double)zM,(double)cM,
                                      S,P);