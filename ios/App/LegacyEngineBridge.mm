#import "LegacyEngineBridge.h"
#include "Vec3.h"
#include "Random.h"
#include <cmath>

@implementation LegacyEngineBridge

+ (NSString *)runCoreSmokeTest {
    Vec3::CreateNewThreadStorage();
    Vec3 *vector = Vec3::newTemp(3.0, 4.0, 0.0);
    const double length = vector->length();
    Vec3 *translated = vector->add(1.0, -2.0, 3.0);
    const bool valid = std::fabs(length - 5.0) < 1e-9 &&
        std::fabs(translated->x - 4.0) < 1e-9 &&
        std::fabs(translated->y - 2.0) < 1e-9 &&
        std::fabs(translated->z - 3.0) < 1e-9;
    Vec3::ReleaseThreadStorage();
    Random first(12345);
    Random second(12345);
    bool randomValid = true;
    for (int index = 0; index < 16; ++index) {
        if (first.nextInt(100000) != second.nextInt(100000)) {
            randomValid = false;
            break;
        }
    }
    if (valid && randomValid) {
        return [NSString stringWithFormat:@"Legacy C++ core linked · Vec3 + Random passed · length %.1f", length];
    }
    return @"Legacy C++ core linked · core check failed";
}

@end
