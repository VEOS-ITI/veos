package com.veos.dashboard;

import android.annotation.SuppressLint;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.widget.ImageView;
import android.widget.MediaController;
import android.widget.TextView;
import android.widget.VideoView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

public class MainActivity extends AppCompatActivity {
    
    // Property IDs from your list
    private static final int PROP_GEAR_SELECTION = 557846352;
    private static final int PROP_VEHICLE_SPEED = 557846353;
    private static final int PROP_BATTERY_LEVEL = 557846354;
    private static final int PROP_HEADLIGHTS_STATE = 555749214;
    private static final int PROP_HIGH_BEAM_LIGHTS = 555749215;
    private static final int PROP_DAYTIME_LIGHTS = 555749216;
    private static final int PROP_BATTERY_WARNING = 555749217;
    private static final int PROP_TPMS_WARNING = 555749219;
    private static final int PROP_BRAKE_WARNING = 555749220;
    
    // Using turn signal properties (we'll use gear selection with different values)
    private static final int TURN_SIGNAL_NONE = 0;
    private static final int TURN_SIGNAL_RIGHT = 1;
    private static final int TURN_SIGNAL_LEFT = 2;

    private RecyclerView gearRecycler;
    private TextView speedText, chargeText, tempText;
    private VideoView videoView;
    private ImageView lightingON, lightingOFF, lightingERROR;
    private ImageView rightLightON, rightLightOFF;
    private ImageView leftLightON, leftLightOFF;
    private ImageView batteryCharge;
    private ImageView batteryLevel1, batteryLevel2, batteryLevel3, batteryLevel4, batteryLevel5;
    private ImageView seatBeltImg, tireImg;
    
    private Handler handler;
    private static final int UPDATE_INTERVAL = 1000; // 1 second

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Initialize views
        initializeViews();
        
        // Setup video background
        setupVideoBackground();
        
        // Setup gear recycler view
        setupGearRecycler();
        
        // Initialize VHAL
        VhalNative.init();
        
        // Create handler for periodic updates
        handler = new Handler(Looper.getMainLooper());
        
        // Start periodic updates
        startVehicleDataUpdates();
    }

    private void initializeViews() {
        speedText = findViewById(R.id.textView);
        chargeText = findViewById(R.id.charge_txt);
        tempText = findViewById(R.id.temp_txt);
        videoView = findViewById(R.id.videoView);
        
        // Lighting indicators
        lightingON = findViewById(R.id.light_on_img);
        lightingOFF = findViewById(R.id.light_off_img);
        lightingERROR = findViewById(R.id.light_error_img);
        
        // Turn signal indicators
        rightLightON = findViewById(R.id.right_on_img);
        rightLightOFF = findViewById(R.id.right_off_img);
        leftLightON = findViewById(R.id.left_on_img);
        leftLightOFF = findViewById(R.id.left_off_img);
        
        // Battery indicators
        batteryCharge = findViewById(R.id.charge_img);
        batteryLevel1 = findViewById(R.id.battery1_img);
        batteryLevel2 = findViewById(R.id.battery2_img);
        batteryLevel3 = findViewById(R.id.battery3_img);
        batteryLevel4 = findViewById(R.id.battery4_img);
        batteryLevel5 = findViewById(R.id.battery5_img);
        
        // Warning indicators
        seatBeltImg = findViewById(R.id.seat_belt_img);
        tireImg = findViewById(R.id.tire_img);
        
        gearRecycler = findViewById(R.id.gearRecycler);
    }

    private void setupVideoBackground() {
        String videoPath = "android.resource://" + getPackageName() + "/" + R.raw.road_moving4;
        Uri uri = Uri.parse(videoPath);
        videoView.setVideoURI(uri);

        MediaController mediaController = new MediaController(this);
        videoView.setMediaController(mediaController);
        mediaController.setAnchorView(videoView);
        videoView.start();
        
        // Loop the video
        videoView.setOnCompletionListener(mp -> videoView.start());
    }

    private void setupGearRecycler() {
        LinearLayoutManager layoutManager = new LinearLayoutManager(
            this, LinearLayoutManager.HORIZONTAL, false);
        gearRecycler.setLayoutManager(layoutManager);

        GearAdapter adapter = new GearAdapter();
        gearRecycler.setAdapter(adapter);
    }

    private void startVehicleDataUpdates() {
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                updateVehicleData();
                handler.postDelayed(this, UPDATE_INTERVAL);
            }
        }, UPDATE_INTERVAL);
    }

    @SuppressLint("DefaultLocale")
    private void updateVehicleData() {
        try {
            // Read data from VHAL via JNI
            int speed = VhalNative.getInt32(PROP_VEHICLE_SPEED, 0);
            int batteryLevel = VhalNative.getInt32(PROP_BATTERY_LEVEL, 0);
            boolean headlightsOn = VhalNative.getBool(PROP_HEADLIGHTS_STATE, 0);
            boolean highBeamOn = VhalNative.getBool(PROP_HIGH_BEAM_LIGHTS, 0);
            boolean batteryWarning = VhalNative.getBool(PROP_BATTERY_WARNING, 0);
            boolean tireWarning = VhalNative.getBool(PROP_TPMS_WARNING, 0);
            int gearPosition = VhalNative.getInt32(PROP_GEAR_SELECTION, 0);
            
            // For turn signals, we'll use gear selection with different values
            int turnSignalState = gearPosition % 3; // 0=none, 1=right, 2=left

            // Update UI with new data
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    // Update speed
                    speedText.setText(String.valueOf(speed));
                    
                    // Update battery
                    chargeText.setText(String.format("%d%%", batteryLevel));
                    updateBatteryIndicator(batteryLevel, batteryWarning);
                    
                    // Update lighting
                    updateLightingIndicator(headlightsOn, highBeamOn);
                    
                    // Update turn signals
                    updateTurnSignalIndicator(turnSignalState);
                    
                    // Update gear position (use modulo to ensure valid position 0-3)
                    GearAdapter adapter = (GearAdapter) gearRecycler.getAdapter();
                    if (adapter != null) {
                        adapter.setSelectedPosition(gearPosition % 4);
                    }
                    
                    // Update warning indicators
                    updateWarningIndicators(tireWarning);
                }
            });
        } catch (Exception e) {
            Log.e("VHAL", "Error reading vehicle data: " + e.getMessage());
        }
    }

    private void updateBatteryIndicator(int level, boolean warning) {
        // Hide all battery level images first
        batteryLevel1.setVisibility(ImageView.INVISIBLE);
        batteryLevel2.setVisibility(ImageView.INVISIBLE);
        batteryLevel3.setVisibility(ImageView.INVISIBLE);
        batteryLevel4.setVisibility(ImageView.INVISIBLE);
        batteryLevel5.setVisibility(ImageView.INVISIBLE);
        
        // Show warning if battery is low
        if (warning) {
            batteryCharge.setVisibility(ImageView.VISIBLE);
        } else {
            batteryCharge.setVisibility(ImageView.INVISIBLE);
        }
        
        // Show appropriate battery level
        if (level >= 80) {
            batteryLevel5.setVisibility(ImageView.VISIBLE);
        } else if (level >= 60) {
            batteryLevel4.setVisibility(ImageView.VISIBLE);
        } else if (level >= 40) {
            batteryLevel3.setVisibility(ImageView.VISIBLE);
        } else if (level >= 20) {
            batteryLevel2.setVisibility(ImageView.VISIBLE);
        } else {
            batteryLevel1.setVisibility(ImageView.VISIBLE);
        }
    }

    private void updateLightingIndicator(boolean headlightsOn, boolean highBeamOn) {
        if (highBeamOn) {
            // High beam mode
            lightingON.setVisibility(ImageView.VISIBLE);
            lightingOFF.setVisibility(ImageView.INVISIBLE);
            lightingERROR.setVisibility(ImageView.INVISIBLE);
        } else if (headlightsOn) {
            // Normal headlights
            lightingON.setVisibility(ImageView.VISIBLE);
            lightingOFF.setVisibility(ImageView.INVISIBLE);
            lightingERROR.setVisibility(ImageView.INVISIBLE);
        } else {
            // Lights off
            lightingOFF.setVisibility(ImageView.VISIBLE);
            lightingON.setVisibility(ImageView.INVISIBLE);
            lightingERROR.setVisibility(ImageView.INVISIBLE);
        }
    }

    private void updateTurnSignalIndicator(int turnSignalState) {
        switch (turnSignalState) {
            case TURN_SIGNAL_RIGHT:
                rightLightON.setVisibility(ImageView.VISIBLE);
                rightLightOFF.setVisibility(ImageView.INVISIBLE);
                leftLightON.setVisibility(ImageView.INVISIBLE);
                leftLightOFF.setVisibility(ImageView.VISIBLE);
                break;
                
            case TURN_SIGNAL_LEFT:
                rightLightON.setVisibility(ImageView.INVISIBLE);
                rightLightOFF.setVisibility(ImageView.VISIBLE);
                leftLightON.setVisibility(ImageView.VISIBLE);
                leftLightOFF.setVisibility(ImageView.INVISIBLE);
                break;
                
            case TURN_SIGNAL_NONE:
            default:
                rightLightON.setVisibility(ImageView.INVISIBLE);
                rightLightOFF.setVisibility(ImageView.VISIBLE);
                leftLightON.setVisibility(ImageView.INVISIBLE);
                leftLightOFF.setVisibility(ImageView.VISIBLE);
                break;
        }
    }

    private void updateWarningIndicators(boolean tireWarning) {
        // Tire pressure warning
        tireImg.setVisibility(tireWarning ? ImageView.VISIBLE : ImageView.INVISIBLE);
        
        // For now, we'll keep seat belt hidden since we don't have that property
        seatBeltImg.setVisibility(ImageView.INVISIBLE);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // Remove any pending callbacks when activity is destroyed
        if (handler != null) {
            handler.removeCallbacksAndMessages(null);
        }
    }
}
