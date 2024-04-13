    **/
    static const CImg<Tuchar>& HSV_LUT256() {
      static CImg<Tuchar> colormap;
      cimg::mutex(8);
      if (!colormap) {
        CImg<Tint> tmp(1,256,1,3,1);
        tmp.get_shared_channel(0).sequence(0,359);
        colormap = tmp.HSVtoRGB();
      }
      cimg::mutex(8,0);
      return colormap;