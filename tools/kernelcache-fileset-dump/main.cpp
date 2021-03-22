#include <assert.h>
#include "KCFS/Slurp.h"
#include "KCFS/MachO.h"
#include <fmt/core.h>

int main(int argc, const char *argv[]) {
  assert(argc == 3);
  std::string infile(argv[1]);
  std::string outfile(argv[2]);
  uint64_t size;
  uint8_t *inbuf = Slurp::readfile(infile, &size);
  auto macho = (const MachO *)inbuf;
  fmt::print("macho: {}\n", (void*)macho);
  fmt::print("macho->cputype: {}\n", macho->cputype);
  // Slurp::writefile(outfile, inbuf, size);
  return 0;
}
