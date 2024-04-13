cvt_flip(int type, int flip)
{
	if (flip == 0)
		return type;
	switch (type) {
	case FILE_BESHORT:
		return FILE_LESHORT;
	case FILE_BELONG:
		return FILE_LELONG;
	case FILE_BEDATE:
		return FILE_LEDATE;
	case FILE_BELDATE:
		return FILE_LELDATE;
	case FILE_BEQUAD:
		return FILE_LEQUAD;
	case FILE_BEQDATE:
		return FILE_LEQDATE;
	case FILE_BEQLDATE:
		return FILE_LEQLDATE;
	case FILE_BEQWDATE:
		return FILE_LEQWDATE;
	case FILE_LESHORT:
		return FILE_BESHORT;
	case FILE_LELONG:
		return FILE_BELONG;
	case FILE_LEDATE:
		return FILE_BEDATE;
	case FILE_LELDATE:
		return FILE_BELDATE;
	case FILE_LEQUAD:
		return FILE_BEQUAD;
	case FILE_LEQDATE:
		return FILE_BEQDATE;
	case FILE_LEQLDATE:
		return FILE_BEQLDATE;
	case FILE_LEQWDATE:
		return FILE_BEQWDATE;
	case FILE_BEFLOAT:
		return FILE_LEFLOAT;
	case FILE_LEFLOAT:
		return FILE_BEFLOAT;
	case FILE_BEDOUBLE:
		return FILE_LEDOUBLE;
	case FILE_LEDOUBLE:
		return FILE_BEDOUBLE;
	default:
		return type;
	}
}