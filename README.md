# AOS-2023

Input<br>
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

Output <br>
![image](https://github.com/Richa-iitr/AOS-2023/assets/76250660/8c4e85a7-591f-4f09-b711-f86a3593122d)
