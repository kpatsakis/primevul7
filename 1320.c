static void XDrawWidgetText(Display *display,const XWindowInfo *window_info,
  XWidgetInfo *text_info)
{
  GC
    widget_context;

  int
    x,
    y;

  unsigned int
    height,
    width;

  XFontStruct
    *font_info;

  XRectangle
    crop_info;

  /*
    Clear the text area.
  */
  widget_context=window_info->annotate_context;
  if (text_info->raised)
    (void) XClearArea(display,window_info->id,text_info->x,text_info->y,
      text_info->width,text_info->height,MagickFalse);
  else
    {
      (void) XFillRectangle(display,window_info->id,widget_context,text_info->x,
        text_info->y,text_info->width,text_info->height);
      widget_context=window_info->highlight_context;
    }
  if (text_info->text == (char *) NULL)
    return;
  if (*text_info->text == '\0')
    return;
  /*
    Set cropping region.
  */
  font_info=window_info->font_info;
  crop_info.width=(unsigned short) text_info->width;
  crop_info.height=(unsigned short) text_info->height;
  crop_info.x=text_info->x;
  crop_info.y=text_info->y;
  /*
    Draw text.
  */
  width=WidgetTextWidth(font_info,text_info->text);
  x=text_info->x+(QuantumMargin >> 1);
  if (text_info->center)
    x=text_info->x+(text_info->width >> 1)-(width >> 1);
  if (text_info->raised)
    if (width > (text_info->width-QuantumMargin))
      x+=(text_info->width-QuantumMargin-width);
  height=(unsigned int) (font_info->ascent+font_info->descent);
  y=text_info->y+((text_info->height-height) >> 1)+font_info->ascent;
  (void) XSetClipRectangles(display,widget_context,0,0,&crop_info,1,Unsorted);
  (void) XDrawString(display,window_info->id,widget_context,x,y,text_info->text,
    Extent(text_info->text));
  (void) XSetClipMask(display,widget_context,None);
  if (x < text_info->x)
    (void) XDrawLine(display,window_info->id,window_info->annotate_context,
      text_info->x,text_info->y,text_info->x,text_info->y+text_info->height-1);
}