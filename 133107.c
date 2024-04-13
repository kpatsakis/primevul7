void license_print_product_info(PRODUCT_INFO* productInfo)
{
	char* CompanyName = NULL;
	char* ProductId = NULL;

	ConvertFromUnicode(CP_UTF8, 0, (WCHAR*) productInfo->pbCompanyName,
			productInfo->cbCompanyName / 2, &CompanyName, 0, NULL, NULL);

	ConvertFromUnicode(CP_UTF8, 0, (WCHAR*) productInfo->pbProductId,
			productInfo->cbProductId / 2, &ProductId, 0, NULL, NULL);

	fprintf(stderr, "ProductInfo:\n");
	fprintf(stderr, "\tdwVersion: 0x%08X\n", productInfo->dwVersion);
	fprintf(stderr, "\tCompanyName: %s\n", CompanyName);
	fprintf(stderr, "\tProductId: %s\n", ProductId);

	free(CompanyName);
	free(ProductId);
}