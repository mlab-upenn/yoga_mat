var net = require('net');

//ip address of cable box
var HOST = '127.0.0.1';
var PORT = 12321;

//create server for socket communication with cable box
net.createServer(function(sock) {
    
    // We have a connection - a socket object is assigned to the connection automatically
    console.log('CONNECTED: ' + sock.remoteAddress +':'+ sock.remotePort);
    
    // Add a 'data' event handler to this instance of socket
    sock.on('data', function(data) {
        var temp=""+data;
        if(temp[0]=="a"){
            //got the information, which could be changed according to different requirement
            setImmediate(function(){
                omx.pause();
            });
        }
        // Write the data back to the socket, the client will receive it as data from the server
        //sock.write(data);
        
    });
    
    // Add a 'close' event handler to this instance of socket
    sock.on('close', function(data) {
        console.log('CLOSED: ' + sock.remoteAddress +' '+ sock.remotePort);
    });

    console.log('Server listening on ' + HOST +':'+ PORT);

    sleep=require('sleep');
    //use omxplayer to play the video
    omx = require('omxcontrol');
    omx.start('yoga1.mp4');

    //set pause time and it can send control signal to cable box
    setTimeout(function(){
        omx.pause();
        sock.write("a");
    },10000);
    
}).listen(PORT, HOST);






