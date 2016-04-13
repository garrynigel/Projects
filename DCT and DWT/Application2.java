
import java.awt.*;
import java.awt.image.*;
import java.io.*;
import javax.swing.*;


public class Application2 {

	JFrame frame;
	JLabel lbIm1;
	JLabel lbIm2;
	double[][] r = new double[512][512];
	double[][] g = new double[512][512];
	double[][] b = new double[512][512];
	double[][] coeffDCT = new double[8][8];
	double[][] coeffDCTT = new double[8][8];

	double[][] rInter = new double[512][512];
	double[][] gInter = new double[512][512];
	double[][] bInter= new double[512][512];

	double[][] rDCT = new double[512][512];
	double[][] gDCT = new double[512][512];
	double[][] bDCT= new double[512][512];

	double[][] rJPEG = new double[512][512];
	double[][] gJPEG = new double[512][512];
	double[][] bJPEG= new double[512][512];

	BufferedImage inImg;
	BufferedImage jpegImg;

	public void convert2rgb(String[] args){
		int width = 512;
		int height = 512;

		inImg = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
		jpegImg = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);

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

					int a = 0;
					int r = (int)bytes[ind];
					int g = (int)bytes[ind+height*width];
					int b = (int)bytes[ind+height*width*2];

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

	public void displayImg(){
		// Use labels to display the images
		frame = new JFrame();
		GridBagLayout gLayout = new GridBagLayout();
		frame.getContentPane().setLayout(gLayout);

		JLabel lbText1 = new JLabel("Original image (Left)");
		lbText1.setHorizontalAlignment(SwingConstants.CENTER);
		JLabel lbText2 = new JLabel("Image after modification (Right)");
		lbText2.setHorizontalAlignment(SwingConstants.CENTER);
		lbIm1 = new JLabel(new ImageIcon(inImg));
		lbIm2 = new JLabel(new ImageIcon(jpegImg));

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

	public void updateCoefficients(double matrix[][], int count){

		if(matrix == null || matrix.length == 0 || matrix[0].length == 0) return;

	         int m = matrix.length, n = matrix[0].length;
	         int j = 0;
	         for(int i = 0; i < m + n - 1; i++){
	             if(i % 2 == 1){
	                 for(int y = Math.min(i, n - 1); y >= Math.max(0, i - m + 1); y--){
	                      if(j > count)
												  matrix[y][i-y] = 0;
												j++;
	                 }
	             } else{
	                 for(int x = Math.min(i, m - 1); x >= Math.max(0, i - n + 1); x--){
										 if(j > count)
											 matrix[i-x][x] = 0;
											j++;
	                 }
	             }
	         }
	}


	public final void initDCTCoeff()
	 {
	    final int N = coeffDCT.length;
	    final double value1 = 1/Math.sqrt(8.0);
			final double value2 = 1/2.0;

			for(int i = 0 ;i<8;i++){
				for(int j = 0;j<8;j++){

						coeffDCT[i][j] = i > 0 ? (value2 * Math.cos(((2*j + 1) * i * Math.PI)/16.0)):value1;

					}
				}

				for(int i = 0 ;i<8;i++){
					for(int j = 0;j<8;j++){

							coeffDCTT[i][j] = coeffDCT[j][i];

						}
					}

	}

	public void stdizeSub(){

		for(int i = 0 ;i<512;i++){
			for(int j = 0;j<512;j++){

					r[i][j] -= 128.0 ;
					g[i][j] -= 128.0 ;
					b[i][j] -= 128.0 ;
				}
			}

	}

	public void stdizeAdd(){

		for(int i = 0 ;i<512;i++){
			for(int j = 0;j<512;j++){

					r[i][j] += 128.0 ;
					g[i][j] += 128.0 ;
					b[i][j] += 128.0 ;
				}
			}

			for(int i = 0 ;i<512;i++){
				for(int j = 0;j<512;j++){

						rJPEG[i][j] += 128.0 ;
						gJPEG[i][j] += 128.0 ;
						bJPEG[i][j] += 128.0 ;
					}
				}

	}
	public void channelValues(){
		    for(int i = 0 ;i<512;i++){
					for(int j = 0;j<512;j++){

						Color color = new Color(inImg.getRGB(i,j));

							r[i][j] = (color.getRed());
							g[i][j] = (color.getGreen());
							b[i][j] = (color.getBlue());
						}
					}
	}



  public void applyDCT(){


		for(int x = 0; x < 512;x+=8){
 		 for(int y = 0; y < 512;y+=8){

			 for(int i = 0; i<8;i++){
				    for(int j = 0;j<8;j++){
					       for(int k = 0;k<8;k++){

						           rInter[i+x][j+y] += coeffDCT[i][k] * r[k+x][j+y];
	 					           gInter[i+x][j+y] += coeffDCT[i][k] * g[k+x][j+y];
	 					           bInter[i+x][j+y] += coeffDCT[i][k] * b[k+x][j+y];

				         }
 		       }
 	     }

			 for(int i = 0; i<8;i++){
				    for(int j = 0;j<8;j++){
					       for(int k = 0;k<8;k++){

						           rDCT[i+x][j+y] += rInter[i+x][k+y] * coeffDCTT[k][j];
	 					           gDCT[i+x][j+y] += gInter[i+x][k+y] * coeffDCTT[k][j];
	 					           bDCT[i+x][j+y] += bInter[i+x][k+y] * coeffDCTT[k][j];

				         }
 		       }
 	     }


	  }
  }
}
	public void applyIDCT(){


		for(int i = 0 ;i<512;i++){
			for(int j = 0;j<512;j++){

					rInter[i][j] = 0.0 ;
					gInter[i][j] = 0.0 ;
					bInter[i][j] = 0.0 ;
				}
			}

		for(int x = 0; x < 512;x+=8){
 		 for(int y = 0; y < 512;y+=8){

			 for(int i = 0; i<8;i++){
				    for(int j = 0;j<8;j++){
					       for(int k = 0;k<8;k++){

						           rInter[i+x][j+y] += coeffDCT[i][k] * rDCT[k+x][j+y];
	 					           gInter[i+x][j+y] += coeffDCT[i][k] * gDCT[k+x][j+y];
	 					           bInter[i+x][j+y] += coeffDCT[i][k] * bDCT[k+x][j+y];

				         }
 		       }
 	     }

			 for(int i = 0; i<8;i++){
				    for(int j = 0;j<8;j++){
					       for(int k = 0;k<8;k++){

						           rJPEG[i+x][j+y] += rInter[i+x][k+y] * coeffDCTT[k][j];
	 					           gJPEG[i+x][j+y] += gInter[i+x][k+y] * coeffDCTT[k][j];
	 					           bJPEG[i+x][j+y] += bInter[i+x][k+y] * coeffDCTT[k][j];

				         }
 		       }
 	     }


	  }
  }
	}


	public void displayDCT(double[][] dct){
		for(int x = 0; x < 8;x++){
			for(int y = 0; y < 8; y++){

        System.out.print((int)dct[x][y]+" ");
			}
			System.out.println();
		}
	}

	public static void main(String[] args) {

		//double[][] newArray = new double[8][8];
		Application2 ren1 = new Application2();
		ren1.initDCTCoeff();
		ren1.convert2rgb(args);
		ren1.channelValues();


    System.out.println("Before");
	  ren1.displayDCT(ren1.r);


 	 	ren1.stdizeSub();

		ren1.applyDCT();
//		ren1.displayDCT(ren1.coeffDCT);

//		System.out.println();
//		ren1.displayDCT(ren1.coeffDCTT);

		ren1.applyIDCT();

		ren1.stdizeAdd();
/*


		for(int x = 0; x < 512;x+=8){
			for(int y = 0; y < 512;y+=8){

				ren.applyDCT(x,y);

			}
		}

	int noCoeff = Integer.parseInt(args[1]);
  int m = (int)Math.round(noCoeff/4096.0);
	ren.updateCoefficients(ren.coeffDCT,m);


		for(int x = 0; x < 512;x+=8){
			for(int y = 0; y < 512;y+=8){

				ren.applyIDCT(x,y);

			}
		}
*/
    System.out.println("Red Channel DCT");
		System.out.println("After");
    ren1.displayDCT(ren1.rJPEG);



	for(int y = 0; y < 512; y++){

			for(int x = 0; x < 512; x++){

        int r = (int)(ren1.rJPEG[y][x]);
				int g = (int)(ren1.gJPEG[y][x]);
				int b = (int)(ren1.bJPEG[y][x]);
				int pix = 0xff000000 | ((r & 0xff)<< 16) | (( g & 0xff) << 8) | ( b & 0xff);
				//int pix = ((a << 24) + (r << 16) + (g << 8) + b);
				ren1.jpegImg.setRGB(y,x,pix);
			}
		}
		ren1.displayImg();

	}


}
