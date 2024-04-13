static bool char_must_be_escaped(ut8 c) {
	switch (c) {
	case 0x7f:
	case 0x81:
	case 0x8f:
	case 0x90:
	case 0x9d:
	case 0xa0:
	case 0xad:
		return true;
	default:
		if (c < 0x20) {
			return true;
		}
		return false;
	}
}