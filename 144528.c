    **/
    const CImg<T>& display(CImgDisplay &disp, const bool display_info, unsigned int *const XYZ=0,
                           const bool exit_on_anykey=false) const {
      return _display(disp,0,display_info,XYZ,exit_on_anykey,false);