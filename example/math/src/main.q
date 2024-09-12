#include "vec3.qh"

def ext i32 printf(i8*, ?)

def ext i32 main() {
    def vec3 a { 0.1, 0.2, 0.9 }
    def vec3 b { 2.5, -0.1, 0.123 }

    def vec3 ab = a + b
    printf("%f %f %f (%f)\n", ab[0], ab[1], ab[2], ab.length())

    return 0
}
