#include "core.h"

int main() {
  clean_env();
  welcome();

  while (1) {
    fetch_input_hashes_from_DB();
    detect_hashType_generate_hashfiles();
    launch_hashcat();
    update_db();

    sleep(10);
  }

  return 0;
}
