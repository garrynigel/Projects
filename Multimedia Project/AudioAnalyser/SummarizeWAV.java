package AudioAnalyser;

import java.io.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.UnsupportedAudioFileException;

/**
 * Created by sarvesh on 5/2/16.
 */
public class SummarizeWAV {
    private InputStream waveStream;
    private AudioInputStream audioInputStream;
    private int AUDIO_FRAMERATE;
    private int THRESHOLD = 500;
    private int durationInSec;
    private static float[] audioData ;
    private int stepsPerSec = 2;


    public SummarizeWAV(String fileName)
    {
        try {
            waveStream = new FileInputStream(fileName);
            audioInputStream = AudioSystem.getAudioInputStream(new BufferedInputStream(waveStream));
            AUDIO_FRAMERATE = (int)audioInputStream.getFormat().getFrameRate();
            durationInSec = (int)(audioInputStream.getFrameLength() / AUDIO_FRAMERATE);
            audioData = new float[AUDIO_FRAMERATE * durationInSec];
        }
        catch (UnsupportedAudioFileException | FileNotFoundException e1) {
            e1.printStackTrace();
        } catch (IOException e1) {
            e1.printStackTrace();
        }
        System.out.println("*************" + audioInputStream.getFormat().getFrameSize());

    }

    public double meanSquare(double[] buffer){
        double ms = 0;
        for (int i = 0; i < buffer.length; i++)
            ms  += buffer[i];
        ms /= buffer.length;
        return ms;
    }

    public void generateAudioDataFromIS() {
        for (int i = 0; i< audioData.length; i++) {

            byte[] b = new byte[2];
            try {
                if (audioInputStream.read(b, 0, 2) != -1) {
                    short num = ByteBuffer.wrap(b).order(ByteOrder.LITTLE_ENDIAN).getShort();
                    audioData[i] = Math.abs(num);
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public void normalizeAudioData(double[] audioDataPerStep, double[] audioMeanPer10Sec) {

        int steps = audioDataPerStep.length;
        int stepSize = AUDIO_FRAMERATE/stepsPerSec;
        int data10SecCount = 0;
        for(int i = 0; i < steps;i++)
        {
            double[] buffer = new double[stepSize];
            int count = 0;
            int j = i*stepSize;
            while(count < stepSize)
                buffer[count++] = audioData[j++];

            audioDataPerStep[i] = meanSquare(buffer);
            if(i%20 ==0 && i!=0) {
                int mean = 0;
                for(int k =i-20; k<i ; k++) {
                    mean+=audioDataPerStep[k];
                }
                mean = mean/20;
                audioMeanPer10Sec[data10SecCount++] = mean;
            }
        }

    }

    public ArrayList<Integer> processAudio() {

        ArrayList<Integer> list = new ArrayList<>();
        int steps = durationInSec * stepsPerSec;
        double[] audioDataPerStep = new double[steps];
        //THRESHOLD = (int)meanSquare(audioDataPerStep);

        double[] audioMeanPer10Sec = new double[durationInSec/10];
        generateAudioDataFromIS();
        normalizeAudioData(audioDataPerStep,audioMeanPer10Sec);
        System.out.println("Threshold: " + THRESHOLD);
        THRESHOLD = (int)meanSquare(audioDataPerStep);
        System.out.println("Threshold: " + THRESHOLD);

        for(int i=0 ; i<audioDataPerStep.length -2;i++)
        {
            if (audioDataPerStep[i] > THRESHOLD && audioDataPerStep[i+1] > THRESHOLD && audioDataPerStep[i+2] > THRESHOLD )
            {
                if (audioDataPerStep[i] > audioMeanPer10Sec[i/20] && audioDataPerStep[i+1] > audioMeanPer10Sec[i/20]  && audioDataPerStep[i+2] > audioMeanPer10Sec[i/20])
                {
                    list.add(i);
                }
            }
        }

        return list;
    }
}
