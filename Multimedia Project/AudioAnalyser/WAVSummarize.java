package AudioAnalyser;

import Config.Constants;

import javax.sound.sampled.*;
import java.io.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.*;

public class WAVSummarize {


    private static InputStream waveStream;
    private static AudioInputStream audioInputStream;
    private static AudioFormat audioFormat;
    private double MAINTHRESHOLD;


    private static int audioFrameRate;
    private static int audioLengthInSeconds;

    public WAVSummarize(String filename) {
        //TODO set values from external file
        try {
            this.waveStream = new FileInputStream(filename);
            this.audioInputStream = AudioSystem.getAudioInputStream(new BufferedInputStream(waveStream));
            this.audioFormat = audioInputStream.getFormat();
            this.audioFrameRate = Math.round(audioFormat.getFrameRate());
            this.audioLengthInSeconds = Math.round((float) audioInputStream.getFrameLength() / (float) this.audioFrameRate);

        } catch (UnsupportedAudioFileException e1) {
            e1.printStackTrace();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e1) {
            e1.printStackTrace();
        }
    }

   /* public ArrayList<Integer> getTimeStamps() {
        try {
            return calAudioIntensities();
        } catch (Exception ex) {
            ex.printStackTrace();
            return null;
        }
    }
    */

    private double meanSquare(double[] buffer) {
        double ms = 0;
        for (int i = 0; i < buffer.length; i++)
            ms += buffer[i];
        ms /= buffer.length;
        return ms;
    }

    public ArrayList<Integer> processAudio() throws Exception {

        int totalFrames = this.audioFrameRate * this.audioLengthInSeconds;
        int sampleSize = this.audioLengthInSeconds * 2;

        double[] audioData = new double[totalFrames];
        double[] meanSq = new double[sampleSize];

        ArrayList<Integer> secs = new ArrayList<>();


        //read the wav file into a double array byte by byte.
        byte[] b;
        for (int i = 0; i < totalFrames; i++) {
            //2 bytes for each sample of 16 bit.
            b = new byte[2];
            if (audioInputStream.read(b, 0, 2) != -1) {
                //re-encoding endianness
                short num = ByteBuffer.wrap(b).order(ByteOrder.LITTLE_ENDIAN).getShort();
                audioData[i] = Math.abs(num);
            }

        }

        int audioIdx = 0;
        double[] buffer;
        for (int time = 0; time < sampleSize; time++) {
            buffer = new double[Constants.AUDIO_BUFFER_SIZE];
            for (int i = 0; i < Constants.AUDIO_BUFFER_SIZE; i++) {

                buffer[i] = audioData[audioIdx];
                audioIdx++;
            }
            meanSq[time] = meanSquare(buffer);
        }

        //System.out.println(Arrays.toString(meanSq));
        //Standard Deviation Calculation
        double MEAN = meanSquare(meanSq);
        //System.out.println(MEAN);
        //Calculate Variance
        double variance = 0.0;
        for (int i = 0; i < meanSq.length; i++) {
            variance += Math.pow((meanSq[i] - MEAN), 2);
        }
        variance /= sampleSize;
        MAINTHRESHOLD = MEAN;
        //System.out.println(variance);
        double standard_deviation = Math.sqrt(variance);
        //System.out.println(standard_deviation);


        int meanIdx = 0;
        for (int time = 0; time < sampleSize; time += Constants.WINDOWSIZE) {
            buffer = new double[Constants.WINDOWSIZE];
            for (int i = 0; i < Constants.WINDOWSIZE; i++) {
                //System.out.println("I : "+i);
                buffer[i] = meanSq[meanIdx];
               /* System.out.println("Meanidx : "+meanIdx);
                System.out.println("MeanSq Length : "+meanSq.length);
                 */
                meanIdx++;
            }
            double windowThreshold = meanSquare(buffer);
            for (int i = 0; i < Constants.WINDOWSIZE; i++) {
                if (buffer[i] >= MAINTHRESHOLD && buffer[i] >= windowThreshold) {
                    secs.add(time + i);
                    /*try{

                        if(meanSq[time+i+3] >= windowThreshold){
                            secs.add(time + i + 3);
                        }
                    }
                    catch (Exception e){

                    }

                    int temp = time+i+3;
                    System.out.println("Time : "+time);
                    try {
                        while (meanSq[temp] >= windowThreshold) {
                            secs.add(temp);
                            temp++;
                        }

                        System.out.println("Temp : "+temp);
                    }
                    catch(Exception e){

                    }
                    */
                }
            }
        }

        Integer[] arrayList = secs.toArray(new Integer[secs.size()]);
        secs.clear();

        for (int i = 0; i < arrayList.length - 2; i++) {

            if ((arrayList[i + 1] - arrayList[i] == 1)) {

                if ((arrayList[i + 2] - arrayList[i + 1] == 1)) {
                    secs.add(arrayList[i]);
                    /*try{

                        if(arrayList[i+3]- arrayList[i + 2] == 1){
                            secs.add(i + 3);
                        }
                    }
                    catch (Exception e){

                    }
                    */
                }

            }

        }
        return secs;
    }

}
