void PackLinuxElf::defineSymbols(Filter const *)
{
    linker->defineSymbol("O_BINFO", o_binfo);
}