#import "LegacyEngineBridge.h"
#include "Vec3.h"
#include "Random.h"
#include "AABB.h"
#include <cmath>

@implementation LegacyEngineBridge

+ (NSString *)runCoreSmokeTest {
    Vec3::CreateNewThreadStorage();
    AABB::CreateNewThreadStorage();
    Vec3 *vector = Vec3::newTemp(3.0, 4.0, 0.0);
    const double length = vector->length();
    Vec3 *translated = vector->add(1.0, -2.0, 3.0);
    const bool valid = std::fabs(length - 5.0) < 1e-9 &&
        std::fabs(translated->x - 4.0) < 1e-9 &&
        std::fabs(translated->y - 2.0) < 1e-9 &&
        std::fabs(translated->z - 3.0) < 1e-9;
    AABB *body = AABB::newTemp(0.0, 0.0, 0.0, 1.0, 2.0, 1.0);
    AABB *wall = AABB::newTemp(0.8, 0.0, 0.0, 1.8, 2.0, 1.0);
    const bool collisionValid = body->intersects(wall) &&
        !body->intersects(2.0, 0.0, 0.0, 3.0, 1.0, 1.0);
    AABB::ReleaseThreadStorage();
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
    if (valid && randomValid && collisionValid) {
        return [NSString stringWithFormat:@"Legacy C++ core linked · Vec3 + Random + AABB collisions passed · length %.1f", length];
    }
    return @"Legacy C++ core linked · core check failed";
}

@end
