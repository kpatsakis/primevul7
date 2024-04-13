gst_h264_parse_nalu_header (GstH264NalUnit * nalu)
{
  guint8 *data = nalu->data + nalu->offset;
  guint8 svc_extension_flag;
  GstBitReader br;

  if (nalu->size < 1)
    return FALSE;

  nalu->type = (data[0] & 0x1f);
  nalu->ref_idc = (data[0] & 0x60) >> 5;
  nalu->idr_pic_flag = (nalu->type == 5 ? 1 : 0);
  nalu->header_bytes = 1;

  nalu->extension_type = GST_H264_NAL_EXTENSION_NONE;

  switch (nalu->type) {
    case GST_H264_NAL_PREFIX_UNIT:
    case GST_H264_NAL_SLICE_EXT:
      if (nalu->size < 4)
        return FALSE;
      gst_bit_reader_init (&br, nalu->data + nalu->offset + nalu->header_bytes,
          nalu->size - nalu->header_bytes);

      svc_extension_flag = gst_bit_reader_get_bits_uint8_unchecked (&br, 1);
      if (svc_extension_flag) { /* SVC */

        nalu->extension_type = GST_H264_NAL_EXTENSION_SVC;

      } else {                  /* MVC */
        GstH264NalUnitExtensionMVC *const mvc = &nalu->extension.mvc;

        nalu->extension_type = GST_H264_NAL_EXTENSION_MVC;
        mvc->non_idr_flag = gst_bit_reader_get_bits_uint8_unchecked (&br, 1);
        mvc->priority_id = gst_bit_reader_get_bits_uint8_unchecked (&br, 6);
        mvc->view_id = gst_bit_reader_get_bits_uint16_unchecked (&br, 10);
        mvc->temporal_id = gst_bit_reader_get_bits_uint8_unchecked (&br, 3);
        mvc->anchor_pic_flag = gst_bit_reader_get_bits_uint8_unchecked (&br, 1);
        mvc->inter_view_flag = gst_bit_reader_get_bits_uint8_unchecked (&br, 1);

        /* Update IdrPicFlag (H.7.4.1.1) */
        nalu->idr_pic_flag = !mvc->non_idr_flag;
      }
      nalu->header_bytes += 3;
      break;
    default:
      break;
  }

  GST_DEBUG ("Nal type %u, ref_idc %u", nalu->type, nalu->ref_idc);
  return TRUE;
}