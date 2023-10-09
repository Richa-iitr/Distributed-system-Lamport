# AOS-2023

## Code Run:
`(Version: g++.exe (MinGW.org GCC-6.3.0-1) 6.3.0)`
- Compile the code with the command: g++ lamport.cpp
- Run the compiled program: ./a

## Sample Input/Output:
*Input format*: write all the commands on newline, end command accepts any format starting with end  (end process, end process p1, end ), an input of newline (enter button twice) will print the output.

1. 
```
begin process p1
print e11
send (p2) e12
print e13
print e14
send (p2) e15
print e16
recv p2 e17
end process
begin process p2
print e21
print e22
recv p1 e12
send (p1) e17
recv p1 e15
print e26
end process
```
![image](https://github.com/Richa-iitr/AOS-2023/assets/76250660/46f69333-8583-49a9-9ec3-55782a787331)

2
```
begin process p1
recv p2 m2
send (p4) m4
end process
begin process p2
recv p3 m3
send (p1) m2
end
begin process p3
recv p4 m1
send (p2) m3
end process
begin process p4
send (p3) m1
end process
```
![image](https://github.com/Richa-iitr/AOS-2023/assets/76250660/56f79084-846b-41b9-b6cf-e6a602507f46)

3
```
begin process p1
recv p2 m2
send (p4) m4
end process
begin process p2
recv p3 m3
send (p1) m2
end
begin process p3
recv p4 m1
send (p2) m3
end process
begin process p4
send (p3) m9
end
```
![image](https://github.com/Richa-iitr/AOS-2023/assets/76250660/710a6649-17bb-4cf1-bc2d-71e0f0cc24c2)

4
```
begin process p1
recv p2 m2
send (p4) m4
end process
begin process p2
recv p3 m3
send (p1) m2
end
begin process p3
recv p4 m1
send (p2) m3
end process
begin process p4
print def
end process
```
![image](https://github.com/Richa-iitr/AOS-2023/assets/76250660/dd194fa4-25c8-4ab8-8771-71fbbf25a6aa)

5
```
begin process p1
recv p2 m2
end process
begin process p2
recv p1 m1
end process
```
![image](https://github.com/Richa-iitr/AOS-2023/assets/76250660/1f9a4a71-8bba-414b-b32b-d59d069f9310)

6
```
begin process p1
recv p2 m2
send (p4) m4
end process
begin process p2
recv p3 m3
send (p1) m2
end
begin process p3
recv p4 m1
send (p2) m3
end process
begin process p4
recv p1 m4
end process
```
![image](https://github.com/Richa-iitr/AOS-2023/assets/76250660/960206af-ec68-44c6-af14-8addec72e834)

7
```
begin process p1
send (p2) m1
print abc
print def
end process
begin process p2
print x1
recv p1 m1
print x2
send (p1) m2
print x3
end process
```
![image](https://github.com/Richa-iitr/AOS-2023/assets/76250660/42f86ecd-fe86-44be-a832-bed03768d4d3)


