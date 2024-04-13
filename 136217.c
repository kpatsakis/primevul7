static void OneLine24(struct ico_progressive_state *context)
{
	gint X;
	guchar *Pixels;

	X = 0;
	if (context->Header.Negative == 0)
		Pixels = (context->pixbuf->pixels +
			  (gsize) context->pixbuf->rowstride *
			  (context->Header.height - context->Lines - 1));
	else
		Pixels = (context->pixbuf->pixels +
			  (gsize) context->pixbuf->rowstride *
			  context->Lines);
	while (X < context->Header.width) {
		Pixels[X * 4 + 0] = context->LineBuf[X * 3 + 2];
		Pixels[X * 4 + 1] = context->LineBuf[X * 3 + 1];
		Pixels[X * 4 + 2] = context->LineBuf[X * 3 + 0];
		X++;
	}

}