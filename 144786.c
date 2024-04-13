    //! Wait for any event occuring on the display \c disp1.
    static void wait(CImgDisplay& disp1) {
      disp1._is_event = false;
      while (!disp1._is_closed && !disp1._is_event) wait_all();