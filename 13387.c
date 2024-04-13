R_API char *r_bin_java_print_utf8_cp_stringify(RBinJavaCPTypeObj *obj) {
	char *utf8_str = r_hex_bin2strdup (obj->info.cp_utf8.bytes, obj->info.cp_utf8.length);
	char *res = r_str_newf ("%d.0x%04"PFMT64x ".%s.%d.%s",
			obj->metas->ord, obj->file_offset + obj->loadaddr, ((RBinJavaCPTypeMetas *) obj->metas->type_info)->name,
			obj->info.cp_utf8.length, utf8_str);
	free (utf8_str);
	return res;
}