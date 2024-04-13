    template<typename T>
    explicit CImgDisplay(const CImg<T>& img,
                         const char *const title=0, const unsigned int normalization=3,
                         const bool is_fullscreen=false, const bool is_closed=false):
      _width(0),_height(0),_normalization(0),
      _min(0),_max(0),
      _is_fullscreen(false),
      _title(0),
      _window_width(0),_window_height(0),_button(0),
      _keys(new unsigned int[128]),_released_keys(new unsigned int[128]),
      _window_x(0),_window_y(0),_mouse_x(-1),_mouse_y(-1),_wheel(0),
      _is_closed(true),_is_resized(false),_is_moved(false),_is_event(false) {
      assign(img,title,normalization,is_fullscreen,is_closed);