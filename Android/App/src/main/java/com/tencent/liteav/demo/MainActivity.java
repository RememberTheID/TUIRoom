package com.tencent.liteav.demo;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.util.Log;

import com.tencent.liteav.debug.GenerateTestUserSig;
import com.tencent.liteav.login.model.ProfileManager;
import com.tencent.liteav.login.model.UserModel;
import com.tencent.liteav.meeting.model.TRTCMeeting;
import com.tencent.liteav.meeting.model.TRTCMeetingCallback;
import com.tencent.liteav.meeting.ui.CreateMeetingActivity;

public class MainActivity extends Activity {
    private static final String TAG = "MainActivity";

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        initMeetingData();
        Intent intent = new Intent(this, CreateMeetingActivity.class);
        startActivity(intent);
        finish();
    }

    private void initMeetingData() {
        final UserModel userModel = ProfileManager.getInstance().getUserModel();
        TRTCMeeting.sharedInstance(this).login(GenerateTestUserSig.SDKAPPID, userModel.userId, userModel.userSig, new TRTCMeetingCallback.ActionCallback() {
            @Override
            public void onCallback(int code, String msg) {
                Log.d(TAG, "code: "+code + " msg:"+msg);
            }
        });
    }
}