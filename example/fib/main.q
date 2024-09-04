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

def ext i32 printf(i8*, ?)
def ext i32 scanf(i8*, ?)
def ext i32 atoi(i8*)

def ext i32 main(i32 argc, i8** argv) {
    
    def i32 n
    
    if argc == 1 {
        scanf("%d", &n)
    }
    else if argc == 2 {
        n = atoi(argv[1])
    }
    else {
        printf("USAGE: %s <n>\n", argv[0])
        return 1
    }

    def i32 r = fib(n)

    printf("fib(%d) = %d\n", n, r)
    return 0
}
