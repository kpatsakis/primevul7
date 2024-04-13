    //! Wait for any event occuring either on the display \c disp1, \c disp2 or \c disp3.
    static void wait(CImgDisplay& disp1, CImgDisplay& disp2, CImgDisplay& disp3) {
      disp1._is_event = disp2._is_event = disp3._is_event = false;
      while ((!disp1._is_closed || !disp2._is_closed || !disp3._is_closed) &&
             !disp1._is_event && !disp2._is_event && !disp3._is_event) wait_all();