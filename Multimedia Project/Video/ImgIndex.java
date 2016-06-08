package Video;



import org.opencv.core.*;
import Bucket.*;
import Config.Constants;
import javax.imageio.*;
import Config.Constants;
import java.awt.image.BufferedImage;
import java.io.*;
import java.util.ArrayList;
import java.util.*;

/**
 * Created by garrydmello on 4/28/16.
 */
public class ImgIndex {


    private File inFile;
    private String audioFile;
    private String videoFile;
    private FileInputStream inFileStream;
    private int TOTAL_FRAMES;

    private ArrayList<Double> metricList = new ArrayList<>();
    private ArrayList<List<Mat>> histList = new ArrayList<>();
    double minHist;
    double maxHist;

    private ArrayList<Bucket> clusters = new ArrayList<>();
    double rangeCluster;

    // Store in ImageLoader
    private int[] bucketMap = new int[4500];


    public ImgIndex(String videoFile, String audioFile){

        try{
            this.audioFile = audioFile;
            this.videoFile = videoFile;
            inFile = new File(videoFile);
            inFileStream = new FileInputStream(inFile);
            this.TOTAL_FRAMES = (int)inFile.length()/Constants.BYTES_PER_FRAME;


        }
        catch(FileNotFoundException e){
            e.printStackTrace();
        }


    }

    public String getAudioFile()
    {
        return this.audioFile;
    }

    public String getVideoFile()
    {
        return this.videoFile;
    }

    public byte[] getNextFrame(){

        byte[] bytes = new byte[Constants.BYTES_PER_FRAME];

        int offset = 0;
        int numRead;

        try {
            while (offset < bytes.length && (numRead = this.inFileStream.read(bytes, offset, bytes.length - offset)) >= 0) {
                offset += numRead;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        return bytes;
    }

    public ArrayList<List<Mat>> getHistList(){return this.histList;}

    public ArrayList<Bucket> getClusters(){

        return this.clusters;

    }

    public void genHistList(){

           try {


               byte[] bytes;
                int count = 0;
               for(int i = 0; i< TOTAL_FRAMES;i++){

                   count += Constants.BYTES_PER_FRAME;
                   bytes = new byte[Constants.BYTES_PER_FRAME];
                   inFileStream.read(bytes,0,Constants.BYTES_PER_FRAME);

                   Histogram hist = new Histogram();
                   List<Mat> chanHist = hist.getHistogram(bytes);
                   histList.add(i,chanHist);


                   Mat chanR = chanHist.get(0);
                   Mat chanG = chanHist.get(1);
                   Mat chanB = chanHist.get(2);



                   double totalR = 0.0;
                   double totalG = 0.0;
                   double totalB = 0.0;


                   double[] tempR;
                   double[] tempG;
                   double[] tempB;


                   for(int j = 0 ; j < chanR.rows() ; j++){

                       tempR = chanR.get(j,0);
                       totalR += (tempR[0] * (j+1));

                       tempG = chanG.get(j,0);
                       totalG += (tempG[0] * (j+1));

                       tempB =chanB.get(j,0);
                       totalB += (tempB[0] * (j+1));


                   }

                   double AvgHist = Math.round((totalR+totalG+totalB)/(256*3));

                   metricList.add(AvgHist);



               }




           }catch (Exception e) {
               e.printStackTrace();

           }





    }

    public void genClusters(){

        minHist = Collections.min(metricList);

        maxHist = Collections.max(metricList);

        System.out.println("Min :"+minHist);
        System.out.println("Max :"+maxHist);

        double diff = maxHist - minHist;

        System.out.println(diff);

        rangeCluster = Math.round(diff/Constants.NUM_BUCKETS);

        //System.out.println("Range :"+rangeCluster);

        //Integer[] arrayList = metricList.toArray(new Integer[metricList.size()]);

        int metric = (int)Math.round(minHist);

        // Create Clusters based on range and NUM_BUCKETS

        for(int i = 0 ; i < Constants.NUM_BUCKETS;i++){

            Bucket buck = new Bucket(i);
            metric+=(rangeCluster+1);
            buck.setRange(metric);
            clusters.add(buck);

        }

        for(int i=0;i < metricList.size();i++){


            double histFrame = metricList.get(i);
            int bucket_id = (int)Math.floor((histFrame - minHist)/(rangeCluster+1));
            Double[] drop = new Double[2];
            // Store the frame Info in the drop
            drop[0] = (double)i;
            drop[1] = histFrame;

            // Store the frame Info in bucketmap

            bucketMap[i] = bucket_id;

            Bucket buck = clusters.get(bucket_id);
            buck.addDrop(drop);

        }



    }


    public void genFiles(){

             BufferedImage bi;

        try {
            inFileStream = new FileInputStream(inFile);
            this.TOTAL_FRAMES = (int)inFile.length()/Constants.BYTES_PER_FRAME;
             } catch (FileNotFoundException e) {
                  e.printStackTrace();
             }




             for(int i = 0; i< TOTAL_FRAMES;i++){


                 int bucket_id = bucketMap[i];
                 String dirPath = Constants.CLUSTER_PATH+"Cluster"+bucket_id+"";

                 try{

                     File dir = new File(dirPath);

                     dir.mkdir();

                 }
                 catch(Exception ex){

                     ex.printStackTrace();
                     System.exit(1);

                 }

                 String outFileName = dirPath+"/img"+i+".png";

                 File outFile = new File(outFileName);
                 bi = new BufferedImage(Constants.WIDTH, Constants.HEIGHT, BufferedImage.TYPE_INT_RGB);

                 byte[] bytes = getNextFrame();
                 int ind = 0;
                 for(int y = 0; y < Constants.HEIGHT; y++) {

                     for(int x = 0; x < Constants.WIDTH; x++) {

                         byte a = 0;
                         byte r = bytes[ind];
                         byte g = bytes[ind+Constants.PIXELS_PER_FRAME];
                         byte b = bytes[ind+Constants.PIXELS_PER_FRAME*2];

                         int pix = 0xff000000 | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
                         bi.setRGB(x,y,pix);
                         ind++;
                     }
                 }


                 try{
                     ImageIO.write(bi, "png", outFile);
                     //bi.flush();
                 }catch(IOException ioex) {
                     ioex.printStackTrace();
                 }



             }



    }

}
