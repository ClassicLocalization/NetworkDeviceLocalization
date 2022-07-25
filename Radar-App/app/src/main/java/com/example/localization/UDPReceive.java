package com.example.localization;


import android.app.Activity;
import android.util.Log;
import android.widget.TextView;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;

public class UDPReceive implements Runnable{
    private DatagramSocket udpSocket;
    private MainActivity activity;
    private int port;

    public UDPReceive(int port , MainActivity activity) throws SocketException {
        this.udpSocket = new DatagramSocket(port);
        this.activity = activity;
        this.port = port;
    }

    @Override
    public void run() {
        try {
            InetAddress serverAddr = InetAddress.getByName("192.168.4.1");
            byte[] buf = ("1 .4").getBytes();
            DatagramPacket packet = new DatagramPacket(buf, buf.length,serverAddr, this.port);
            udpSocket.send(packet);
            Log.i("UDP client: ", "Message sent");
        } catch (SocketException e) {
            Log.e("Udp:", "Socket Error:", e);
        } catch (IOException e) {
            Log.e("Udp Send:", "IO Error:", e);
        }
        boolean run = true;
        while (run) {
            try {
                byte[] message = new byte[8000];
                DatagramPacket packet = new DatagramPacket(message,message.length);
                Log.i("UDP client: ", "about to wait to receive");
                this.udpSocket.receive(packet);
                String text = new String(message, 0, packet.getLength());
                activity.runOnUiThread(new Runnable() {
                    public void run() {
                        String[] csiData = new String[3];
                        for(int i = 1; i < csiData.length; i++) {
                            csiData[i] = "";
                        }
                        int index = 0;
                        for(int i = 1; i < text.length(); i++) {
                            if(text.charAt(i) == ';') {
                                index++;
                            }else {
                                csiData[index] += text.charAt(i);
                            }
                            if(index > 2) {
                                break;
                            }
                        }
                        Log.i("UDP client: ", csiData[0]);
                        Log.i("UDP client: ", csiData[1]);
                        Log.i("UDP client: ", csiData[2]);
                        activity.setCsiValues(csiData[1], csiData[0].substring(4), csiData[2], packet.getAddress().getHostAddress());

                    }
                });
                Log.d("Received data", text);
            }catch (IOException e) {
                Log.e("UDP client has IOException", "error: ", e);
                run = false;
            }
        }
    }
}