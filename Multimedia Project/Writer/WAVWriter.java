package Writer;

import Config.Constants;

import javax.sound.sampled.AudioFileFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.UnsupportedAudioFileException;
import java.io.*;

public class WAVWriter {

    private int BUFFER;
    AudioInputStream audStream;
    InputStream ipStream;
    String inFile;

    public WAVWriter(String Filename){
        this.inFile = Filename;

        try {
            ipStream = new FileInputStream(inFile);
            audStream  = AudioSystem.getAudioInputStream(new BufferedInputStream(ipStream));
            this.BUFFER = Constants.AUDIO_SAMPLES_PER_VIDEO_FRAME * audStream.getFormat().getFrameSize();

        }
        catch (FileNotFoundException e) {
            e.printStackTrace();
        }
        catch (UnsupportedAudioFileException e) {
            e.printStackTrace();
        }
        catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void genAudio(boolean[] frames, File outFile){

        try {

            int framesGen=0;
            for(int i = 0; i < frames.length;i++){
                if(frames[i])
                    framesGen++;
            }

            byte[] byteArray = new byte[BUFFER*framesGen];
            int offset = 0;

            for(int i = 0; i < frames.length;i++){

                if(frames[i]){
                    audStream.read(byteArray,offset,BUFFER);
                    offset+=BUFFER;
                }
                else {
                    audStream.skip(BUFFER);
                }
            }

            InputStream is = new ByteArrayInputStream(byteArray);
            AudioInputStream outStream = new AudioInputStream(is, audStream.getFormat(), BUFFER * framesGen);
            AudioSystem.write(outStream, AudioFileFormat.Type.WAVE, outFile);
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    public void genAudio(int frameNumber, File outputFile)
    {
        boolean[] frames = new boolean[Constants.TOTAL_FRAMES];
        int frameCount;

        if(frameNumber < 150)
        {
            frameCount = 0;
        }
        else
        {
            frameCount = frameNumber - 150;
        }

        for (; frameCount < frameNumber + 150 && frameCount < Constants.TOTAL_FRAMES; frameCount++ )
        {
            frames[frameCount] = true;
        }

        genAudio(frames, outputFile);
    }
}
