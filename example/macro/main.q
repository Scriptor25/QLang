macro min(a, b) ((\a\) < (\b\) ? (\a\) : (\b\))

def i32 printf(i8*, ?)

def i32 main() {
    def f64 m = min(1.234, 5432)
    printf("min of 1.234 and 5432 is %f\n", m)
    return 0
}
