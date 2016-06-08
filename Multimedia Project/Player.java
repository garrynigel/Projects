import java.awt.*;
import java.io.*;
import javax.swing.*;
import java.awt.event.*;
import PlayerComponents.*;
import Summary.*;
import Video.*;
import Config.Constants;


public class Player {

    String audFile;
    String vidFile;
    String searchImg;
    JFrame frame;
    InputStream is;
    //BufferedImage img;
    Video videoPlayer;
    Audio audioPlayer;
    JButton playButton, pauseButton, stopButton, summarizeButton, searchButton;


    JLabel lbIm1;
    public Player(String searchImg){
        this.searchImg = searchImg;
    }

    public void loadAudio( String audioFile){
        audFile = audioFile;
        audioPlayer = new Audio(audFile);
        audioPlayer.name = "sarvesh";

    }

    public void loadVideo(String videoFile){
        vidFile = videoFile;
        videoPlayer = new Video(vidFile,audioPlayer);
    }

    public void initialize(){

        frame = new JFrame();

        //frame.getContentPane().setLayout(gLayout);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        JPanel buttons = new JPanel();
        buttons.setPreferredSize(new Dimension(Constants.WIDTH, 90));

        JPanel buttonPanel = new JPanel();
        buttonPanel.setPreferredSize(new Dimension(Constants.WIDTH, 45));
        buttons.add(buttonPanel);

        JPanel advancedPanel = new JPanel();
        advancedPanel.setPreferredSize(new Dimension(Constants.WIDTH, 45));
        buttons.add(advancedPanel);

        frame.getContentPane().add(buttons, BorderLayout.SOUTH);

        playButton = new JButton("PLAY");
        playButton.addActionListener(new CustomActionListener());
        buttonPanel.add(playButton, BorderLayout.NORTH);


        pauseButton = new JButton("PAUSE");
        pauseButton.addActionListener(new CustomActionListener());
        buttonPanel.add(pauseButton, BorderLayout.NORTH);

        stopButton = new JButton("STOP");
        stopButton.addActionListener(new CustomActionListener());
        buttonPanel.add(stopButton, BorderLayout.NORTH);

        summarizeButton = new JButton("SUMMARIZE");
        summarizeButton.addActionListener(new CustomActionListener());
        advancedPanel.add(summarizeButton, BorderLayout.SOUTH);

        searchButton = new JButton("SEARCH");
        searchButton.addActionListener(new CustomActionListener());
        advancedPanel.add(searchButton, BorderLayout.SOUTH);


        JLabel lbText1 = new JLabel("Video: " + vidFile);
        lbText1.setHorizontalAlignment(SwingConstants.LEFT);
        JLabel lbText2 = new JLabel("AudioAnalyser: " + audFile);
        lbText2.setHorizontalAlignment(SwingConstants.LEFT);
        lbIm1 = new JLabel(new ImageIcon( videoPlayer.img));

        frame.getContentPane().add(lbIm1);

        frame.pack();
        frame.setVisible(true);
    }

    public void play(){
        lbIm1.setIcon(new ImageIcon(videoPlayer.img));
        audioPlayer.play();
        videoPlayer.play(frame);
    }

    public void pause(){
        audioPlayer.pause();
        videoPlayer.pause();
    }

    public void stop(){
        audioPlayer.stop();
        videoPlayer.stop();
        lbIm1.setIcon(null);
    }

    public void summarize(){
        stop();
        playButton.setEnabled(false);
        pauseButton.setEnabled(false);

        Condenser summ = new Condenser(vidFile,audFile);
        summ.condense("asd");

        System.out.println("Video Summarized");
        loadAudio(Constants.SUMMARIZED_AUDIO);
        loadVideo(Constants.SUMMARIZED_VIDEO);
        playButton.setEnabled(true);
        pauseButton.setEnabled(true);

    }

    public void search()
    {
        stop();
        playButton.setEnabled(false);
        pauseButton.setEnabled(false);


        ImgIndex img = new ImgIndex(this.vidFile, this.audFile);

        img.genHistList();
        img.genClusters();
        //img.genFiles();


        QueryImg query = new QueryImg();
        query.searchImg(this.searchImg,img);
        System.out.println("Image Found");
        loadAudio(Constants.SEARCHED_AUDIO);
        loadVideo(Constants.SEARCHED_VIDEO);
        playButton.setEnabled(true);
        pauseButton.setEnabled(true);
    }


    class CustomActionListener implements ActionListener {
        public void actionPerformed(ActionEvent e) {
            String buttonText = ((JButton) e.getSource()).getText();
            if(buttonText.equals("PLAY")) {
                playButton.setEnabled(false);
                pauseButton.setEnabled(true);
                play();
            }
            else if(buttonText.equals("PAUSE")) {
                pauseButton.setEnabled(false);
                playButton.setEnabled(true);
                pause();
            }
            else if(buttonText.equals("STOP")) {
                playButton.setEnabled(true);
                pauseButton.setEnabled(true);
                stop();
            }
            else if(buttonText.equals("SUMMARIZE")){
                summarize();
            }
            else if(buttonText.equals("SEARCH")) {
                search();
            }
        }
    }

}
