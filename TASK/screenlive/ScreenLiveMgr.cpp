#include "stdafx.h"
#include "ScreenLiveMgr.h"
#include "StreamMgr.h"
#include "log/FinalLog.h"

#undef _CLS_
#define _CLS_ "ScreenLiveMgr"

static EduMediaScreen::Executor * executor = nullptr;
ScreenLiveMgr* ScreenLiveMgr::instance_ = nullptr;

ScreenLiveMgr::ScreenLiveMgr()
  :sink_(new EduVideoSink())
  , event_(false)
{
  LOGN(_CLS_) << " ScreenLiveMgr ";
  onSdkInit();
  onCallbackInit();
  StreamMgr::instance()->registerScreenShareListener(this);
}
void ScreenLiveMgr::onSdkInit()
{
  if (isSdkInit())
  {
	return;
  }
  executor = new EduMediaScreen::Executor;
  executor->start();
}
bool ScreenLiveMgr::isSdkInit()
{
  return executor != nullptr;
}
void ScreenLiveMgr::onSdkShutdown()
{
  if (!isSdkInit())
  {
	return;
  }
  executor->shutdown();
  delete executor;
  executor = nullptr;
}
ScreenLiveMgr::~ScreenLiveMgr()
{
  StreamMgr::instance()->registerScreenShareListener(nullptr);
  onSdkShutdown();
  onCallbackReset();
  sink_.reset();

  LOGN(_CLS_) << " ScreenLiveMgr::~ScreenLiveMgr ";
}
void ScreenLiveMgr::onCallbackInit()
{
  share_hwd_callback_.m_resultClkName = "share_hwd_callback";
  share_stop_callback_.m_resultClkName = "share_stop_callback";
  share_screen_callback_.m_resultClkName = "share_screen_callback";
  share_rect_callback_.m_resultClkName = "share_rect_callback";
}
void ScreenLiveMgr::onCallbackReset()
{
  share_hwd_callback_.reset();
  share_stop_callback_.reset();
  share_screen_callback_.reset();
  share_rect_callback_.reset();
}
void ScreenLiveMgr ::onSinkReset()
{
  if (need_reset_sink_)
  {
	LOGW(_CLS_) << " ScreenLiveMgr reset sink";
	sink_.reset(new EduVideoSink());
	need_reset_sink_ = false;
  }
  else
  {
	LOGN(_CLS_) << " ScreenLiveMgr no need reset sink";
  }

}
ScreenLiveMgr* ScreenLiveMgr::instance()
{
  if (instance_ == nullptr)
  {
	instance_ = new ScreenLiveMgr();
  }
  return instance_;
}

ipc_error ScreenLiveMgr::Initialize(const char * logpath, const EDUSSContext& context, EduSSEventHandler *handler)
{
  handler_ = handler;
  ipc_error ret = ipc_ok;
  if (sink_.get())
  {
	ret = sink_->initialize(this, context);
  }
  else
  {
	LOGN(_CLS_) << " sink not init ";
	return ipc_sink_not_init;
  }
  return ret;
}
int ScreenLiveMgr::Destroy()
{
  StopScreenCapture();
  return 0;
}
ipc_error ScreenLiveMgr::StartScreenCaptureForHwnd(HWND hWnd, const RECT* pRect)
{
  if (sink_.get())
  {
	return sink_->startScreenCaptureForHwnd(hWnd, pRect);
  }
  LOGE(_CLS_) << " sink not init ";
  return ipc_sink_not_init;
}
ipc_error ScreenLiveMgr::StartScreenCaptureForScreen(int screenId, const RECT* pRect)
{
  if (sink_.get())
  {
	return sink_->startScreenCaptureForScreen(screenId, pRect);
  }
  LOGE(_CLS_) << " sink not init ";
  return ipc_sink_not_init;
}
ipc_error ScreenLiveMgr::UpdateScreenCaptureRect(const RECT* pRect)
{
  return ipc_ok;
}
ipc_error ScreenLiveMgr::StopScreenCapture()
{
  if (sink_.get())
  {
	ipc_error ret =  sink_->stopCaptureScreen();
    //收到stop通知后再执行退出
	return ret;
  }
  LOGE(_CLS_) << " sink not init ";
  return ipc_sink_not_init;
}

ipc_error ScreenLiveMgr::JoinChannel()
{
  if (sink_.get())
  {
	return sink_->join();
  }
  LOGN(_CLS_) << " sink not init ";
  return ipc_sink_not_init;
}
ipc_error ScreenLiveMgr::LeaveChannel()
{
  if (sink_.get())
  {
	return sink_->leave();
  }
  LOGN(_CLS_) << " sink not init ";
  return ipc_sink_not_init;
}
ipc_error ScreenLiveMgr::SwitchSdk(EMSS_SdkType from, EMSS_SdkType to)
{
  if (sink_.get())
  {
	return sink_->switchSdk(from, to);
  }
  LOGN(_CLS_) << " sink not init ";
  return ipc_sink_not_init;
}
void ScreenLiveMgr::AsyncExecute(EduMediaScreen::Runnable *r) const {
  EduMediaScreen::Executor::instance()->execute(r);
}
//////////////////////////////////////////////
/////////////api for livesdk
//////////////////////////////////////////////
EDUSSContext ScreenLiveMgr::GenCtx(std::string yyappid,std::string agoappid,uint32_t uid,uint32_t sid, EDUSdkType type,int bitrate,int fps)
{
  cursdktype_ = (ELiveSdkType)type;
  EDUSSContext ctx(yyappid,agoappid,uid,sid,type,bitrate,fps);
  return std::move(ctx);
}


void ScreenLiveMgr::onSyncStartShareHwnd(const EDUSSContext& context, HWND hWnd)
{
  onSinkReset();
  Initialize(logpath_.c_str(), context, nullptr);
  JoinChannel();
  StartScreenCaptureForHwnd(hWnd);
}

EDUSSContext getCtx()
{
  EDUSSContext context;
  std::string appidAGO = StreamMgr::instance()->param().getAgoraId();
  std::string appidyy = std::to_string(StreamMgr::instance()->param().getYyId());
  context.bitrate = 1500;
  context.fps = 15;
  context.sdkType = (EDUSdkType)StreamMgr::instance()->param().getSMSdkType();//默认开播类型
  if (context.sdkType != EDUSdkType::KSdkType_Agora || context.sdkType != EDUSdkType::KSdkType_YY)
  {
	LOGE(_CLS_) << "bug!!!!getCtx default sdktype " << context.sdkType << " error ";
  }
  //context.uid = 1933; //先不填,流管理通知填入
  context.sid = StreamMgr::instance()->param().getRoomName();
  LOGI(_CLS_) << "getCtx default sdktype " << context.sdkType << " sid " << context.sid;
  strcpy_s(context.appidYY, appidyy.size() + 1, appidyy.c_str());
  strcpy_s(context.appidAGO, appidAGO.size() + 1, appidAGO.c_str());
  return std::move(context);
}

ipc_error ScreenLiveMgr::StartShareHwnd(HWND hWnd, iLiveSdkSucCallback suc, iLiveSdkErrCallback err, void* data)
{
  screen_cap_type_ = EmSS_ScreenCaptureType::EM_SCAP_TYPE_HWND;
  share_hwd_callback_.set(suc, err, data); 
  static EDUSSContext ctx;
  ctx.set(getCtx());
  LOGI(_CLS_) << "StartShareHwnd sdktype " << ctx.sdkType << " sid " << ctx.sid ;
 // static std::function<void()> s_startShareHwnd = [this]
 // {
	//if (this)
	//{

	// // this->Initialize(ctx);
	//}
	//else
	//{
	//  LOGE(_CLS_) << " StartShareHwnd s_joinChannel no this";
	//}
 // };

  static std::function<void()> s_waitUid = [this,hWnd]
  {
	if (this)
	{
	  EduMediaScreen::EduEvent::WaitResult result= this->event_.WaitFor(10 * 1000);
	  if (result != EduMediaScreen::EduEvent::WaitResult::WAIT_RESULT_SET)
	  {
		this->share_hwd_callback_.fail(SCREENSHARE_RETCODE_QUERY_UID_RES_FAIL,"StartShareHwnd waitfor uid fail");
	  }
	  else
	  {
		//获取到uid
		ctx.uid = screenUid_;
		onSyncStartShareHwnd(ctx, hWnd);
	  }
	}
	else
	{
	  LOGE(_CLS_) << " StartShareHwnd s_starthwnd no this";
	}
  };

  auto fn = [this]()
  {
	StreamMgr::instance()->queryScreenShareUid([](void *data) {
	  if (data)
	  {
		LOGI(_CLS_) << "queryScreenShareUid success";
		ScreenLiveMgr *thiz = (ScreenLiveMgr*)data;

		//请求uid成功，这个时候在独立线程中等待uid 应答
		thiz->AsyncExecute(new EduMediaScreen::Runnable(s_waitUid));
	  }
	  }, [](int code, const char *desc, void* data) {
		//回调用户获取uid失败
		LOGE(_CLS_) << "queryScreenShareUid fail";
		if (data)
		{
		  ScreenLiveMgr *thiz = (ScreenLiveMgr*)data;
		  thiz->share_hwd_callback_.fail(SCREENSHARE_RETCODE_QUERY_UID_REQ_FAIL,"StartShareHwnd queryScreenShareUid fail");
		}
		
	  }, this);
  };
  AsyncExecute(new EduMediaScreen::Runnable(fn));
  return ipc_ok;
  //同步测试
  onSyncStartShareHwnd(ctx,hWnd);
  return ipc_ok;
}

void ScreenLiveMgr::onSyncShareScreen(const EDUSSContext& ctx, int screenId)
{
  //同步测试
  onSinkReset();
  Initialize(logpath_.c_str(), ctx, nullptr);
  JoinChannel();
  StartScreenCaptureForScreen(screenId, nullptr);
}
ipc_error ScreenLiveMgr::StartShareScreen(int screenId, iLiveSdkSucCallback suc, iLiveSdkErrCallback err, void* data)
{
  screen_cap_type_ = EmSS_ScreenCaptureType::EM_SCAP_TYPE_SCREEN;
  share_screen_callback_.set(suc, err, data);
  static EDUSSContext ctx;
  ctx.set(getCtx());

  static std::function<void()> s_waitUid = [this, screenId]
  {
	if (this)
	{
	  EduMediaScreen::EduEvent::WaitResult result = this->event_.WaitFor(10 * 1000);
	  if (result != EduMediaScreen::EduEvent::WaitResult::WAIT_RESULT_SET)
	  {
		this->share_screen_callback_.fail(SCREENSHARE_RETCODE_QUERY_UID_RES_FAIL, "StartShareScreen waitfor uid fail");
	  }
	  else
	  {
		//获取到uid
		ctx.uid = screenUid_;
		onSyncShareScreen(ctx, screenId);
	  }
	}
	else
	{
	  LOGE(_CLS_) << " StartShareScreen s_waitUid no this";
	}
  };

  auto fn = [this]()
  {
	StreamMgr::instance()->queryScreenShareUid([](void *data) {
	  if (data)
	  {
		LOGI(_CLS_) << "queryScreenShareUid success";
		ScreenLiveMgr *thiz = (ScreenLiveMgr*)data;

		//请求uid成功，这个时候在独立线程中等待uid 应答
		thiz->AsyncExecute(new EduMediaScreen::Runnable(s_waitUid));
	  }
	  }, [](int code, const char *desc, void* data) {
		//回调用户获取uid失败
		LOGI(_CLS_) << "queryScreenShareUid fail";
		if (data)
		{
		  ScreenLiveMgr *thiz = (ScreenLiveMgr*)data;
		  thiz->share_screen_callback_.fail(SCREENSHARE_RETCODE_QUERY_UID_REQ_FAIL, "StartShareScreen queryScreenShareUid fail");
		}
	  }, this);
  };
  AsyncExecute(new EduMediaScreen::Runnable(fn));
  return ipc_ok;
}
std::string rectToStr(const RECT* pRect)
{
  std::string str = "[rect ";
  if (pRect)
  {
	str += " bottom " + std::to_string(pRect->bottom) + " left " + std::to_string(pRect->left) +
	  " right " + std::to_string(pRect->right) + " top " + std::to_string(pRect->top) + "]";
  }
  else
  {
	str + "is null]";
  }
  return str;
}
void ScreenLiveMgr::onSyncShareRect(const EDUSSContext& ctx,const RECT* pRect)
{
  //同步测试
  onSinkReset();
  Initialize(logpath_.c_str(), ctx, nullptr);
  JoinChannel();
  StartScreenCaptureForScreen(0, pRect);
}
ipc_error ScreenLiveMgr::StartShareRect(const RECT* pRect, iLiveSdkSucCallback suc, iLiveSdkErrCallback err, void* data)
{
  screen_cap_type_ = EmSS_ScreenCaptureType::EM_SCAP_TYPE_AREA;
  share_rect_callback_.set(suc, err, data);
  static EDUSSContext ctx;
  ctx.set(getCtx());
  LOGI(_CLS_) << " StartShareRect rect " << rectToStr(pRect);
  static RECT rect;
  rect.bottom = pRect->bottom;
  rect.top = pRect->top;
  rect.left = pRect->left;
  rect.right = pRect->right;

  static std::function<void()> s_waitUid = [this]
  {
	if (this)
	{
	  EduMediaScreen::EduEvent::WaitResult result = this->event_.WaitFor(10 * 1000);
	  if (result != EduMediaScreen::EduEvent::WaitResult::WAIT_RESULT_SET)
	  {
		this->share_screen_callback_.fail(SCREENSHARE_RETCODE_QUERY_UID_RES_FAIL, "StartShareRect waitfor uid fail");
	  }
	  else
	  {
		//获取到uid
		ctx.uid = screenUid_;
		onSyncShareRect(ctx,&rect);
	  }
	}
	else
	{
	  LOGE(_CLS_) << " StartShareRect s_waitUid no this";
	}
  };

  auto fn = [this]()
  {
	StreamMgr::instance()->queryScreenShareUid([](void *data) {
	  if (data)
	  {
		LOGI(_CLS_) << "queryScreenShareUid success";
		ScreenLiveMgr *thiz = (ScreenLiveMgr*)data;

		//请求uid成功，这个时候在独立线程中等待uid 应答
		thiz->AsyncExecute(new EduMediaScreen::Runnable(s_waitUid));
	  }
	  }, [](int code, const char *desc, void* data) {
		//回调用户获取uid失败
		LOGI(_CLS_) << "queryScreenShareUid fail";
		if (data)
		{
		  ScreenLiveMgr *thiz = (ScreenLiveMgr*)data;
		  thiz->share_screen_callback_.fail(SCREENSHARE_RETCODE_QUERY_UID_REQ_FAIL, "StartShareScreen queryScreenShareUid fail");
		}
	  }, this);
  };
  AsyncExecute(new EduMediaScreen::Runnable(fn));
  return ipc_ok;

}

ipc_error ScreenLiveMgr::StopScreenCaptureShare(iLiveSdkSucCallback suc, iLiveSdkErrCallback err, void* data)
{
  share_stop_callback_.set(suc,err,data);

  auto fn = [this] {
	StopScreenCapture();
  };
  AsyncExecute(new EduMediaScreen::Runnable(fn));
  return ipc_ok;
}
////////////////////
int ScreenLiveMgr::setLogPath(const char * logpath)
{
  std::string path = std::string(logpath);
  if (path.empty())
  {
	LOGI(_CLS_) << " ScreenLiveMgr logpath empty ";
	//指定一个默认路径?
	logpath = "D:\\ALEX\\";
  }

  LOGI(_CLS_) << "ScreenLiveMgr logpath " << path;
  //std::wstring logpathW = EduMediaUtils::Str2Wstr(path);
  //std::string logpathA = EduMediaUtils::CreateUTF8TextInitWithString(logpathW.c_str());
  //EduSetLogPath_(path.c_str());
  logpath_ = path;
  return 0;
}

bool closeProcess(DWORD dwProcess) {

  HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, dwProcess);
  if (INVALID_HANDLE_VALUE != processHandle) {
	if (TerminateProcess(processHandle, 0)) {}
	else
	  WaitForSingleObject(processHandle, 2000);

	return CloseHandle(processHandle);
  }

  return false;
}
bool ScreenLiveMgr::CloseChildProcess()
{
  bool ret = false;
  if (screenshare_processId_ != 0)
  {
	//根据进程id关闭进程
	ret = closeProcess(screenshare_processId_);
	LOGI(_CLS_) << " kill source process   " << screenshare_processId_ << " closed ret " << ret;
	return ret;
  }
  else
  {
	LOGI(_CLS_) << " no child process";
	ret = false;
  }
  return ret;
}
//event handler
void ScreenLiveMgr::onVideoSourceProcessId(uint32_t processid)
{
  screenshare_processId_ = processid;
  if (handler_)
  {
	if (handler_->onSSProcessId)
	{
	  handler_->onSSProcessId(processid);
	}
  }
}
void ScreenLiveMgr::onVideoSourceMonitorList(EMSS_MonitorList list)
{
  if (handler_)
  {
	if (handler_->onMonitorList)
	{
	  handler_->onMonitorList(list);
	}
  }
}

void ScreenLiveMgr::onVideoSourceJoinedChannel(bool success, uint32_t uid,uint32_t sid)
{  
  LOGI(_CLS_) << "onVideoSourceJoinedChannel " << success <<" uid "<< uid <<" sid "<<sid ;
  //加组失败通知用户
  if (!success)
  {		
	switch (screen_cap_type_)
	{
	case EmSS_ScreenCaptureType::EM_SCAP_TYPE_HWND:
	  share_hwd_callback_.fail(SCREENSHARE_RETCODE_JOINCHANNEL_FAIL, "screenshare hwnd joinchannel fail");
	  break;
	case EmSS_ScreenCaptureType::EM_SCAP_TYPE_SCREEN:
	  share_screen_callback_.fail(SCREENSHARE_RETCODE_JOINCHANNEL_FAIL, "screenshare screen joinchannel fail");
	  break;
	case EmSS_ScreenCaptureType::EM_SCAP_TYPE_AREA:
	  share_rect_callback_.fail(SCREENSHARE_RETCODE_JOINCHANNEL_FAIL, "screenshare area joinchannel fail");
	  break;
	default:
	  LOGI(_CLS_) << "unkown screen cap type to callback user ";
	  break;
	}
  }  
}


void ScreenLiveMgr::onVideoSourceLeaveChannel(bool success, uint32_t uid, uint32_t sid)
{
  LOGI(_CLS_) << "==>onVideoSourceLeaveChannel success:"<<success<<" to disconnect with source,uid "<<uid<<" sid "<<sid;
  sink_->disconnect();
}
void ScreenLiveMgr::onVideoSourceCaptureStop()
{
  LOGI(_CLS_) << "==>onVideoSourceCaptureStop to leavechannel";
  LeaveChannel();
}
void ScreenLiveMgr::onVideoSourceExit()
{
  LOGW(_CLS_) << "==>onVideoSource process exit";
  share_stop_callback_.success();
  need_reset_sink_ = true;
  //通知流管理屏幕分享退出
  LOGI(_CLS_) << "onVideoSourceExit notifyScreenShareStatus uid " << screenUid_;
  StreamMgr::instance()->notifyScreenShareStatus(ScreenshareStreamStatus::KScreenShareStreamStatus_Stop, [](void *) {
	LOGI(_CLS_) << "====>notifyScreenShareStatus stop send ok";
	},
	[](int code, const char *desc, void* data) {
	  LOGI(_CLS_) << "====>notifyScreenShareStatus stop send fail";
	}, this);
  //todo 通知app或者上层 子进程退出成功
}
void ScreenLiveMgr::onVideoSourceCaptureStart(bool success, std::string desc,EmSS_ScreenCaptureType captype,int ret)
{
  LOGI(_CLS_) << "onVideoSourceCaptureStart "<<success<<" sdktype "<<desc <<" captype "<<captype <<" screenuid "<< screenUid_;
  if (success)
  {
	//通知流管理屏幕分享开始
	LOGI(_CLS_) << "notifyScreenShareStatus uid " << screenUid_;
	StreamMgr::instance()->notifyScreenShareStatus(ScreenshareStreamStatus::KScreenShareStreamStatus_Start, [](void *) {
	  LOGI(_CLS_) << "====>notifyScreenShareStatus start send ok";
	  },
	  [](int code, const char *desc, void* data) {
		LOGI(_CLS_) << "====>notifyScreenShareStatus start send fail";
	  },this);
  }
  else
  {
	//不通知？
	LOGE(_CLS_) << "onVideoSourceCaptureStart fail ,captype "<<captype <<" desc "<<desc <<" ret "<<ret ;
  }
  //通知用户屏幕分享开始成功
  switch (captype)
  {
  case EmSS_ScreenCaptureType::EM_SCAP_TYPE_AREA:
	if (success)
	{
	  share_rect_callback_.success();
	}
	else {
	  share_rect_callback_.fail(ret, desc.c_str());
	}
	break;
  case EmSS_ScreenCaptureType::EM_SCAP_TYPE_HWND:
	if (success)
	{
	  share_hwd_callback_.success();
	}
	else {
	  share_hwd_callback_.fail(ret,desc.c_str());
	}
	break;
  case EmSS_ScreenCaptureType::EM_SCAP_TYPE_SCREEN:
	if (success)
	{
	  share_screen_callback_.success();
	}
	else {
	  share_screen_callback_.fail(ret, desc.c_str());
	}
	  break;
  default:
	LOGE(_CLS_) <<" unkown screen captype "<< captype;
	break;
  }
}

void ScreenLiveMgr::onStreenUid(bool success, uint32_t uid, std::string desc)
{
  screenUid_ = uid;
  LOGI(_CLS_) << " onStreenUid " << uid;
  event_.notifyAll();
}
void ScreenLiveMgr::onStreamStatus(uint32_t uid, uint32_t screenUid, uint32_t sid, int status)
{
  //回调用户屏幕分享
  LOGI(_CLS_) << " onStreamStatus " << uid << " sid " << sid << " status " << status;
}

void ScreenLiveMgr::onSwitch(ELiveSdkType oldsdk, ELiveSdkType newsdk)
{
#if 0
  static ELiveSdkType _old = E_LiveDefault;
  static ELiveSdkType _new = E_LiveDefault;

  if (E_LiveNone == newsdk) {
	LOGE(_CLS_) << " onSwitch err :newsdk is none "<< newsdk;
	return;
  }
  if (newsdk == oldsdk)
  {
	LOGE(_CLS_) << " onSwitch  sdk is same "<< newsdk;
	return;
  }
  if (cursdktype_ == newsdk)
  {
	LOGE(_CLS_) << "err cursdk  same with new " << cursdktype_ << " newsdk " << newsdk;
	return;
  }
  if (cursdktype_ != _old)
  {
	LOGE(_CLS_) << "err cur sdk is not same with _old " << cursdktype_ <<" old "<< _old;
	return;
  }
#endif
  LOGI(_CLS_) << "ScreenLiveMgr switch from " << oldsdk << " to " << newsdk;
  if (oldsdk != newsdk)
  {
	LOGI(_CLS_) << "ScreenLiveMgr changed !switch from " << oldsdk << " to " << newsdk;
  }
  auto fn = [this,oldsdk, newsdk] {
	if (this)
	{
	  EMSS_SdkType from = (EMSS_SdkType)oldsdk;
	  EMSS_SdkType to = (EMSS_SdkType)newsdk;
	  LOMI(_CLS_) << " old " << oldsdk << " new " << newsdk << " from " << from << " to " << to;
	  this->SwitchSdk(from, to);
	}
  };
  AsyncExecute(new EduMediaScreen::Runnable(fn));
}

void ScreenLiveMgr::registeStreamListener(StreamListenr *listener, void *data)
{
  StreamMgr::instance()->registeStreamListener(listener, data);
}