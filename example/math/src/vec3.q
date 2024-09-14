#include "vec3.qh"

#macro min(a, b) ((#a#) < (#b#) ? (#a#) : (#b#)) #endmacro

def ext f64 abs(f64)
def ext f64 sqrt(f64)

def +vec3(f64 t) { self[0] = self[1] = self[2] = t }

def +vec3(f64 x, f64 y, f64 z) {
    self[0] = x
    self[1] = y
    self[2] = z
}

def +vec3(vec3 v) {
    self[0] = v[0]
    self[1] = v[1]
    self[2] = v[2]
}

def vec3& vec3:operator=(vec3 v) {
    self[0] = v[0]
    self[1] = v[1]
    self[2] = v[2]
    return self
}

def vec3 vec3:operator-() { return vec3(-self[0], -self[1], -self[2]) }

def f64& vec3:operator[](i64 i) { return self.e[i] }

def vec3& vec3:operator+=(vec3 v) {
    self[0] += v[0]
    self[1] += v[1]
    self[2] += v[2]
    return self
}

def vec3& vec3:operator-=(vec3 v) {
    self[0] -= v[0]
    self[1] -= v[1]
    self[2] -= v[2]
    return self
}

def vec3& vec3:operator*=(f64 t) {
    self[0] *= t
    self[1] *= t
    self[2] *= t
    return self
}

def vec3& vec3:operator/=(f64 t) { return self *= 1 / t }

def vec3& vec3:operator%=(f64 t) {
    self[0] %= t
    self[1] %= t
    self[2] %= t
    return self
}

def f64 vec3:length_squared() { return self[0] * self[0] + self[1] * self[1] + self[2] * self[2] }

def f64 vec3:length() { return sqrt(self.length_squared()) }

def vec3 operator+(vec3 a, vec3 b) { return vec3(a[0] + b[0], a[1] + b[1], a[2] + b[2]) }

def vec3 operator-(vec3 a, vec3 b) { return vec3(a[0] - b[0], a[1] - b[1], a[2] - b[2]) }

def vec3 operator*(f64 a, vec3 b) { return vec3(a * b[0], a * b[1], a * b[2]) }

def vec3 operator*(vec3 a, f64 b) { return b * a }

def vec3 operator/(vec3 a, f64 b) { return (1 / b) * a }

def vec3 operator%(vec3 a, f64 b) { return vec3(a[0] % b, a[1] % b, a[2] % b) }

def f64 dot(vec3 a, vec3 b) { return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] }

def vec3 cross(vec3 a, vec3 b) {
    return vec3(
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0]
    )
}

def vec3 reflect(vec3 v, vec3 n) { return v - 2 * dot(v, n) * n }

def vec3 refract(vec3 v, vec3 n, f64 i) {
    def f64 ct = min(dot(-v, n), 1.0)
    def vec3 perp = i * (v + ct * n)
    def vec3 para = -sqrt(abs(1.0 - perp.length_squared())) * n
    return perp + para
}
