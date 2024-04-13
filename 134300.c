	std::string gzip_error_category::message(int ev) const BOOST_SYSTEM_NOEXCEPT
	{
		static char const* msgs[] =
		{
			"no error",
			"invalid gzip header",
			"inflated data too large",
			"available inflate data did not terminate",
			"output space exhausted before completing inflate",
			"invalid block type (type == 3)",
			"stored block length did not match one's complement",
			"dynamic block code description: too many length or distance codes",
			"dynamic block code description: code lengths codes incomplete",
			"dynamic block code description: repeat lengths with no first length",
			"dynamic block code description: repeat more than specified lengths",
			"dynamic block code description: invalid literal/length code lengths",
			"dynamic block code description: invalid distance code lengths",
			"invalid literal/length or distance code in fixed or dynamic block",
			"distance is too far back in fixed or dynamic block",
			"unknown gzip error",
		};
		if (ev < 0 || ev >= int(sizeof(msgs)/sizeof(msgs[0])))
			return "Unknown error";
		return msgs[ev];
	}