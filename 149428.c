gst_h264_parser_parse_sei_unhandled_payload (GstH264NalParser * parser,
    GstH264SEIUnhandledPayload * payload, NalReader * nr, guint payload_type,
    guint payload_size)
{
  guint8 *data = NULL;
  gint i;

  payload->payloadType = payload_type;

  data = g_malloc0 (payload_size);
  for (i = 0; i < payload_size; ++i) {
    READ_UINT8 (nr, data[i], 8);
  }

  payload->size = payload_size;
  payload->data = data;

  return GST_H264_PARSER_OK;

error:
  GST_WARNING ("error parsing \"Unhandled payload\"");
  g_free (data);

  return GST_H264_PARSER_ERROR;
}