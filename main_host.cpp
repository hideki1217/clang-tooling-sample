#include <iostream>

extern const char sample_metadata_start[];
extern const char sample_metadata_end[];

int main() {
  char head[16 + 1];
  for (int i = 0; i < sizeof(head) - 1; i++) {
    head[i] = sample_metadata_start[i];
  }
  head[sizeof(head) - 1] = '\0';

  std::cout << "metadata: " << "\n";
  std::cout << "    start: " << std::hex << (void *)sample_metadata_start
            << "    '" << head << "'" << "\n";
  std::cout << "      end: " << std::hex << (void *)sample_metadata_end << "\n";
}