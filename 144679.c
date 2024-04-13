    template<typename t>
    CImg<Tfloat> get_blur_guided(const CImg<t>& guide, const float radius, const float regularization) const {
      if (!is_sameXYZ(guide))
        throw CImgArgumentException(_cimg_instance
                                    "blur_guided(): Invalid size for specified guide image (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    guide._width,guide._height,guide._depth,guide._spectrum,guide._data);
      if (is_empty() || !radius) return *this;
      const int _radius = radius>=0?(int)radius:(int)(-radius*cimg::max(_width,_height,_depth)/100);
      float _regularization = regularization;
      if (regularization<0) {
        T edge_min, edge_max = guide.max_min(edge_min);
        if (edge_min==edge_max) return *this;
        _regularization = -regularization*(edge_max - edge_min)/100;
      }
      _regularization = std::max(_regularization,0.01f);
      const unsigned int psize = (unsigned int)(1 + 2*_radius);
      CImg<Tfloat>
        mean_p = get_blur_box(psize,true),
        mean_I = guide.get_blur_box(psize,true).resize(mean_p),
        cov_Ip = get_mul(guide).blur_box(psize,true)-=mean_p.get_mul(mean_I),
        var_I = guide.get_sqr().blur_box(psize,true)-=mean_I.get_sqr(),
        &a = cov_Ip.div(var_I+=_regularization),
        &b = mean_p-=a.get_mul(mean_I);
      a.blur_box(psize,true);
      b.blur_box(psize,true);
      return a.mul(guide)+=b;