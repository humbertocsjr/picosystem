# B++ Language

## Syntax Highlighting

[External B Language Syntax Highlighting for VS Code](https://github.com/b-foundation/vscode-b-syntax-highlighting)

## Compiler Usage

- Compile file.bpp to file.com

    ```sh
    i86-bpp output.s /usr/local/picosdk/include/stdlib.bpp file.bpp

    i86-as -o output.o output.s
    
    i86-ld -f com -o output.com -L /usr/local/picosdk/lib/ -l libdos.a output.o 
    ```

## New features compared to the B language

- Near Pointers and Far Pointers
- Signed/Unsigned Math and Logic
- Unified variable declaration
- Byte Vectors and Pointers (Using **@** marker)
- Modernized and Modularized Standard Library

# Language Manual

## Unsigned Math and Logic Marker

```c
// 16 bit words
auto a = unsigned(0x4444 < 0xffff); // true
auto b = 0x4444 < 0xffff; // false
```

## Function Definition

```c
main()
{
    // commands
}
```

## Function Declaration

\* Only valid at root level

```c
external_function();
```

## Constant Declaration

```c
const CONST_TEST = 1;
```

## Structure Declaration

#### Byte Pointer Struct:

```c
struct OBJ_BTEST
{
    BTEST_ID_LOW, // size: 1 byte | offset: 0
    BTEST_ID_HIGH, // size: 1 byte | offset: 1
    BTEST_NAME, // size: 8 bytes | offset: 2
    BTEST_GROUP_ID = 10 // size: 1 | offset: 10
}
```

Usage:

```c
get_user_id(user)
{
    return  user[BTEST_ID_LOW byte] | (user[BTEST_ID_HIGH byte] << 8);
}
```

#### Word Pointer Struct:

```c
struct OBJ_WTEST
{
    WTEST_ID, // size: 2 bytes | offset: 0
    WTEST_NAME = 1, // size: 8 bytes | offset: 2
    WTEST_GROUP_ID = 5 // size: 2 bytes | offset: 10
}
```

Usage:

```c
get_user_id(user)
{
    return  user[WTEST_ID word];
}
```

#### Far Pointer Struct:

```c
struct OBJ_FTEST
{
    FTEST_ID, // size: 2 bytes | offset: 0
    FTEST_NAME = 2, // size: 8 bytes | offset: 2
    BTEST_GROUP_ID = 10 // size: 2 bytes | offset: 10
}
```

Usage:

```c
get_user_id(user)
{
    return  *user:FTEST_ID;
}
```

## Variable Declaration

```c
auto var_name;
```

#### Multiple Variables Declaration

```c
auto var_name1, var_name2, var_name3;
```

#### Vector Declaration

```c
auto vector_name[50];
```

OR

```c
auto vector_name[50 word];
```

OR

```c
auto vector_name[50 words];
```

#### Byte Vector Declaration

```c
auto vector_name[50 byte];
```

OR

```c
auto vector_name[50 bytes];
```

## If

\* Only valid at function level

```c
if(a > 0)
{
    command_if_true();
}
```

OR

```c
if(a > 0) command_if_true();
```

## If Else

\* Only valid at function level

```c
if(a > 0)
{
    command_if_true();
}
else
{
    command_if_false();
}
```

OR

```c
if(a > 0) command_if_true();
else command_if_false();
```

## While Loop

\* Only valid at function level

```c
while(a < 10)
{
    a++;
}
```

## Until Loop

\* Only valid at function level

```c
until(a >= 10)
{
    a++;
}
```
## Do While Loop

\* Only valid at function level

```c
do
{
    a++;
}
while(a < 10);
```

## Do Until Loop

\* Only valid at function level

```c
do
{
    a++;
}
until(a >= 10);
```

## For Loop

\* Only valid at function level

```c
auto i;
for(i = 0; i < 10; i++)
{
}
```

## Near Word Pointers

\* Only valid at function level

```c
auto ptr, a = 5;
ptr = &a;
*ptr = 6;
// now a == 6
```

## Near Byte Pointers

\* Only valid at function level

```c
auto ptr, a = 0x500;
ptr = &a;
@ptr = 0x6;
// now a == 0x506
```

## Far Word Pointers

\* Only valid at function level

```c
// Write 'A' on first location of screen with light gray color on 16 bit PC
*0xb800:0 = 'A'| 0x700;
```

## Far Byte Pointers

\* Only valid at function level

```c
// Write 'A' on third location of screen on 16 bit PC
@0xb800:(3 * 2) = 'A';
```

## Inline Assembly

```c
asm("mov ax, ax");
```

OR

```c
asm
(
    "mov ax, 0xe00 | 'a'",
    "int 0x10"
);
```

