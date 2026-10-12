#include "Vec3.h"

#include <cmath>
#include <cstdlib>

int main() {
    Vec3::CreateNewThreadStorage();

    Vec3* vector = Vec3::newTemp(3.0, 4.0, 0.0);
    if (std::fabs(vector->length() - 5.0) > 1e-9) {
        return EXIT_FAILURE;
    }

    Vec3* moved = vector->add(1.0, -2.0, 3.0);
    if (std::fabs(moved->x - 4.0) > 1e-9 ||
        std::fabs(moved->y - 2.0) > 1e-9 ||
        std::fabs(moved->z - 3.0) > 1e-9) {
        return EXIT_FAILURE;
    }

    Vec3::ReleaseThreadStorage();
    return EXIT_SUCCESS;
}
