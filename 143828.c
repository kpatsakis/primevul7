    **/
    CImg<T> get_projections2d(const unsigned int x0, const unsigned int y0, const unsigned int z0) const {
      if (is_empty() || _depth<2) return +*this;
      const unsigned int
        _x0 = (x0>=_width)?_width - 1:x0,
        _y0 = (y0>=_height)?_height - 1:y0,
        _z0 = (z0>=_depth)?_depth - 1:z0;
      const CImg<T>
        img_xy = get_crop(0,0,_z0,0,_width - 1,_height - 1,_z0,_spectrum - 1),
        img_zy = get_crop(_x0,0,0,0,_x0,_height - 1,_depth - 1,_spectrum - 1).permute_axes("xzyc").
        resize(_depth,_height,1,-100,-1),
        img_xz = get_crop(0,_y0,0,0,_width - 1,_y0,_depth - 1,_spectrum - 1).resize(_width,_depth,1,-100,-1);
      return CImg<T>(_width + _depth,_height + _depth,1,_spectrum,cimg::min(img_xy.min(),img_zy.min(),img_xz.min())).
        draw_image(0,0,img_xy).draw_image(img_xy._width,0,img_zy).
        draw_image(0,img_xy._height,img_xz);