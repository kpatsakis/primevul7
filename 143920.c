    **/
    const CImg<T>& display_graph(CImgDisplay &disp,
                                 const unsigned int plot_type=1, const unsigned int vertex_type=1,
                                 const char *const labelx=0, const double xmin=0, const double xmax=0,
                                 const char *const labely=0, const double ymin=0, const double ymax=0,
                                 const bool exit_on_anykey=false) const {
      return _display_graph(disp,0,plot_type,vertex_type,labelx,xmin,xmax,labely,ymin,ymax,exit_on_anykey);