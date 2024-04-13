    //! Compute Haar multiscale wavelet transform \newinstance.
    CImg<Tfloat> get_haar(const bool invert=false, const unsigned int nb_scales=1) const {
      CImg<Tfloat> res;
      if (nb_scales==1) { // Single scale transform
        if (_width>1) get_haar('x',invert,1).move_to(res);
        if (_height>1) { if (res) res.haar('y',invert,1); else get_haar('y',invert,1).move_to(res); }
        if (_depth>1) { if (res) res.haar('z',invert,1); else get_haar('z',invert,1).move_to(res); }
        if (res) return res;
      } else { // Multi-scale transform
        if (invert) { // Inverse transform
          res.assign(*this,false);
          if (_width>1) {
            if (_height>1) {
              if (_depth>1) {
                unsigned int w = _width, h = _height, d = _depth;
                for (unsigned int s = 1; w && h && d && s<nb_scales; ++s) { w/=2; h/=2; d/=2; }
                for (w = w?w:1, h = h?h:1, d = d?d:1; w<=_width && h<=_height && d<=_depth; w*=2, h*=2, d*=2)
                  res.draw_image(res.get_crop(0,0,0,w - 1,h - 1,d - 1).get_haar(true,1));
              } else {
                unsigned int w = _width, h = _height;
                for (unsigned int s = 1; w && h && s<nb_scales; ++s) { w/=2; h/=2; }
                for (w = w?w:1, h = h?h:1; w<=_width && h<=_height; w*=2, h*=2)
                  res.draw_image(res.get_crop(0,0,0,w - 1,h - 1,0).get_haar(true,1));
              }
            } else {
              if (_depth>1) {
                unsigned int w = _width, d = _depth;
                for (unsigned int s = 1; w && d && s<nb_scales; ++s) { w/=2; d/=2; }
                for (w = w?w:1, d = d?d:1; w<=_width && d<=_depth; w*=2, d*=2)
                  res.draw_image(res.get_crop(0,0,0,w - 1,0,d - 1).get_haar(true,1));
              } else {
                unsigned int w = _width;
                for (unsigned int s = 1; w && s<nb_scales; ++s) w/=2;
                for (w = w?w:1; w<=_width; w*=2)
                  res.draw_image(res.get_crop(0,0,0,w - 1,0,0).get_haar(true,1));
              }
            }
          } else {
            if (_height>1) {
              if (_depth>1) {
                unsigned int h = _height, d = _depth;
                for (unsigned int s = 1; h && d && s<nb_scales; ++s) { h/=2; d/=2; }
                for (h = h?h:1, d = d?d:1; h<=_height && d<=_depth; h*=2, d*=2)
                  res.draw_image(res.get_crop(0,0,0,0,h - 1,d - 1).get_haar(true,1));
              } else {
                unsigned int h = _height;
                for (unsigned int s = 1; h && s<nb_scales; ++s) h/=2;
                for (h = h?h:1; h<=_height; h*=2)
                  res.draw_image(res.get_crop(0,0,0,0,h - 1,0).get_haar(true,1));
              }
            } else {
              if (_depth>1) {
                unsigned int d = _depth;
                for (unsigned int s = 1; d && s<nb_scales; ++s) d/=2;
                for (d = d?d:1; d<=_depth; d*=2)
                  res.draw_image(res.get_crop(0,0,0,0,0,d - 1).get_haar(true,1));
              } else return *this;
            }
          }
        } else { // Direct transform
          res = get_haar(false,1);
          if (_width>1) {
            if (_height>1) {
              if (_depth>1)
                for (unsigned int s = 1, w = _width/2, h = _height/2, d = _depth/2; w && h && d && s<nb_scales;
                     ++s, w/=2, h/=2, d/=2)
                  res.draw_image(res.get_crop(0,0,0,w - 1,h - 1,d - 1).haar(false,1));
              else for (unsigned int s = 1, w = _width/2, h = _height/2; w && h && s<nb_scales; ++s, w/=2, h/=2)
                     res.draw_image(res.get_crop(0,0,0,w - 1,h - 1,0).haar(false,1));
            } else {
              if (_depth>1) for (unsigned int s = 1, w = _width/2, d = _depth/2; w && d && s<nb_scales; ++s, w/=2, d/=2)
                              res.draw_image(res.get_crop(0,0,0,w - 1,0,d - 1).haar(false,1));
              else for (unsigned int s = 1, w = _width/2; w && s<nb_scales; ++s, w/=2)
                     res.draw_image(res.get_crop(0,0,0,w - 1,0,0).haar(false,1));
            }
          } else {
            if (_height>1) {
              if (_depth>1)
                for (unsigned int s = 1, h = _height/2, d = _depth/2; h && d && s<nb_scales; ++s, h/=2, d/=2)
                  res.draw_image(res.get_crop(0,0,0,0,h - 1,d - 1).haar(false,1));
              else for (unsigned int s = 1, h = _height/2; h && s<nb_scales; ++s, h/=2)
                     res.draw_image(res.get_crop(0,0,0,0,h - 1,0).haar(false,1));
            } else {
              if (_depth>1) for (unsigned int s = 1, d = _depth/2; d && s<nb_scales; ++s, d/=2)
                              res.draw_image(res.get_crop(0,0,0,0,0,d - 1).haar(false,1));
              else return *this;
            }
          }
        }
        return res;
      }
      return *this;