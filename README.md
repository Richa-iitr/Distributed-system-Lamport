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
![image](https://github.com/Richa-iitr/AOS-2023/assets/76250660/359796ee-adda-4e88-8681-98c7e632ab5f)

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
![image](https://github.com/Richa-iitr/AOS-2023/assets/76250660/f2172613-eb72-4872-89a5-fc44a1bf6134)


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
![image](https://github.com/Richa-iitr/AOS-2023/assets/76250660/262d4def-45da-4e40-acf5-7e0386977e3a)

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
![image](https://github.com/Richa-iitr/AOS-2023/assets/76250660/d072ae15-4d82-4dd6-a816-f9e50f2b1a9c)

5
```
begin process p1
recv p2 m2
end process
begin process p2
recv p1 m1
end process
```
![image](https://github.com/Richa-iitr/AOS-2023/assets/76250660/09638c79-aff0-42ed-8772-2832873d1e09)

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
![image](https://github.com/Richa-iitr/AOS-2023/assets/76250660/9e5b5f20-b4c7-4df9-bea6-c3bee54cb06d)

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
![image](https://github.com/Richa-iitr/AOS-2023/assets/76250660/3fc8dd7f-31c4-4dce-94f5-1747ac3662a8)

8
```
begin process p1
send (p2,p3) m1
end process
begin process p2
recv p1 m1
end process
begin process p3
recv p1 m1
end process

```
![image](https://github.com/Richa-iitr/AOS-2023/assets/76250660/7ce94fa4-56eb-4ee5-a43e-b9b44d00e15f)

9
```
begin process p1
send (p2) m1
send (p2) m1
end process
begin process p2
recv p1 m1
recv p1 m1
end process
```
![image](https://github.com/Richa-iitr/AOS-2023/assets/76250660/d8d757a3-d95e-4ec2-b784-d0bc7d666ba5)

10
```
begin process p1
send (p2) m1
end process
begin process p2
recv p1 m1
recv p1 m1
end process
```
![image](https://github.com/Richa-iitr/AOS-2023/assets/76250660/860f9ca8-ffc4-4eab-b617-52d7f8e13902)

## Errors Caught
- Receiver process does not exist.
- System deadlock
- System stuck (message needed to proceed not sent and deadlock is not there)
- Sender process does not exist
- Incorrect input format
- Starting new process before ending previous
- Starting the same process again
- Empty receivers, message
- Sending msg to itself
- Incorrect command etc.


