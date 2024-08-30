def i32 fib(i32 n) {
    def i32 a = 0
    def i32 b = 1
    def i32 i = 0
    while ++i < n {
        def i32 x = a + b
        a = b
        b = x
    }
    return b
}

def i32 puts(i8*)
def i32 printf(i8*, ?)
def i32 atoi(i8*)

def i32 main(i32 argc, i8** argv) {
    if argc != 2 {
        puts("USAGE: fib <n>")
        return 1
    }

    def i32 n = atoi(argv[1])
    def i32 r = fib(n)

    printf("fib(%d) = %d\n", n, r)
    return 0
}
