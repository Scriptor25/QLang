#include "stdio.qh"
#include "string.qh"
#include "time.qh"

#macro theta_spacing 0.07
#macro phi_spacing   0.02
#macro R1 1.0
#macro R2 2.0
#macro K1 (screen_width * K2 * 3.0 / (8.0 * (R1 + R2)))
#macro K2 5.0
#macro pi 3.141596
#macro screen_width  40
#macro screen_height 20

#macro min(a, b) ((#a#) < (#b#) ? (#a#) : (#b#))
#macro max(a, b) ((#a#) > (#b#) ? (#a#) : (#b#))
#macro clamp(x, min, max) min(#max#, max(#min#, #x#))

#macro frame_time (CLOCKS_PER_SEC * 0.02)

def ext f64 cos(f64)
def ext f64 sin(f64)

def i8[screen_width * screen_height] output
def f64[screen_width * screen_height] z_buffer

def void render_frame(f64 A, f64 B) {
    def f64 cosA = cos(A)
    def f64 sinA = sin(A)
    def f64 cosB = cos(B)
    def f64 sinB = sin(B)

    memset(output, 0x20, screen_width * screen_height)
    memset(z_buffer, 0x00, screen_width * screen_height * 8)

    def f64 theta = 0
    while theta < 2 * pi {
        def f64 cos_theta = cos(theta)
        def f64 sin_theta = sin(theta)

        def f64 phi = 0
        while phi < 2 * pi {
            def f64 cos_phi = cos(phi)
            def f64 sin_phi = sin(phi)

            def f64 circle_x = R2 + R1 * cos_theta
            def f64 circle_y = R1 * sin_theta

            def f64 x = circle_x * (cosB * cos_phi + sinA * sinB * sin_phi) - circle_y * cosA * sinB
            def f64 y = circle_x * (sinB * cos_phi - sinA * cosB * sin_phi) + circle_y * cosA * cosB
            def f64 z = K2 + cosA * circle_x * sin_phi + circle_y * sinA
            def f64 ooz = 1 / z

            def i32 xp = screen_width * 0.5 + K1 * ooz * x
            def i32 yp = screen_height * 0.5 - K1 * ooz * y * 0.5

            if (xp >= 0 && xp < screen_width && yp >= 0 && yp < screen_height) {
                def f64 L = cos_phi * cos_theta * sinB - cosA * cos_theta * sin_phi - sinA * sin_theta + cosB * (cosA * sin_theta - cos_theta * sinA * sin_phi)

                if L > 0 {
                    def i64 idx = xp + yp * screen_width
                    if (ooz > z_buffer[idx]) {
                        z_buffer[idx] = ooz
                        def i64 luminance_index = clamp(L * 8, 0, 11)
                        output[idx] = ".,-~:;=!*#$@"[luminance_index]
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

    def f64 a = 0
    def f64 b = 0

    while 1 {
        def i64 t = clock()
        render_frame(a += 0.04, b += 0.02)
        while (clock() - t) < frame_time {}
    }

    return 0
}
