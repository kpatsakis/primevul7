    **/
    CImg<T>& blur_box(const float boxsize, const bool boundary_conditions=true) {
      const float nboxsize = boxsize>=0?boxsize:-boxsize*cimg::max(_width,_height,_depth)/100;
      return blur_box(nboxsize,nboxsize,nboxsize,boundary_conditions);