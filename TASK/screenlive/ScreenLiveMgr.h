#pragma once

#include "EduVideoSink.h"
#include "LiveSdkCallback.h"
#include "EduExecutor.h"
#include "EduEvent.h"
#include "ScreenLiveBase.h"
#include "EduVideoSink.h"

class ScreenLiveMgr :public IVideoSourceEventHandler,public ISceenShareListener
{
public:
  ~ScreenLiveMgr();
  static ScreenLiveMgr* instance();

  EDUSSContext GenCtx(std::string yyappid, std::string agoappid, uint32_t uid, uint32_t sid, EDUSdkType type, int bitrate, int fps);
  void AsyncExecute(EduMediaScreen::Runnable *r) const;
  ipc_error SwitchSdk(EMSS_SdkType from, EMSS_SdkType to);

  ipc_error StartShareHwnd(HWND hWnd, iLiveSdkSucCallback suc, iLiveSdkErrCallback err, void* data);
  ipc_error StartShareScreen(int screenId, iLiveSdkSucCallback suc, iLiveSdkErrCallback err, void* data);
  ipc_error StartShareRect(const RECT* pRect, iLiveSdkSucCallback suc, iLiveSdkErrCallback err, void* data);
  ipc_error StopScreenCaptureShare(iLiveSdkSucCallback suc, iLiveSdkErrCallback err, void* data);

  //event handler
  void onVideoSourceProcessId(uint32_t processid) override;

  void onVideoSourceMonitorList(EMSS_MonitorList list) override;
  /**
   * Video source joined channel success event.
   * @param uid : video source's uid.
   */
  void onVideoSourceJoinedChannel(bool success, uint32_t uid, uint32_t sid) override;

  /**
   * Video source leaved channel event.
   */
  void onVideoSourceLeaveChannel(bool success, uint32_t uid, uint32_t sid) override;
  void onVideoSourceCaptureStop() override;
  void onVideoSourceCaptureStart(bool success, std::string desc, EmSS_ScreenCaptureType captype, int ret) override;
  void onVideoSourceExit() override;
  ///stream manager event
  void  onStreenUid(bool success, uint32_t uid, std::string desc = "") override;
  void onStreamStatus(uint32_t uid,uint32_t screenUid, uint32_t sid, int status) override;
  void onSwitch(ELiveSdkType oldsdk, ELiveSdkType newsdk) override;
  //设置子进程的日志路径
  int setLogPath(const char * logpath);
  void registeStreamListener(StreamListenr *listener, void *data);
  bool CloseChildProcess();
  ipc_error Initialize(const char * logpath, const EDUSSContext& context, EduSSEventHandler *handler);
  int Destroy();
  ipc_error StartScreenCaptureForHwnd(HWND hWnd, const RECT* pRect = NULL);
  ipc_error StartScreenCaptureForScreen(int screenId = 0, const RECT* pRect = NULL);
  ipc_error UpdateScreenCaptureRect(const RECT* pRect);
  ipc_error StopScreenCapture();
  ipc_error JoinChannel();
  ipc_error LeaveChannel();
private:
  void onSyncStartShareHwnd(const EDUSSContext& context, HWND hWnd);
  void onSyncShareScreen(const EDUSSContext& context, int screenid);
  void onSyncShareRect(const EDUSSContext& ctx, const RECT* pRect);
private:
  void onCallbackInit();
  void onCallbackReset();
  void onSdkInit();
  bool isSdkInit();
  void onSdkShutdown();
  void onSinkReset();
  ScreenLiveMgr();
  static ScreenLiveMgr *instance_;
  std::unique_ptr<EduVideoSink> sink_;
  std::string logpath_;
  EduSSEventHandler *handler_ = nullptr;
  EduMediaScreen::EduEvent event_;
  uint32_t screenUid_ = 0;
  uint32_t screenshare_processId_ = 0;

  LiveSdkResultCallback share_hwd_callback_;
  LiveSdkResultCallback share_screen_callback_;
  LiveSdkResultCallback share_rect_callback_;
  LiveSdkResultCallback share_stop_callback_;
  ELiveSdkType cursdktype_;
  std::atomic_bool need_reset_sink_ = false;
  EmSS_ScreenCaptureType screen_cap_type_ = EM_SCAP_TYPE_UNKOWN;
};



