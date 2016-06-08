package Bucket;

import java.util.ArrayList;

/**
 * Created by garrydmello on 4/28/16.
 */
public class Bucket {

    private int BUCKET_ID;
    private int BUCKET_RANGE;
    private ArrayList<Double[]> BUCKET_LIST;

    public Bucket(int id){

        this.BUCKET_ID = id;
        this.BUCKET_LIST = new ArrayList<>();
    }

    public int getId(){
        return this.BUCKET_ID;
    }


    public int getRange(){
        return this.BUCKET_RANGE;
    }

    public ArrayList<Double[]> getBucketList(){return this.BUCKET_LIST;}

    public void setRange(int range){

        this.BUCKET_RANGE = range;
    }

    public void addDrop(Double[] drop){
        this.BUCKET_LIST.add(drop);
    }



}
