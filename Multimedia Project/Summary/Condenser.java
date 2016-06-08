package Summary;

import Config.Constants;
import Writer.*;
import Video.*;
import AudioAnalyser.*;

import java.io.File;
import java.util.ArrayList;

/**
 * Created by sarvesh on 4/30/16.
 */
public class Condenser {

    private String IMAGE_FILE;
    private String AUDIO_FILE;

    private String OUTPUT_FILE;



    public Condenser(String imageFile, String audioFile)
    {
        this.IMAGE_FILE = imageFile;
        this.AUDIO_FILE = audioFile;
    }

    public void condense(String outFile)
    {
        System.out.printf("Starting summary");
        this.OUTPUT_FILE = outFile;

        boolean[] framesRequired = new boolean[Constants.TOTAL_FRAMES];

        ImageProcessing condensedImage = new ImageProcessing(IMAGE_FILE);
        ArrayList<Integer> keyImageFrames = condensedImage.generateKeyFrames();
        System.out.printf("Video len: " + keyImageFrames.size());
        for (int frame: keyImageFrames)
        {
            framesRequired[frame] = true;
        }

        System.out.printf("Video done");

        WAVSummarize condensedAudio = new WAVSummarize(AUDIO_FILE);
        ArrayList<Integer> keyAudioFrames = null;
        try {
            keyAudioFrames = condensedAudio.processAudio();
        } catch (Exception e) {
            e.printStackTrace();
        }
        System.out.println(keyAudioFrames);
        System.out.printf("Audio len: " + keyImageFrames.size());
        for (int i = 0; i < keyAudioFrames.size(); i++)
        {
            int frameTime = keyAudioFrames.get(i);
            framesRequired[(int)Math.floor(frameTime * 7.5)] = true;
        }

        int frameIndex = 16;

        //Always consider the first 15 frames i.e the first second
        for (int i = 0; i < frameIndex; i++)
        {
            framesRequired[i] = true;
        }

        //If Frame has been identified in either Image or AudioAnalyser processing keep the frames around it.
        while( frameIndex < framesRequired.length)
        {
            if(!framesRequired[frameIndex])
            {
                //If frame is not a key frame check next frame
                frameIndex += 1;
            }
            else
            {
                for(int j = frameIndex - 14; j <= frameIndex + 15 && j <framesRequired.length; j++)
                {
                    framesRequired[j] = true;
                }
                frameIndex += 16;
            }

        }

        FrameWriter writeImage = new FrameWriter(IMAGE_FILE);
        writeImage.writeFrames(framesRequired, new File(Constants.SUMMARIZED_VIDEO));

        WAVWriter writeAudio = new WAVWriter(AUDIO_FILE);
        writeAudio.genAudio(framesRequired, new File(Constants.SUMMARIZED_AUDIO));
    }

}
