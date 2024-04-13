	boost::system::error_category& get_gzip_category()
	{
		static gzip_error_category gzip_category;
		return gzip_category;
	}