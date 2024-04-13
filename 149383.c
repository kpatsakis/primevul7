gst_h264_sei_clear (GstH264SEIMessage * sei)
{
  switch (sei->payloadType) {
    case GST_H264_SEI_REGISTERED_USER_DATA:{
      GstH264RegisteredUserData *rud = &sei->payload.registered_user_data;

      g_free ((guint8 *) rud->data);
      rud->data = NULL;
      break;
    }
    case GST_H264_SEI_UNHANDLED_PAYLOAD:{
      GstH264SEIUnhandledPayload *payload = &sei->payload.unhandled_payload;

      g_free (payload->data);
      payload->data = NULL;
      payload->size = 0;
      break;
    }
    default:
      break;
  }
}