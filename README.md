### 95w
A CD / OEM key generator for Windows 95, NT Server, and NT Workstation.

### Notes
Windows 95, NT Server, and NT Workstation uses a mod 7 value to determine whether the provided key is valid or not. Usually, it's dividable by 7 which should result in reminder 0 and that's all! This is applied in both CD and OEM keys.

#### FAQ
1. I can't be able to compile the program on Windows.\
A. I personally don't use Windows. What you can do either to implement nanosecond as seed for Xorshitft64 by using `SYSTEMTIME` (see [here](https://learn.microsoft.com/en-us/windows/win32/api/minwinbase/ns-minwinbase-systemtime)) or compile it under Linux (or BSD system). Usually, if you grab the Arch Linux ISO (which is likely 800+ MB in size) you can compile and generate the key there.

2. Is there any specification out here?\
A. Yes. Original [Specification](https://gurney.dev/posts/mod7/) or [another](./paper.txt)

