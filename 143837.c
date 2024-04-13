    template<typename to>
    float* _object3dtoCImg3d(const CImg<to>& opacities, float *ptrd) const {
      const to *ptro = opacities._data;
      cimg_foroff(opacities,o) *(ptrd++) = (float)*(ptro++);
      return ptrd;