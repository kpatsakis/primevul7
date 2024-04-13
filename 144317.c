    template<typename t, typename tc>
    CImg<T>& draw_ellipse(const int x0, const int y0, const CImg<t> &tensor,
                          const tc *const color, const float opacity,
                          const unsigned int pattern) {
      CImgList<t> eig = tensor.get_symmetric_eigen();
      const CImg<t> &val = eig[0], &vec = eig[1];
      return draw_ellipse(x0,y0,std::sqrt(val(0)),std::sqrt(val(1)),
                          std::atan2(vec(0,1),vec(0,0))*180/cimg::PI,
                          color,opacity,pattern);