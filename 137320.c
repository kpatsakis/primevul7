static void exif_iif_add_tag(image_info_type *image_info, int section_index, char *name, int tag, int format, size_t length, void* value, size_t value_len)
{
	exif_iif_add_value(image_info, section_index, name, tag, format, (int)length, value, value_len, image_info->motorola_intel);
}