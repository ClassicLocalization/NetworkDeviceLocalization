package com.example.localization;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import java.net.SocketException;
import java.util.Random;
import java.util.concurrent.ThreadLocalRandom;

public class MainActivity extends AppCompatActivity {
    Thread thread;

    public void changeConnectedIP(String ip) {
        setContentView(R.layout.activity_main);
        TextView ipView = (TextView)findViewById(R.id.ip);
        ipView.setText(ip);
    }

    public void changeRssi(String rssi) {
        setContentView(R.layout.activity_main);
        TextView rssiView = (TextView)findViewById(R.id.rssiView);
        rssiView.setText(rssi);
    }

    public void setCsiValues(String rssi, String std, String corr, String ip) {
        setContentView(R.layout.activity_main);

        TextView ipView = (TextView)findViewById(R.id.ip);
        TextView rssiView = (TextView)findViewById(R.id.rssiView);
        TextView stdView  = (TextView)findViewById(R.id.stdView);
        TextView corrView  = (TextView)findViewById(R.id.corrView);

        ipView.setText(ip);
        rssiView.setText(rssi);
        stdView.setText(std);
        corrView.setText(corr);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        try {
            thread = new Thread(new UDPReceive(50000, this));
        } catch (SocketException e) {
            e.printStackTrace();
        }
        setContentView(R.layout.activity_main);
    }

    public void startLocalization(View v) throws Exception {
        if(!this.thread.isAlive()) {
            this.thread.start();
        }
    }

    //For testing purposes: randomly generated values to determine optimal fit for numbers in circle
    public void changeValues(View v) throws Exception {
        setContentView(R.layout.activity_main);

        TextView rssi = (TextView)findViewById(R.id.rssiView);
        TextView std = (TextView)findViewById(R.id.stdView);
        TextView corr = (TextView)findViewById(R.id.corrView);
        TextView ip = (TextView)findViewById(R.id.ip);
        ip.setText("192.168.172.4");

        Random r = new Random();

        rssi.setText(Integer.toString(ThreadLocalRandom.current().nextInt(-100, -5)));
        std.setText(Float.toString(0 + r.nextFloat() * (1)));
        corr.setText(Float.toString(0 + r.nextFloat() * (1)));

        if(!thread.isAlive()) {
            thread.start();
        }

    }
}