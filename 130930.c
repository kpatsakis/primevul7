zzip_get_default_ext(void)
{
    static zzip_strings_t ext[] = {
	/* *INDENT-OFF* */
       ".zip", ".ZIP", /* common extension */
#     ifdef ZZIP_USE_ZIPLIKES
       ".pk3", ".PK3", /* ID Software's Quake3 zipfiles */
       ".jar", ".JAR", /* Java zipfiles */
#      endif
	/* *INDENT-ON* */
        0
    };

    return ext;
}