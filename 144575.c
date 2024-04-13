    **/
    static const CImg<Tuchar>& jet_LUT256() {
      static CImg<Tuchar> colormap;
      cimg::mutex(8);
      if (!colormap) {
        colormap.assign(1,4,1,3,(T)0);
        colormap[2] = colormap[3] = colormap[5] = colormap[6] = colormap[8] = colormap[9] = 255;
        colormap.resize(1,256,1,3,3);
      }
      cimg::mutex(8,0);
      return colormap;