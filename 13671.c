static int __init algif_skcipher_init(void)
{
	return af_alg_register_type(&algif_type_skcipher);
}