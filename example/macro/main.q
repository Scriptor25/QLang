include "assert.qh"

macro min(a, b) ((#a#) < (#b#) ? (#a#) : (#b#))

def i32 main() {
    def f64 m = min(1.234, 5432)
    printf("min of 1.234 and 5432 is %f\n", m)
    assert(m == 1.235)
    return 0
}
