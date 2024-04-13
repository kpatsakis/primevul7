BSON_ASSERT_BSON_EQUAL (const bson_t *a, const bson_t *b)
{
   const uint8_t *data1 = bson_get_data (a);
   const uint8_t *data2 = bson_get_data (b);
   uint32_t i;

   if (!bson_equal (a, b)) {
      for (i = 0; i < BSON_MAX (a->len, b->len); i++) {
         if (i >= a->len) {
            printf ("a is too short len=%u\n", a->len);
            abort ();
         } else if (i >= b->len) {
            printf ("b is too short len=%u\n", b->len);
            abort ();
         }
         if (data1[i] != data2[i]) {
            printf ("a[%u](0x%02x,%u) != b[%u](0x%02x,%u)\n",
                    i,
                    data1[i],
                    data1[i],
                    i,
                    data2[i],
                    data2[i]);
            abort ();
         }
      }
   }
}