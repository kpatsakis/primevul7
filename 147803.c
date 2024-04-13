FLAC__StreamDecoderWriteStatus write_audio_frame_to_client_(FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 * const buffer[])
{
	if(decoder->private_->is_seeking) {
		FLAC__uint64 this_frame_sample = frame->header.number.sample_number;
		FLAC__uint64 next_frame_sample = this_frame_sample + (FLAC__uint64)frame->header.blocksize;
		FLAC__uint64 target_sample = decoder->private_->target_sample;

		FLAC__ASSERT(frame->header.number_type == FLAC__FRAME_NUMBER_TYPE_SAMPLE_NUMBER);

#if FLAC__HAS_OGG
		decoder->private_->got_a_frame = true;
#endif
		decoder->private_->last_frame = *frame; /* save the frame */
		if(this_frame_sample <= target_sample && target_sample < next_frame_sample) { /* we hit our target frame */
			unsigned delta = (unsigned)(target_sample - this_frame_sample);
			/* kick out of seek mode */
			decoder->private_->is_seeking = false;
			/* shift out the samples before target_sample */
			if(delta > 0) {
				unsigned channel;
				const FLAC__int32 *newbuffer[FLAC__MAX_CHANNELS];
				for(channel = 0; channel < frame->header.channels; channel++)
					newbuffer[channel] = buffer[channel] + delta;
				decoder->private_->last_frame.header.blocksize -= delta;
				decoder->private_->last_frame.header.number.sample_number += (FLAC__uint64)delta;
				/* write the relevant samples */
				return decoder->private_->write_callback(decoder, &decoder->private_->last_frame, newbuffer, decoder->private_->client_data);
			}
			else {
				/* write the relevant samples */
				return decoder->private_->write_callback(decoder, frame, buffer, decoder->private_->client_data);
			}
		}
		else {
			return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
		}
	}
	else {
		/*
		 * If we never got STREAMINFO, turn off MD5 checking to save
		 * cycles since we don't have a sum to compare to anyway
		 */
		if(!decoder->private_->has_stream_info)
			decoder->private_->do_md5_checking = false;
		if(decoder->private_->do_md5_checking) {
			if(!FLAC__MD5Accumulate(&decoder->private_->md5context, buffer, frame->header.channels, frame->header.blocksize, (frame->header.bits_per_sample+7) / 8))
				return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
		}
		return decoder->private_->write_callback(decoder, frame, buffer, decoder->private_->client_data);
	}
}