package PlayerComponents;

import Config.Constants;

import java.awt.image.*;
import java.io.*;
import javax.swing.*;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.Timer;
import java.util.TimerTask;
import java.lang.*;

public class Video {

	File vidFile;
	Audio audioPlayer;
	FileInputStream is;
	public BufferedImage img;
	Timer timer;

	long fileLen;
	long delay = 0;
	long interval = 67;
	int currFrameNum;

		public Video(String vidPath, Audio audioPlayer){
		img = new BufferedImage(Constants.WIDTH, Constants.HEIGHT, BufferedImage.TYPE_INT_RGB);
		this.audioPlayer = audioPlayer;
		currFrameNum = 0;
		System.out.println("Name: " + audioPlayer.name);
		try{
			vidFile = new File(vidPath);
			is = new FileInputStream(vidFile);
			fileLen = vidFile.length();
		}
		catch (FileNotFoundException e) {
			e.printStackTrace();
		}
	}

	public void play(JFrame frame){
		TimerTask renderFrame = new TimerTask(){

			@Override
			public void run(){
				//System.out.println("Last frame #: " + currFrameNum);

				displayFrame(frame);
				currFrameNum++;
				double currentAudioPosition = audioPlayer.getPos()/1000;
				int expectedFrameNum = (int)(currentAudioPosition/66.666);
				int syncFrames = expectedFrameNum - currFrameNum;
				if(syncFrames<0)
				{
					try{
						Thread.sleep(Math.abs(syncFrames)*67);
					}
					catch(InterruptedException ie) {
						ie.printStackTrace();
					}
				}
				else
				{
					try{
						is.skip(Constants.BYTES_PER_FRAME*syncFrames);
						currFrameNum +=syncFrames;

					}
					catch (IOException e) {
						e.printStackTrace();
					}
				}
			}
		};

		timer = new Timer();
		timer.schedule(renderFrame,delay,interval);
	}

	public void displayFrame(JFrame frame)
	{
		byte[] bytes = new byte[(int)Constants.BYTES_PER_FRAME];
		int offset = 0;
		int numRead = 0;
		try{
			while (offset < bytes.length && (numRead=is.read(bytes, offset, bytes.length-offset)) >= 0) {
				offset += numRead;
			}
		}
		catch (IOException e) {
			e.printStackTrace();
		}

		int ind = 0;
		for(int y = 0; y < Constants.HEIGHT; y++) {

			for(int x = 0; x < Constants.WIDTH; x++) {

				byte a = 0;
				byte r = bytes[ind];
				byte g = bytes[ind+Constants.PIXELS_PER_FRAME];
				byte b = bytes[ind+Constants.PIXELS_PER_FRAME*2];

				ind++;
				int pix = 0xff000000 | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
				img.setRGB(x,y,pix);
			}
		}
		frame.repaint();
	}

	public void pause(){
		System.out.println("------------------Video curr: " + currFrameNum);
		timer.cancel();
	}

	public void stop(){
		currFrameNum = 0;
		try{
			is = new FileInputStream(vidFile);
		}
		catch (FileNotFoundException e) {
			e.printStackTrace();
		}
		timer.cancel();
	}
}
