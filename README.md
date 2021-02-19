# Growtopia Private Server Source Code
**This is a re created version of [GrowtopiaNoobs' Project](https://github.com/LucarioGamer/GrowtopiaServer)**. Use this for your own will

> **The header files are already filtered. There's nothing that you need to worry about regarding the category**

> ***I won't be responsible for any bugs and errors that may occur as this source code isn't meant to be used by new people who just learned C++.***

> ***DO NOT repost this source in any place and selling it, or calling you're the one who made this source!***

> ***The main function is located at `Event_Handler.h` inside `void runEvent()`

- No credits is fine.
## DO NOT DO THIS
**DO NOT** declare the namespace in the header file as it's a dangerious thing to do.

***For example***
```c++
#pragma once
// You're not allowed to do this in the header file
#include <iostream>

using namespace std;

cout << "Console out" << endl;
```

```c++
#pragma once
// instead, please do this
#include <iostream>

std::cout << "Console out" << std::endl;
```

**DO NOT** put your function inside the header file as it may will 
cause compiler becomes slower

```c++
#pragma once
// Bad
#include <iostream>

void foo() {
    sd::cout << "foo" std::endl;
}
```

**Instead**, header file must only contains extern variables and function prototypes

```c++
#pragma once
// Header.h
#include <iostream>

void foo();

// Header.cpp
#include "Header.h>

void foo() {
    std::cout << "foo" std::endl;
}
```
## Notes
*If you're new to C++*, I don't recommend you to straight away making a private server
and saying you'll get better at C++ by just making a private server. You won't get rich by owning a server!

*If you're new to C++*, I don't recommend you to start using this source as this source involves header files. 
Since header files are confusing to some people such as where to put this, where is `this` code at.

**This is just a suggestion**. Some people might new to C++. As I've see how they code. For example,
If you want to make your codes readable by others, you can try to do that by simply to do `this` and `not this`

```c++
// nested if() is okay, but do it for something that you need to

// Don't do this
if (foo == false) {}

// Instead, do this
if (!foo) {}

// Don't do this
if (!isCooldown(foo)) {
    // do something
    if (foo2.gems > 999) { // enough gems
        if (foo3.online) // do something
    }
}
else {
    // You're in cooldown!
}

// Instead, do this
if (isCooldown(foo)) {
    // You're in cooldown!
    return; // return / break, depends on the situation
}
else if (foo2.gems < 1000)
     return; // not enough gems
else if (!foo3.online) 
    return;

// do something
```

**Something else that you must avoid is using `if()` and `else if()`
too much**

```c++
// Bad
if (foo == 1) 
    // do something
else if (foo == 2)
    // do something
else if (foo == 3)
    // do something
else if (foo == 4) 
    // Do something

// Good
switch (foo)
{
    case 1: // do something
    break;
    // ...
}

// If you want to execute one of the statement that is true

// Bad
if (foo == "one") // do something
if (foo == "two") // do something
if (foo == "four") // do something

// Good
if (foo == "one") 
    // do something
else if (foo == "two") 
    // do something
else if (foo == "four") 
    // do something
```

~ LinkProfit
