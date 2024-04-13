gst_h264_parser_parse_registered_user_data (GstH264NalParser * nalparser,
    GstH264RegisteredUserData * rud, NalReader * nr, guint payload_size)
{
  guint8 *data = NULL;
  guint i;

  rud->data = NULL;
  rud->size = 0;

  if (payload_size < 2) {
    GST_WARNING ("Too small payload size %d", payload_size);
    return GST_H264_PARSER_BROKEN_DATA;
  }

  READ_UINT8 (nr, rud->country_code, 8);
  --payload_size;

  if (rud->country_code == 0xFF) {
    READ_UINT8 (nr, rud->country_code_extension, 8);
    --payload_size;
  } else {
    rud->country_code_extension = 0;
  }

  if (payload_size < 1) {
    GST_WARNING ("No more remaining payload data to store");
    return GST_H264_PARSER_BROKEN_DATA;
  }

  data = g_malloc (payload_size);
  for (i = 0; i < payload_size; ++i) {
    READ_UINT8 (nr, data[i], 8);
  }

  GST_MEMDUMP ("SEI user data", data, payload_size);

  rud->data = data;
  rud->size = payload_size;
  return GST_H264_PARSER_OK;

error:
  {
    GST_WARNING ("error parsing \"Registered User Data\"");
    g_free (data);
    return GST_H264_PARSER_ERROR;
  }
}