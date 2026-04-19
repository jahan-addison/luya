#include <crescendum/hello.h> // for Greeter
#include <fmt/base.h>         // for println

int main()
{
    auto greeter = crescendum::Greeter{};
    fmt::println("Greet: {}", greeter.greet("John"));
}