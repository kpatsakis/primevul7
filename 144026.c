    template<typename tf, typename tc>
    CImg<floatT> get_projections3d(CImgList<tf>& primitives, CImgList<tc>& colors,
                                   const unsigned int x0, const unsigned int y0, const unsigned int z0,
                                   const bool normalize_colors=false) const {
      float m = 0, M = 0, delta = 1;
      if (normalize_colors) { m = (float)min_max(M); delta = 255/(m==M?1:M-m); }
      const unsigned int
        _x0 = (x0>=_width)?_width - 1:x0,
        _y0 = (y0>=_height)?_height - 1:y0,
        _z0 = (z0>=_depth)?_depth - 1:z0;
      CImg<tc> img_xy, img_xz, img_yz;
      if (normalize_colors) {
        ((get_crop(0,0,_z0,0,_width - 1,_height - 1,_z0,_spectrum - 1)-=m)*=delta).move_to(img_xy);
        ((get_crop(0,_y0,0,0,_width - 1,_y0,_depth - 1,_spectrum - 1)-=m)*=delta).resize(_width,_depth,1,-100,-1).
          move_to(img_xz);
        ((get_crop(_x0,0,0,0,_x0,_height - 1,_depth - 1,_spectrum - 1)-=m)*=delta).resize(_height,_depth,1,-100,-1).
          move_to(img_yz);
      } else {
        get_crop(0,0,_z0,0,_width - 1,_height - 1,_z0,_spectrum - 1).move_to(img_xy);
        get_crop(0,_y0,0,0,_width - 1,_y0,_depth - 1,_spectrum - 1).resize(_width,_depth,1,-100,-1).move_to(img_xz);
        get_crop(_x0,0,0,0,_x0,_height - 1,_depth - 1,_spectrum - 1).resize(_height,_depth,1,-100,-1).move_to(img_yz);
      }
      CImg<floatT> points(12,3,1,1,
                          0,_width - 1,_width - 1,0,   0,_width - 1,_width - 1,0, _x0,_x0,_x0,_x0,
                          0,0,_height - 1,_height - 1, _y0,_y0,_y0,_y0,       0,_height - 1,_height - 1,0,
                          _z0,_z0,_z0,_z0,         0,0,_depth - 1,_depth - 1, 0,0,_depth - 1,_depth - 1);
      primitives.assign();
      CImg<tf>::vector(0,1,2,3,0,0,img_xy._width - 1,0,img_xy._width - 1,img_xy._height - 1,0,img_xy._height - 1).
        move_to(primitives);
      CImg<tf>::vector(4,5,6,7,0,0,img_xz._width - 1,0,img_xz._width - 1,img_xz._height - 1,0,img_xz._height - 1).
        move_to(primitives);
      CImg<tf>::vector(8,9,10,11,0,0,img_yz._width - 1,0,img_yz._width - 1,img_yz._height - 1,0,img_yz._height - 1).
        move_to(primitives);
      colors.assign();
      img_xy.move_to(colors);
      img_xz.move_to(colors);
      img_yz.move_to(colors);
      return points;