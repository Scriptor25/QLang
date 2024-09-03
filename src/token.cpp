#include <QLang/Token.hpp>

bool QLang::Token::operator!() const { return !Type; }
