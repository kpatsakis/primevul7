    //! Wait for any event occuring either on the display \c disp1 or \c disp2.
    static void wait(CImgDisplay& disp1, CImgDisplay& disp2) {
      disp1._is_event = disp2._is_event = false;
      while ((!disp1._is_closed || !disp2._is_closed) &&
             !disp1._is_event && !disp2._is_event) wait_all();