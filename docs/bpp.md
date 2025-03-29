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

