static void OneLine(struct ico_progressive_state *context)
{
	context->LineDone = 0;
	
	if (context->Lines >= context->Header.height*2) {
		return;
	}
		
	if (context->Lines <context->Header.height) {		
                if (context->Type == 32)
                        OneLine32 (context);
		else if (context->Type == 24)
			OneLine24(context);
                else if (context->Type == 16)
                        OneLine16 (context);
		else if (context->Type == 8)
			OneLine8(context);
		else if (context->Type == 4)
			OneLine4(context);
		else if (context->Type == 1)
			OneLine1(context);
		else 
			g_assert_not_reached ();
	} else
		OneLineTransp(context);
	
	context->Lines++;
	if (context->Lines>=context->Header.height) {
	 	context->Type = 1;
		context->LineWidth = context->Header.width / 8;
		if ((context->Header.width & 7) != 0)
			context->LineWidth++;
		/* Pad to a 32 bit boundary */
		if (((context->LineWidth % 4) > 0))
			context->LineWidth = (context->LineWidth / 4) * 4 + 4;
			
	}
	  

	if (context->updated_func != NULL) {
		(*context->updated_func) (context->pixbuf,
					  0,
					  context->Lines % context->Header.height,
					  context->Header.width,
					  1,
					  context->user_data);

	}
}