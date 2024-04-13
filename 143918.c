    template<typename tp, typename tf, typename tc, typename to>
    const CImg<T>& display_object3d(const char *const title,
                                    const CImg<tp>& vertices,
                                    const CImgList<tf>& primitives,
                                    const CImgList<tc>& colors,
                                    const to& opacities,
                                    const bool centering=true,
                                    const int render_static=4, const int render_motion=1,
                                    const bool is_double_sided=true, const float focale=700,
                                    const float light_x=0, const float light_y=0, const float light_z=-5e8f,
                                    const float specular_lightness=0.2f, const float specular_shininess=0.1f,
                                    const bool display_axes=true, float *const pose_matrix=0,
                                    const bool exit_on_anykey=false) const {
      CImgDisplay disp;
      return _display_object3d(disp,title,vertices,primitives,colors,opacities,centering,render_static,
			       render_motion,is_double_sided,focale,
                               light_x,light_y,light_z,specular_lightness,specular_shininess,
			       display_axes,pose_matrix,exit_on_anykey);