    template<typename tf>
    static CImg<floatT> box3d(CImgList<tf>& primitives,
                              const float size_x=200, const float size_y=100, const float size_z=100) {
      primitives.assign(6,1,4,1,1, 0,3,2,1, 4,5,6,7, 0,1,5,4, 3,7,6,2, 0,4,7,3, 1,2,6,5);
      return CImg<floatT>(8,3,1,1,
                          0.,size_x,size_x,    0.,    0.,size_x,size_x,    0.,
                          0.,    0.,size_y,size_y,    0.,    0.,size_y,size_y,
                          0.,    0.,    0.,    0.,size_z,size_z,size_z,size_z);