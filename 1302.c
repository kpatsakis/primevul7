static void XDrawMatte(Display *display,const XWindowInfo *window_info,
  const XWidgetInfo *matte_info)
{
  /*
    Draw matte.
  */
  if ((matte_info->trough == MagickFalse) || (window_info->depth == 1))
    (void) XFillRectangle(display,window_info->id,
      window_info->highlight_context,matte_info->x,matte_info->y,
      matte_info->width,matte_info->height);
  else
    {
      (void) XSetForeground(display,window_info->widget_context,
        window_info->pixel_info->trough_color.pixel);
      (void) XFillRectangle(display,window_info->id,window_info->widget_context,
        matte_info->x,matte_info->y,matte_info->width,matte_info->height);
    }
}