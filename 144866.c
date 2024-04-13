    **/
    static const CImg<Tuchar>& default_LUT256() {
      static CImg<Tuchar> colormap;
      cimg::mutex(8);
      if (!colormap) {
        colormap.assign(1,256,1,3);
        for (unsigned int index = 0, r = 16; r<256; r+=32)
          for (unsigned int g = 16; g<256; g+=32)
            for (unsigned int b = 32; b<256; b+=64) {
              colormap(0,index,0) = (Tuchar)r;
              colormap(0,index,1) = (Tuchar)g;
              colormap(0,index++,2) = (Tuchar)b;
            }
      }
      cimg::mutex(8,0);
      return colormap;