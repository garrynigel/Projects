
import java.awt.*;
import java.awt.image.*;
import java.io.*;
import javax.swing.*;


public class imageReader {

	JFrame frame;
	JLabel lbIm1;
	JLabel lbIm2;
	BufferedImage inImg;
	BufferedImage outImg;

	int width = 352;
	int height = 288;

	public float[] yVals = new float[(int)(width * height)];

	public float[] uVals = new float[(int)(width * height)];

	public float[] vVals = new float[(int)(width * height)];

  public void readIms(String[] args){


		inImg = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);

		try {
			File file = new File(args[0]);
			InputStream is = new FileInputStream(file);

			long len = file.length();
			byte[] bytes = new byte[(int)len];

			int offset = 0;
			int numRead = 0;

			while (offset < bytes.length && (numRead=is.read(bytes, offset, bytes.length-offset)) >= 0) {
				offset += numRead;
			}

			int ind = 0;
			for(int y = 0; y < height; y++){

				for(int x = 0; x < width; x++){

					byte a = 0;
					byte r = bytes[ind];
					byte g = bytes[ind+height*width];
					byte b = bytes[ind+height*width*2];

					int pix = 0xff000000 | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
					//int pix = ((a << 24) + (r << 16) + (g << 8) + b);
					inImg.setRGB(x,y,pix);
					ind++;
				}
			}


		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}

	}

  public void rgb2Yuv(){


    int ind = 0;
		for(int y = 0; y < height; y++){

			for(int x = 0; x < width; x++){

					int pixel = inImg.getRGB(x,y);
					int a = (pixel >> 24) & 0xff;
	        int r = (pixel >> 16) & 0xff;
	        int g = (pixel >> 8) & 0xff;
	        int b = (pixel) & 0xff;
          /*

						 Calculate Y U V values

					*/
					yVals[ind] = (float)(0.299*r)+(float)(0.587*g)+(float)(0.114*b);
					uVals[ind] = (float)(0.596*r)+(float)(-0.274*g)+(float)(-0.322*b);
					vVals[ind] = (float)(0.211*r)+(float)(-0.523*g)+(float)(0.312*b);

 				 	ind++;
			}
		}

	}

	public float[] subSamp(float[] arr ,int samp){

    int indx = 0;
		int x = 0;
		int n = height*width;
		int start;
		int end;
		if(samp == 1){
			return arr;
		}

		for(int y = 0; y < height; y++){

			x = 0;
			while(x < width){

			  start = x+indx;
				if( (x+samp) >= width )
				{
          for(int i = x+1 ; i < width ; i++)
					{
						arr[start+1] = arr[start];
						start++;
						x++;

					}

					x = x+samp;
				}
				else
				{
			   	 end = x+indx+samp;

			   	float interval = (float)(arr[end]-arr[start])/samp;

			   	for(int i = 1 ;i < samp;i++){
             arr[start+i] = arr[start+i-1]+interval;
				  }

				  x = x + samp;

	      }

	    }
			  indx = indx+width;
   }
	 return arr;
}

	public void conv2RGB(){

		int indx = 0;

		outImg = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);

		for(int y = 0; y < height; y++){

			for(int x = 0; x < width; x++){

	    		int r = (int)((1.000*yVals[indx]) + (0.956*uVals[indx])+(0.621*vVals[indx]));

		    	int g = (int)((1.000*yVals[indx]) + (-0.272*uVals[indx])+(-0.647*vVals[indx]));

		    	int b = (int)((1.000*yVals[indx]) + (-1.106*uVals[indx])+(1.703*vVals[indx]));


				  int pix = 0xff000000 | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);

				  outImg.setRGB(x,y,pix);

					indx++;

		  }
  	}


	}
	public int qtizeVal(int chann, int[] qArr, int q){

     for(int indx = 1; indx < q;indx++){

			 if ((chann > qArr[indx-1]) && (chann < qArr[indx]))
			 {
				 int halfVal = (int)((qArr[indx]+qArr[indx-1])/2);
				 if (chann < halfVal)
				 {
					 return qArr[indx-1];
				 }
				 else
				 {
					 return qArr[indx];
				 }
			 }

			 if(chann == qArr[indx-1])
			   return qArr[indx-1];

			 if(chann == qArr[indx])
			   return qArr[indx];

		 }
     if(chann > qArr[q-1])
		 {
			 return qArr[q-1];
		 }
		 return 0;
	}

	public void qtizeChann(int q){

		int qCounter = (int)(256/q);

		int quantArr[] = new int[q];

		quantArr[0] = 0;
		for(int indx = 1;indx < q;indx++){
			quantArr[indx] = quantArr[indx-1] + qCounter;
		}
		for(int y = 0; y < height; y++){

			for(int x = 0; x < width; x++){

				int pixel = outImg.getRGB(x,y);
				int a = (pixel >> 24) & 0xff;
				int r = (pixel >> 16) & 0xff;
				int g = (pixel >> 8) & 0xff;
				int b = (pixel) & 0xff;



         int newr = qtizeVal(r, quantArr,q);
				 int newg = qtizeVal(g, quantArr,q);
				 int newb = qtizeVal(b, quantArr,q);

				 int pix = 0xff000000 | ((newr & 0xff) << 16) | ((newg & 0xff) << 8) | (newb & 0xff);

				 outImg.setRGB(x,y,pix);
			 }
    }


	}


	public void showIms(){
		/*int width = Integer.parseInt(args[1]);
		int height = Integer.parseInt(args[2]);
		*/
		// Use labels to display the images
		frame = new JFrame();
		GridBagLayout gLayout = new GridBagLayout();
		frame.getContentPane().setLayout(gLayout);

		JLabel lbText1 = new JLabel("Original image (Left)");
		lbText1.setHorizontalAlignment(SwingConstants.CENTER);
		JLabel lbText2 = new JLabel("Image after modification (Right)");
		lbText2.setHorizontalAlignment(SwingConstants.CENTER);
		lbIm1 = new JLabel(new ImageIcon(inImg));
		lbIm2 = new JLabel(new ImageIcon(outImg));

		GridBagConstraints c = new GridBagConstraints();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.anchor = GridBagConstraints.CENTER;
		c.weightx = 0.5;
		c.gridx = 0;
		c.gridy = 0;
		frame.getContentPane().add(lbText1, c);

		c.fill = GridBagConstraints.HORIZONTAL;
		c.anchor = GridBagConstraints.CENTER;
		c.weightx = 0.5;
		c.gridx = 1;
		c.gridy = 0;
		frame.getContentPane().add(lbText2, c);

		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridx = 0;
		c.gridy = 1;
		frame.getContentPane().add(lbIm1, c);

		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridx = 1;
		c.gridy = 1;
		frame.getContentPane().add(lbIm2, c);

		frame.pack();
		frame.setVisible(true);
	}

	public static void main(String[] args) {
		int sampY, sampU, sampV;
		int []err = new int[(288*352)];

		if(args.length < 5)
    {
        System.out.println("Arguments passed are incorrect");
        System.exit(0);
    }
		imageReader ren = new imageReader();
		ren.readIms(args);
		ren.rgb2Yuv();

		int indx = 0;

		/*for(int y = 0; y < 2; y++){

		for(int x = 0; x < 352; x++){

					 System.out.print(ren.uVals[indx]+ " ");

			 indx++;
			}
			System.out.println();
		}
		*/

		ren.subSamp(ren.yVals,Integer.parseInt(args[1]));
		ren.subSamp(ren.uVals,Integer.parseInt(args[2]));
		ren.subSamp(ren.vVals,Integer.parseInt(args[3]));
    indx = 0;

		/*for(int y = 0; y < 2; y++){

 	 	for(int x = 0; x < 352; x++){

           System.out.print(ren.uVals[indx]+ " ");

       indx++;
		 	}
			System.out.println();
	}
	*/
		ren.conv2RGB();





		int q = Integer.parseInt(args[4]);


   ren.qtizeChann(q);

/*
	 for(int y = 0; y < 288; y++){

	 	for(int x = 0; x < 352; x++){


	 							Color imgInColor = new Color(ren.inImg.getRGB(x,y));
	 							Color imgOpColor = new Color(ren.outImg.getRGB(x,y));
	 							err[indx] = Math.abs((imgOpColor.getRed()-imgInColor.getRed()))
	 													+Math.abs((imgOpColor.getBlue()-imgInColor.getBlue()))
	 													+Math.abs((imgOpColor.getGreen()-imgInColor.getGreen()));

	 							indx++;

	 	}
	 }

	double errorLoss = 0.0;
	 for(int y = 0; y < 288; y++){

	 	errorLoss = errorLoss + err[y];

	 }

	 System.out.println("Average Error per pixel "+(errorLoss/(288*352)));
*/


		ren.showIms();
	}


}
