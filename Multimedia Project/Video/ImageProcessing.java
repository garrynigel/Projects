package Video;

import Config.Constants;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;
import org.opencv.core.Mat;


public class ImageProcessing {
    private InputStream videoInputStream;

    private ArrayList<Integer> keyFrames;

    private double THRESHOLD = Constants.IMAGE_SUMMARY_THRESHOLD_PERCENT * Constants.MAX_HISTOGRAM_DIFFERENCE;
    private Histogram hist;
    public static int keyFrameSize;

    public ImageProcessing(String fileName) {
        try {
            videoInputStream = new FileInputStream(fileName);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }


        this.keyFrames = new ArrayList<>();
        this.hist = new Histogram();
    }

    public ArrayList<Integer> generateKeyFrames() {


        List<Mat> referenceHist = null;
        List<Mat> currentHist;
        for (int i = 0; i < Constants.TOTAL_FRAMES; i++) {

            byte[] bytes = new byte[Constants.BYTES_PER_FRAME];
            int offset = 0;
            int numRead;
            try {
                while (offset < bytes.length && (numRead = videoInputStream.read(bytes, offset, bytes.length - offset)) >= 0) {
                    offset += numRead;
                }
            } catch (IOException e) {
                e.printStackTrace();
            }

            if (referenceHist == null) {
                referenceHist = hist.getHistogram(bytes);
                keyFrames.add(i);
                continue;
            }
            else {
                currentHist = hist.getHistogram(bytes);
            }

            if (hist.getDifference(currentHist, referenceHist) >= THRESHOLD) {
                System.out.println("Threshold crossed.");
                referenceHist = currentHist;
                keyFrames.add(i);
            }
        }

        keyFrameSize = keyFrames.size();
        return keyFrames;
    }
}
