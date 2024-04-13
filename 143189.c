    template<typename t>
    double PSNR(const CImg<t>& img, const double max_value=255) const {
      const double vMSE = (double)std::sqrt(MSE(img));
      return (vMSE!=0)?(double)(20*std::log10(max_value/vMSE)):(double)(cimg::type<double>::max());