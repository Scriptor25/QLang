include "time.qh"

macro theta_spacing 0.035
macro phi_spacing 0.01
macro R1 1.0
macro R2 2.0
macro K1 = (screen_width * K2 * 3.0 / (8.0 * (R1 + R2)))
macro K2 5.0
macro pi 3.141596
macro screen_width 80
macro screen_height 40

macro min(a, b) ((#a#) < (#b#) ? (#a#) : (#b#))
macro max(a, b) ((#a#) > (#b#) ? (#a#) : (#b#))
macro clamp(x, min, max) min(#max#, max(#min#, #x#))

def ext f64 cos(f64)
def ext f64 sin(f64)
def ext void* malloc(i64)
def ext void* memset(void*, i64, i64)
def ext void free(void*)
def ext i32 printf(i8*, ?)
def ext i32 putchar(i8)

def i8* output
def f64* zbuffer

def void render_frame(f64 A, f64 B) {
    def f64 cosA = cos(A)
    def f64 sinA = sin(A)
    def f64 cosB = cos(B)
    def f64 sinB = sin(B)

    memset(output, 0x20, screen_width * screen_height)
    memset(zbuffer, 0x00, screen_width * screen_height * 8)

    def f64 theta = 0
    while theta < 2 * pi {
        def f64 costheta = cos(theta)
        def f64 sintheta = sin(theta)

        def f64 phi = 0
        while phi < 2 * pi {
            def f64 cosphi = cos(phi)
            def f64 sinphi = sin(phi)

            def f64 circlex = R2 + R1 * costheta
            def f64 circley = R1 * sintheta

            def f64 x = circlex * (cosB * cosphi + sinA * sinB * sinphi) - circley * cosA * sinB
            def f64 y = circlex * (sinB * cosphi - sinA * cosB * sinphi) + circley * cosA * cosB
            def f64 z = K2 + cosA * circlex * sinphi + circley * sinA
            def f64 ooz = 1 / z

            def i32 xp = screen_width * 0.5 + K1 * ooz * x
            def i32 yp = screen_height * 0.5 - K1 * ooz * y * 0.5

            if (xp >= 0 && xp < screen_width && yp >= 0 && yp < screen_height) {
                def f64 L = cosphi * costheta * sinB - cosA * costheta * sinphi - sinA * sintheta + cosB * (cosA * sintheta - costheta * sinA * sinphi)

                if L > 0 {
                    def i64 idx = xp + yp * screen_width
                    if (ooz > zbuffer[idx]) {
                        zbuffer[idx] = ooz
                        def i64 lidx = clamp(L * 8, 0, 11)
                        output[idx] = ".,-~:;=!*#$@"[lidx]
                    }
                }
            }

            phi += phi_spacing
        }

        theta += theta_spacing
    }

    printf("\x1b[H")
    def i32 j = 0
    while j < screen_height {
        printf("%.*s\n", screen_width, &output[j * screen_width])
        j++
    }
}

def ext i32 main() {
    output = malloc(screen_width * screen_height)
    zbuffer = malloc(screen_width * screen_height * 8)

    def f64 a = 0
    def f64 b = 0
    while 1 {
        def i64 t = clock()
        render_frame(a += 0.04, b += 0.02)
        while (clock() - t) < (CLOCKS_PER_SEC * 0.02) {
        }
    }
    
    free(output)
    free(zbuffer)
    return 0
}
