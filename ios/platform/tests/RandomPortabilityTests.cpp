#include "Random.h"

#include <cstdlib>

int main() {
    Random first(12345);
    Random second(12345);

    for (int index = 0; index < 1000; ++index) {
        if (first.nextInt(100000) != second.nextInt(100000)) {
            return EXIT_FAILURE;
        }
    }

    Random bounded(9876);
    for (int index = 0; index < 1000; ++index) {
        const int value = bounded.nextInt(17);
        if (value < 0 || value >= 17) {
            return EXIT_FAILURE;
        }
    }

    Random doubles(42);
    for (int index = 0; index < 100; ++index) {
        const double value = doubles.nextDouble();
        if (value < 0.0 || value >= 1.0) {
            return EXIT_FAILURE;
        }
    }

    unsigned char bytes[16]{};
    bounded.nextBytes(bytes, 16);

    return EXIT_SUCCESS;
}
