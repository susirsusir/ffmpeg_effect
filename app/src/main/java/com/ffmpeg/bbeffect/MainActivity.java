package com.ffmpeg.bbeffect;

import android.Manifest;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.os.Build;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.PermissionChecker;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import java.util.ArrayList;

public class MainActivity extends AppCompatActivity implements EffectPlayListener{


    private String[] denied;
    private String[] permissions = {Manifest.permission.WRITE_EXTERNAL_STORAGE};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (Build.VERSION.SDK_INT >= 21) {
            View decorView = getWindow().getDecorView();
            int option = View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN | View.SYSTEM_UI_FLAG_LAYOUT_STABLE;
            decorView.setSystemUiVisibility(option);
            getWindow().setStatusBarColor(Color.TRANSPARENT);
        }
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            ArrayList<String> list = new ArrayList<>();
            for (int i = 0; i < permissions.length; i++) {
                if (PermissionChecker.checkSelfPermission(this, permissions[i]) == PackageManager.PERMISSION_DENIED) {
                    list.add(permissions[i]);
                }
            }
            if (list.size() != 0) {
                denied = new String[list.size()];
                for (int i = 0; i < list.size(); i++) {
                    denied[i] = list.get(i);
                    ActivityCompat.requestPermissions(this, denied, 5);
                }

            } else {
                initView();
            }
        } else {
            initView();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (requestCode == 5) {
            boolean isDenied = false;
            for (int i = 0; i < denied.length; i++) {
                String permission = denied[i];
                for (int j = 0; j < permissions.length; j++) {
                    if (permissions[j].equals(permission)) {
                        if (grantResults[j] != PackageManager.PERMISSION_GRANTED) {
                            isDenied = true;
                            break;
                        }
                    }
                }
            }
            if (isDenied) {
                Toast.makeText(this, "请开启权限", Toast.LENGTH_SHORT).show();
            } else {
                initView();

            }
        }
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    }

    private BbEffectView surfaceView;

    private void initView() {
        surfaceView = findViewById(R.id.surface_view);
        surfaceView.setEffectPlayListener(this);
    }

    String videoPatha =  "/storage/emulated/0/xingjiabi/live/defaulta.arf";
    String videoPathb =  "/storage/emulated/0/xingjiabi/live/defaulta.arf";


    public void play_start(View view) {
        surfaceView.setEffectPath(1,videoPatha);
    }

    public void play_startb(View view) {
        surfaceView.setEffectPath(1,videoPathb);
    }

    public void play_stop(View view) {
        surfaceView.videoStop();
    }

    @Override
    public void onAnimEvent(int priority,int type, int ret) {
        Log.d("LiveTest", "activity type: " + type + " ret: " + ret);
        if (type == EffectConst.MSG_TYPE_INFO) {
            if (ret == EffectConst.MSG_STAT_EFFECTS_END) {
                if (surfaceView != null) {
                    surfaceView.setVisibility(View.GONE);
                }
            }
        }
    }
}