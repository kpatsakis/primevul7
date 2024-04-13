    **/
    static const CImg<Tuchar>& flag_LUT256() {
      static CImg<Tuchar> colormap;
      cimg::mutex(8);
      if (!colormap) {
        colormap.assign(1,4,1,3,(T)0);
        colormap[0] = colormap[1] = colormap[5] = colormap[9] = colormap[10] = 255;
        colormap.resize(1,256,1,3,0,2);
      }
      cimg::mutex(8,0);
      return colormap;