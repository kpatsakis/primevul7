    **/
    static const CImg<Tuchar>& cube_LUT256() {
      static CImg<Tuchar> colormap;
      cimg::mutex(8);
      if (!colormap) {
        colormap.assign(1,8,1,3,(T)0);
        colormap[1] = colormap[3] = colormap[5] = colormap[7] =
          colormap[10] = colormap[11] = colormap[12] = colormap[13] =
          colormap[20] = colormap[21] = colormap[22] = colormap[23] = 255;
        colormap.resize(1,256,1,3,3);
      }
      cimg::mutex(8,0);
      return colormap;