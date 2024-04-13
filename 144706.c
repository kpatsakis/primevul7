    template<typename t>
    double MSE(const CImg<t>& img) const {
      if (img.size()!=size())
        throw CImgArgumentException(_cimg_instance
                                    "MSE(): Instance and specified image (%u,%u,%u,%u,%p) have different dimensions.",
                                    cimg_instance,
                                    img._width,img._height,img._depth,img._spectrum,img._data);
      double vMSE = 0;
      const t* ptr2 = img._data;
      cimg_for(*this,ptr1,T) {
        const double diff = (double)*ptr1 - (double)*(ptr2++);
        vMSE+=diff*diff;
      }
      const ulongT siz = img.size();
      if (siz) vMSE/=siz;
      return vMSE;