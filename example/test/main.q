def ext void* malloc(i64)
def ext void* realloc(void*, i64)
def ext void free(void*)
def ext i64 strlen(i8*)
def ext i8* strcpy(i8*, i8*)
def ext i8* strcat(i8*, i8*)
def ext i32 printf(i8*, ?)
def ext i32 puts(i8*)

use string as struct string {
    i64 length = 0,
    i64 reserved = 8,
    i8* data = 0
}

def i8& string:operator[](i64 i) {
    return self.data[i]
}

def +string() {
    self.data = malloc(self.reserved)
    self[0] = 0
}

def +string(i8* str) {
    self.length = strlen(str)
    self.reserved = self.length + 1
    self.data = malloc(self.reserved)
    strcpy(self.data, str)
}

def -string() {
    free(self.data)
}

def i8* string:operator$() {
    return self.data
}

def string& string:operator=(i8* str) {
    self.length = strlen(str)
    self.reserved = self.length + 1
    self.data = realloc(self.data, self.reserved)
    strcpy(self.data, str)
    return self
}

def string& string:operator=(string str) {
    self.length = str.length
    self.reserved = str.reserved
    self.data = realloc(self.data, self.reserved)
    strcpy(self.data, str.data)
    return self
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

def string& string:operator+=(string str) {
    self.prereserve(str.length)
    strcat(self.data, str.data)
    return self
}

def string operator+(i8 lhs, string rhs) {
    def string result
    result += lhs
    result += rhs
    return result
}

def string operator+(string lhs, i8 rhs) {
    def string result
    result += lhs
    result += rhs
    return result
}

def string operator+(string lhs, string rhs) {
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
    printf("%s (%d)\n", self.name.data, self.age)
}

def ext i32 main() {

    def person felix { "Felix", 18 }
    def person max { "Max", 20 }

    felix.print()
    max.print()

    def string felix_max = felix.name + ' ' + max.name
    puts(felix_max)

    return 0
}
