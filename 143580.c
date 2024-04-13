    **/
    CImg<T>& pow(const double p) {
      if (is_empty()) return *this;
      if (p==-4) { cimg_openmp_for(*this,1/(Tfloat)cimg::pow4(*ptr),32768); return *this; }
      if (p==-3) { cimg_openmp_for(*this,1/(Tfloat)cimg::pow3(*ptr),32768); return *this; }
      if (p==-2) { cimg_openmp_for(*this,1/(Tfloat)cimg::sqr(*ptr),32768); return *this; }
      if (p==-1) { cimg_openmp_for(*this,1/(Tfloat)*ptr,32768); return *this; }
      if (p==-0.5) { cimg_openmp_for(*this,1/std::sqrt((Tfloat)*ptr),8192); return *this; }
      if (p==0) return fill((T)1);
      if (p==0.5) return sqrt();
      if (p==1) return *this;
      if (p==2) return sqr();
      if (p==3) { cimg_openmp_for(*this,cimg::pow3(*ptr),262144); return *this; }
      if (p==4) { cimg_openmp_for(*this,cimg::pow4(*ptr),131072); return *this; }
      cimg_openmp_for(*this,std::pow((Tfloat)*ptr,(Tfloat)p),1024);
      return *this;