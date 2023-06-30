Ét eo ét!!!

Sau khi tổng hợp lại thì em có một số vấn đề như sau
Dưới đây là một số yêu cầu cần giúp:
1. Làm lại task 2.2 chạy theo mẫu, sử dụng C
2. Ghép 2.1 tới 2.4 thành 1 file task 2 chạy được như yêu cầu đề bài :(
3. Xử lý hàm checksum của Task 3, vì hiện tại đang sai so với testcase



Trong folder này có file của Task1 Task3. Riêng Task 2 hiện tại đang có 4 file chưa ghép, đang hơi rời rạc :'(
Trong này có kèm theo testcase của thầy



//Run
Task1:
g++ -o dust_sensor Task1.cpp  
./dust_sensor -n [] -st [] -si [] >> task1.log 2>&1

Task2:
T2.1: dust_process_task21
g++ -o dust_process dust_process_task21.cpp
./dust_process >> task2.log 2>&1

T2.2: dust_process_task22
g++ -o dust_aqi dust_aqi.cpp
./dust_aqi >> task21.log 2>&1

T2.1: dust_process_task23
g++ -o dust_summary dust_summary_task23.cpp
./dust_summary

T2.1: dust_process_task24
g++ -o dust_statistic dust_statistic_task24.cpp
./dust_statistic

Task3:
gcc -o dust_convert Task3.c
./dust_convert dust_aqi.csv hex_packet.dat


