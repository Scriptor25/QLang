use string as struct string {
    i64 length,
    i64 reserved,
    i8* data
}

def i8& string:operator[](i64 i) {
    return self.data[i]
}

def +string() {
    self.length = 0
    self.reserved = 8
    self.data = malloc(self.reserved)
    self[0] = 0
}

def -string() {
    free(self.data)
}

def +string(i8* str) {
    self()
    strcpy(self.data, str)
}

def void string:prereserve(i64 n) {
    self.length += n
    if self.length >= self.reserved {
        self.reserved *= 2
        self.data = realloc(self.data, self.reserved)
    }
}

def string& string:operator+=(i8 chr) {
    self.prereserve(1)
    self[self.length - 1] = chr
    self[self.length] = 0
    return self
}

def string& string:operator+=(string& str) {
    self.prereserve(str.length)
    strcat(self.data, str.data)
}

def string operator+(string& lhs, i8 rhs) {
    def string result
    result += lhs
    result += rhs
    return result
}

def string operator+(string& lhs, string& rhs) {
    def string result
    result += lhs
    result += rhs
    return result
}

use person as struct person {
    string name,
    i32 age
}

def +person(i8* name, i32 age) {
    self.name = name
    self.age = age
}

def void person:print() {
    printf("%s (%d)", self.name.data, self.age)
}

def i32 main() {

    def person felix +("Felix", 18)
    def person max +("Max", 20)

    felix.print()
    puts("")
    max.print()
    puts("")

    def string felix_max = felix.name + ' ' + max.name
    puts(felix_max)

    return 0
}
