static void OneLine8(struct ico_progressive_state *context)
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
		/* The joys of having a BGR byteorder */
		Pixels[X * 4 + 0] =
		    context->HeaderBuf[4 * context->LineBuf[X] + 42+context->DIBoffset];
		Pixels[X * 4 + 1] =
		    context->HeaderBuf[4 * context->LineBuf[X] + 41+context->DIBoffset];
		Pixels[X * 4 + 2] =
		    context->HeaderBuf[4 * context->LineBuf[X] + 40+context->DIBoffset];
		X++;
	}
}