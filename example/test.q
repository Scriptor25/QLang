use string as struct string {
    i64 length,
    i64 reserved,
    i8* buffer
}

def +string() {
    self.length = 0
    self.reserved = 8
    self.buffer = malloc(self.reserved)
}

def +string(i8* str) {
    self.length = strlen(str)
    self.reserved = self.length + 1
    self.buffer = malloc(self.reserved)

    strcpy(self.buffer, str)
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
    printf("%s (%d)", self.name, self.age)
}

def i32 main() {

    def person felix +("Felix", 18)
    def person max +("Max", 20)

    felix.print()
    puts("")
    max.print()
    puts("")

    return 0
}
