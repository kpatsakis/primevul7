          current CImgList instance into it.
        Images of the list are appended in a single temporarly image for visualization purposes.
        The function returns when a key is pressed or the display window is closed by the user.
    **/
    const CImgList<T>& display(CImgDisplay &disp, const bool display_info,
                               const char axis='x', const float align=0,