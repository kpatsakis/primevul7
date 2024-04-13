
    CImgDisplay& _assign(const unsigned int dimw, const unsigned int dimh, const char *const ptitle=0,
                         const unsigned int normalization_type=3,
                         const bool fullscreen_flag=false, const bool closed_flag=false) {

      // Allocate space for window title
      const char *const nptitle = ptitle?ptitle:"";
      const unsigned int s = (unsigned int)std::strlen(nptitle) + 1;
      char *const tmp_title = s?new char[s]:0;
      if (s) std::memcpy(tmp_title,nptitle,s*sizeof(char));

      // Destroy previous window if existing
      if (!is_empty()) assign();

      // Set display variables
      _width = std::min(dimw,(unsigned int)screen_width());
      _height = std::min(dimh,(unsigned int)screen_height());
      _normalization = normalization_type<4?normalization_type:3;
      _is_fullscreen = fullscreen_flag;
      _window_x = _window_y = 0;
      _is_closed = closed_flag;
      _is_cursor_visible = true;
      _is_mouse_tracked = false;
      _title = tmp_title;
      flush();
      if (_is_fullscreen) _init_fullscreen();

      // Create event thread
      void *const arg = (void*)(new void*[2]);
      ((void**)arg)[0] = (void*)this;
      ((void**)arg)[1] = (void*)_title;
      _mutex = CreateMutex(0,FALSE,0);
      _is_created = CreateEvent(0,FALSE,FALSE,0);
      _thread = CreateThread(0,0,_events_thread,arg,0,0);
      WaitForSingleObject(_is_created,INFINITE);
      return *this;