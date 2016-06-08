package Video;

import Bucket.Bucket;
import Config.Constants;
import Writer.FrameWriter;
import Writer.WAVWriter;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;

import java.io.*;
import java.util.ArrayList;
import java.util.List;

import javax.imageio.ImageIO;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;

import static org.opencv.core.Core.absdiff;
import static org.opencv.core.Core.sumElems;

/**
 * Created by garrydmello on 4/30/16.
 */
public class QueryImg {

    private static int CHANNELS = 3;
    static int IMGWIDTH = 1280;
    static int IMGHEIGHT = 720;
    double histVal;
    List<Mat> imgHist;





    private static BufferedImage resizeImage(BufferedImage originalImage, int type){
        BufferedImage resizedImage = new BufferedImage(Constants.WIDTH, Constants.HEIGHT, type);
        Graphics2D g = resizedImage.createGraphics();
        g.drawImage(originalImage, 0, 0, Constants.WIDTH, Constants.HEIGHT, null);
        g.dispose();

        return resizedImage;
    }

    public double getDifference(List<Mat> histogramA, List<Mat> histogramB) {
        double value = 0.0;
        for (int i = 0; i < CHANNELS; i++) {
            Mat diff = new Mat();
            absdiff(histogramA.get(i), histogramB.get(i), diff);
            value += sumElems(diff).val[0];
        }
        return value / 3.0;
    }

    public void genHistImg(File ipFile) {

        try {

            InputStream is = new FileInputStream(ipFile);

            //FileWriter out
            byte[] bytes = new byte[Math.round(ipFile.length())];
            is.read(bytes, 0, Math.round(ipFile.length()));

            System.out.println("ipFile length"+ipFile.length());

            Histogram hist = new Histogram();

            imgHist = hist.getHistogram(bytes,IMGWIDTH,IMGHEIGHT);


            Mat chanR = imgHist.get(0);
            Mat chanG = imgHist.get(1);
            Mat chanB = imgHist.get(2);

            Core.multiply(chanR,new Scalar((1/(2.66*2.66))),chanR);
            Core.multiply(chanG,new Scalar((1/(2.66*2.66))),chanG);
            Core.multiply(chanB,new Scalar((1/(2.66*2.66))),chanB);


            double totalR = 0.0;
            double totalG = 0.0;
            double totalB = 0.0;


            double[] tempR;
            double[] tempG;
            double[] tempB;


            for (int j = 0; j < chanR.rows(); j++) {

                tempR = chanR.get(j, 0);
                totalR += (tempR[0] * (j+1));

                tempG = chanG.get(j, 0);
                totalG += (tempG[0] * (j+1));

                tempB = chanB.get(j, 0);
                totalB += (tempB[0] * (j+1));


            }

            histVal = ((totalR + totalG + totalB) / (256 * 3));


        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void searchImg(String fileName,ImgIndex img){


          try {
              File ipFile = new File(fileName);
              genHistImg(ipFile);

              ArrayList<Bucket> clusters = img.getClusters();
              histVal = Math.round(histVal);

              ArrayList<List<Mat>> histList = img.getHistList();

              int bucket_id = (int)Math.floor((histVal - img.minHist)/(img.rangeCluster+1));
              System.out.println("histVal"+histVal);
              System.out.println("Bucket Number"+bucket_id);

              Bucket buck = clusters.get(bucket_id);

              double minDiffHist = Double.MAX_VALUE;
              int frameSearch = 0;
              ArrayList<Double[]> bucketList = buck.getBucketList();

              for(int i = 0; i < bucketList.size();i++){
                  Double[] drop = bucketList.get(i);
                  int frame = (int)Math.round(drop[0]);

                  List<Mat> frameHist = histList.get(frame);

                  double histDiff = getDifference(frameHist,imgHist);

                  if(histDiff < minDiffHist){
                      frameSearch = frame;
                      minDiffHist = histDiff;

                  }


              }

              System.out.println("Frame No : "+frameSearch);
              System.out.println("Min Diff : "+minDiffHist);

              FrameWriter writeImage = new FrameWriter(img.getVideoFile());
              writeImage.writeFrames(frameSearch, new File(Constants.SEARCHED_VIDEO));

              WAVWriter writeAudio = new WAVWriter(img.getAudioFile());
              writeAudio.genAudio(frameSearch, new File(Constants.SEARCHED_AUDIO));

          }catch(Exception e){

              e.printStackTrace();

        }


    }



}
