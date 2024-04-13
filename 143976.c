    **/
    CImg<T>& select(CImgDisplay &disp,
		    const unsigned int feature_type=2, unsigned int *const XYZ=0,
                    const bool exit_on_anykey=false,
                    const bool is_deep_selection_default=false) {
      return get_select(disp,feature_type,XYZ,exit_on_anykey,is_deep_selection_default).move_to(*this);