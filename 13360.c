R_API int extract_type_value(const char *arg_str, char **output) {
	ut8 found_one = 0, array_cnt = 0;
	ut32 len = 0, consumed = 0;
	char *str = NULL;
	if (!arg_str || !output) {
		return 0;
	}
	if (output && *output && *output != NULL) {
		R_FREE (*output);
	}
	while (arg_str && *arg_str && !found_one) {
		len = 1;
		// handle the end of an object
		switch (*arg_str) {
		case 'V':
			str = get_type_value_str ("void", array_cnt);
			break;
		case 'J':
			str = get_type_value_str ("long", array_cnt);
			array_cnt = 0;
			break;
		case 'I':
			str = get_type_value_str ("int", array_cnt);
			array_cnt = 0;
			break;
		case 'D':
			str = get_type_value_str ("double", array_cnt);
			array_cnt = 0;
			break;
		case 'F':
			str = get_type_value_str ("float", array_cnt);
			array_cnt = 0;
			break;
		case 'B':
			str = get_type_value_str ("byte", array_cnt);
			array_cnt = 0;
			break;
		case 'C':
			str = get_type_value_str ("char", array_cnt);
			array_cnt = 0;
			break;
		case 'Z':
			str = get_type_value_str ("boolean", array_cnt);
			array_cnt = 0;
			break;
		case 'S':
			str = get_type_value_str ("short", array_cnt);
			array_cnt = 0;
			break;
		case '[':
			array_cnt++;
			break;
		case 'L':
			len = r_bin_java_extract_reference_name (arg_str, &str, array_cnt);
			array_cnt = 0;
			break;
		case '(':
			str = strdup ("(");
			break;
		case ')':
			str = strdup (")");
			break;
		default:
			return 0;
		}
		if (len < 1) {
			break;
		}
		consumed += len;
		arg_str += len;
		if (str) {
			*output = str;
			break;
		}
	}
	return consumed;
}