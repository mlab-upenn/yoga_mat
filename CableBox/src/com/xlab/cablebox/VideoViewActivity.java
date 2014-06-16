package com.xlab.cablebox;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.ServerSocket;
import java.net.Socket;

import android.media.MediaPlayer;
import android.media.MediaPlayer.OnPreparedListener;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.app.Activity;
import android.app.ProgressDialog;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;
import android.widget.MediaController;
import android.widget.VideoView;
 
public class VideoViewActivity extends Activity {
 
	private ServerSocket serverSocket;

	Handler updateConversationHandler;

	Thread serverThread = null;
	

	public static final int SERVERPORT = 6000;
    // Declare variables
    ProgressDialog pDialog;
    VideoView videoview;
 
    // Insert your Video URL
    String VideoURL = "http://158.130.12.49:3000/uploads/fullsize/macys.mp4";
 
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, 
                                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        // Get the layout from videoview_main.xml	
        setContentView(R.layout.videoview_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        // Find your VideoView in your video_main.xml layout
        videoview = (VideoView) findViewById(R.id.VideoView);
 
        // Create a progressbar
        pDialog = new ProgressDialog(VideoViewActivity.this);
        // Set progressbar title
        pDialog.setTitle("Yoga Mat");
        // Set progressbar message
        pDialog.setMessage("Buffering...");
        pDialog.setIndeterminate(false);
        pDialog.setCancelable(false);
        // Show progressbar
        pDialog.show();
        this.serverThread = new Thread(new ServerThread());
		this.serverThread.start();
        try {
            // Start the MediaController
            MediaController mediacontroller = new MediaController(
                    VideoViewActivity.this);
            mediacontroller.setAnchorView(videoview);
            // Get the URL from String VideoURL
            Uri video = Uri.parse(VideoURL);
            videoview.setMediaController(mediacontroller);
            videoview.setVideoURI(video);
 
        } catch (Exception e) {
            Log.e("Error", e.getMessage());
            e.printStackTrace();
        }
 
        videoview.requestFocus();
        videoview.setOnPreparedListener(new OnPreparedListener() {
            // Close the progress bar and play the video
            public void onPrepared(MediaPlayer mp) {
                pDialog.dismiss();
                //videoview.start();
            }
        });
 
    }
    protected void onResume(){
    	super.onResume();
    }
    protected void onPause(){
    	super.onPause();
    	videoview.pause();
    }
   
	protected void onStop() {
		super.onStop();
		try {
			serverSocket.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	class ServerThread implements Runnable {

		public void run() {
			Socket socket = null;
			try {
				
				serverSocket = new ServerSocket(SERVERPORT);
				Log.d("", "server created");
				System.out.println("server created");
			} catch (IOException e) {
				e.printStackTrace();
			}
			while (!Thread.currentThread().isInterrupted()) {

				try {
					Log.d("msg", "before accept");

					socket = serverSocket.accept();

					CommunicationThread commThread = new CommunicationThread(socket);
					new Thread(commThread).start();

				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
	}

	class CommunicationThread implements Runnable {

		private Socket clientSocket;

		private BufferedReader input;
		private BufferedWriter output;
		public CommunicationThread(Socket clientSocket) {
			if(clientSocket.isConnected()){
				this.clientSocket = clientSocket;
			
				try {
	
					this.input = new BufferedReader(new InputStreamReader(this.clientSocket.getInputStream()));
					
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}

		public void run() {

			while (!Thread.currentThread().isInterrupted()) {
				if(this.clientSocket != null && this.clientSocket.isConnected()){
					try {
						
						String read = input.readLine();
						if(read != null){
							Log.d("msg",read);
							String outgoingMsg;
							outgoingMsg = read + "\n" ;
					        this.output = new BufferedWriter(new OutputStreamWriter(this.clientSocket.getOutputStream()));
		
					        this.output.write(outgoingMsg);
					        this.output.flush();
							switch(read){
							case "p": 
								videoview.start();
								
								break;
							case "s":
								videoview.pause();
								break;
							case "q":
								this.clientSocket.close();
								break;
							default:
								
									
							}

						}
						else{
							this.output = new BufferedWriter(new OutputStreamWriter(this.clientSocket.getOutputStream()));
							
					        this.output.write("empty");
					        this.output.flush();
						}
		
						
						
	
					} catch (IOException e) {
						e.printStackTrace();
					}
				}
			}
		}

	}
	class updateUIThread implements Runnable {
		private String msg;

		public updateUIThread(String str) {
			this.msg = str;
		}

		@Override
		public void run() {
			Log.d("", msg);
		}
	}
 
}