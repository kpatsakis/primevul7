R_API RBinJavaAttrInfo *r_bin_java_line_number_table_attr_new(RBinJavaObj *bin, ut8 *buffer, ut64 sz, ut64 buf_offset) {
	ut32 i = 0;
	ut64 curpos, offset = 0;
	if (sz < 6) {
		return NULL;
	}
	RBinJavaAttrInfo *attr = r_bin_java_default_attr_new (bin, buffer, sz, buf_offset);
	if (!attr) {
		return NULL;
	}
	offset += 6;
	attr->type = R_BIN_JAVA_ATTR_TYPE_LINE_NUMBER_TABLE_ATTR;
	attr->info.line_number_table_attr.line_number_table_length = R_BIN_JAVA_USHORT (buffer, offset);
	offset += 2;
	attr->info.line_number_table_attr.line_number_table = r_list_newf (free);

	ut32 linenum_len = attr->info.line_number_table_attr.line_number_table_length;
	RList *linenum_list = attr->info.line_number_table_attr.line_number_table;
	for (i = 0; i < linenum_len; i++) {
		curpos = buf_offset + offset;
		// eprintf ("%"PFMT64x" %"PFMT64x"\n", curpos, sz);
		// XXX if (curpos + 8 >= sz) break;
		RBinJavaLineNumberAttribute *lnattr = R_NEW0 (RBinJavaLineNumberAttribute);
		if (!lnattr) {
			break;
		}
		// wtf it works
		if (offset - 2 > sz) {
			R_FREE (lnattr);
			break;
		}
		lnattr->start_pc = R_BIN_JAVA_USHORT (buffer, offset);
		offset += 2;
		lnattr->line_number = R_BIN_JAVA_USHORT (buffer, offset);
		offset += 2;
		lnattr->file_offset = curpos;
		lnattr->size = 4;
		r_list_append (linenum_list, lnattr);
	}
	attr->size = offset;
	return attr;
}