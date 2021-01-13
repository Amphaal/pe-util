#include "Arguments.h"
#include "Traverser.h"

#include <exception>

int main(int argc, char **argv) {
  try {
    Arguments args;
    args.parse(argc, argv);
    Traverser t(args);
  } catch (const exception &e) {
    cerr << "Error: " << e.what() << '\n';
    exit(1);
  }
}
