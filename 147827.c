FLAC_API FLAC__bool FLAC__stream_decoder_skip_single_frame(FLAC__StreamDecoder *decoder)
{
	FLAC__bool got_a_frame;
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->protected_);

	while(1) {
		switch(decoder->protected_->state) {
			case FLAC__STREAM_DECODER_SEARCH_FOR_METADATA:
			case FLAC__STREAM_DECODER_READ_METADATA:
				return false; /* above function sets the status for us */
			case FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC:
				if(!frame_sync_(decoder))
					return true; /* above function sets the status for us */
				break;
			case FLAC__STREAM_DECODER_READ_FRAME:
				if(!read_frame_(decoder, &got_a_frame, /*do_full_decode=*/false))
					return false; /* above function sets the status for us */
				if(got_a_frame)
					return true; /* above function sets the status for us */
				break;
			case FLAC__STREAM_DECODER_END_OF_STREAM:
			case FLAC__STREAM_DECODER_ABORTED:
				return true;
			default:
				FLAC__ASSERT(0);
				return false;
		}
	}
}