test_bson_macros (void)
{
   const uint8_t data[] = {1, 2, 3, 4};
   bson_t b = BSON_INITIALIZER;
   bson_t ar = BSON_INITIALIZER;
   bson_decimal128_t dec;
   bson_oid_t oid;
   struct timeval tv;
   time_t t;

   dec.high = 0x3040000000000000ULL;
   dec.low = 0x0ULL;

   t = time (NULL);
#ifdef BSON_OS_WIN32
   tv.tv_sec = (long) t;
#else
   tv.tv_sec = t;
#endif
   tv.tv_usec = 0;

   bson_oid_init (&oid, NULL);

   BSON_APPEND_ARRAY (&b, "0", &ar);
   BSON_APPEND_BINARY (&b, "1", 0, data, sizeof data);
   BSON_APPEND_BOOL (&b, "2", true);
   BSON_APPEND_CODE (&b, "3", "function(){}");
   BSON_APPEND_CODE_WITH_SCOPE (&b, "4", "function(){}", &ar);
   BSON_APPEND_DOUBLE (&b, "6", 123.45);
   BSON_APPEND_DOCUMENT (&b, "7", &ar);
   BSON_APPEND_INT32 (&b, "8", 123);
   BSON_APPEND_INT64 (&b, "9", 456);
   BSON_APPEND_MINKEY (&b, "10");
   BSON_APPEND_MAXKEY (&b, "11");
   BSON_APPEND_NULL (&b, "12");
   BSON_APPEND_OID (&b, "13", &oid);
   BSON_APPEND_REGEX (&b, "14", "^abc", "i");
   BSON_APPEND_UTF8 (&b, "15", "utf8");
   BSON_APPEND_SYMBOL (&b, "16", "symbol");
   BSON_APPEND_TIME_T (&b, "17", t);
   BSON_APPEND_TIMEVAL (&b, "18", &tv);
   BSON_APPEND_DATE_TIME (&b, "19", 123);
   BSON_APPEND_TIMESTAMP (&b, "20", 123, 0);
   BSON_APPEND_UNDEFINED (&b, "21");
   BSON_APPEND_DECIMAL128 (&b, "22", &dec);

   bson_destroy (&b);
   bson_destroy (&ar);
}