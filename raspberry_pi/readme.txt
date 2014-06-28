This project has three parts:

-------------------------------------
cloud part
this part is written in nodejs. To run this part, you should install node js on your cloud service. 
http://nodejs.org/download/
Also, there're some library to use, such as net, sleep and omxcontrol. To run this library, just run npm in node, such as
npm install net
npm install sleep
npm install omxcontrol

When all are set, you can run the cloud service by the command line:
node cloud.js, and it'll open the cloud service.

---------------------------------------
cable box part
this part is written in c. First you have to compile it, with the make file. To run the server, you have to use ./server +raspberry pi client ip address. And it'll go and discover the client. Same time it'll find the could service. It communicate with the raspberry pi client and cloud service with socket communication. There's no order of running cable box or cloud service or raspberry pi part.

----------------------------------------

raspberry pi part
this part is written in c and use wiringPi library to use the GPIO of raspberry. to compile the code, just run the make file. To run the code, please  run sudo ./client to run the code. And it'll connect to the cable box server and do the communication.

------------------------------------------
Things needed to be done
Generalize the control signal part.