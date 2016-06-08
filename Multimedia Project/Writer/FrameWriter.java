package Writer;

import java.io.*;
import Config.Constants;
import com.sun.xml.internal.bind.v2.runtime.reflect.opt.Const;

/**
 * Created by sarvesh on 4/30/16.
 */
public class FrameWriter {

    private String inputFile;


    public FrameWriter(String fileName)
    {
        this.inputFile = fileName;
    }

    public void writeFrames(boolean[] frames, File outputFile)
    {
        File file = new File(inputFile);
        int totalFrames = (int)file.length() / (Constants.BYTES_PER_FRAME);

        try {
            InputStream in = new FileInputStream(file);
            OutputStream out = new FileOutputStream(outputFile);

            for (int i = 0; i < frames.length; i++)
            {

                byte[] byte_array = new byte[Constants.BYTES_PER_FRAME];
                int offset = 0;
                int numberRead;
                while(offset < byte_array.length && (numberRead = in.read(byte_array, offset, byte_array.length - offset)) >= 0)
                {
                    offset += numberRead;
                }
                if(frames[i])
                {
                    out.write(byte_array);
                }
            }
            out.close();

        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }


    public void writeFrames(int frameNumber, File outputFile)
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

        writeFrames(frames, outputFile);
    }

}
