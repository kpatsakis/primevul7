    **/
    const CImg<T>& display(const char *const title=0, const bool display_info=true, unsigned int *const XYZ=0,
                           const bool exit_on_anykey=false) const {
      CImgDisplay disp;
      return _display(disp,title,display_info,XYZ,exit_on_anykey,false);