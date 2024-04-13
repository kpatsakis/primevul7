    template<typename tf>
    static CImg<floatT> torus3d(CImgList<tf>& primitives,
                                const float radius1=100, const float radius2=30,
                                const unsigned int subdivisions1=24, const unsigned int subdivisions2=12) {
      primitives.assign();
      if (!subdivisions1 || !subdivisions2) return CImg<floatT>();
      CImgList<floatT> vertices;
      for (unsigned int v = 0; v<subdivisions1; ++v) {
	const float
	  beta = (float)(v*2*cimg::PI/subdivisions1),
	  xc = radius1*(float)std::cos(beta),
	  yc = radius1*(float)std::sin(beta);
        for (unsigned int u = 0; u<subdivisions2; ++u) {
          const float
            alpha = (float)(u*2*cimg::PI/subdivisions2),
            x = xc + radius2*(float)(std::cos(alpha)*std::cos(beta)),
	    y = yc + radius2*(float)(std::cos(alpha)*std::sin(beta)),
	    z = radius2*(float)std::sin(alpha);
          CImg<floatT>::vector(x,y,z).move_to(vertices);
        }
      }
      for (unsigned int vv = 0; vv<subdivisions1; ++vv) {
	const unsigned int nv = (vv + 1)%subdivisions1;
        for (unsigned int uu = 0; uu<subdivisions2; ++uu) {
          const unsigned int nu = (uu + 1)%subdivisions2, svv = subdivisions2*vv, snv = subdivisions2*nv;
          CImg<tf>::vector(svv + nu,svv + uu,snv + uu,snv + nu).move_to(primitives);
        }
      }
      return vertices>'x';