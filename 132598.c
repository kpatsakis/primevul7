secret_core_std (gcry_mpi_t M, gcry_mpi_t C,
                 gcry_mpi_t D, gcry_mpi_t N)
{
  mpi_powm (M, C, D, N);
}