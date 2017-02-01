#include <psp2/kernel/modulemgr.h>
#include <taihen.h>

#define SCE_NETCHECK_DIALOG_MODE_PSN 2
#define SCE_NETCHECK_DIALOG_MODE_PSN_ONLINE 3

#define SCE_COMMON_DIALOG_ERROR_NOT_IN_USE 0x80020411
#define SCE_COMMON_DIALOG_STATUS_NONE 0
#define SCE_COMMON_DIALOG_STATUS_FINISHED 2

typedef struct SceCommonDialogParam {
  void *infobarParam;
  void *bgColor;
  void *dimmerColor;
  char reserved[60];
  int magic;
} SceCommonDialogParam;

typedef struct SceNetCheckDialogAgeRestriction {
  char countryCode[2];
  char age;
  char padding;
} SceNetCheckDialogAgeRestriction;

typedef struct SceNetCheckDialogParam {
  int sdkVersion;
  SceCommonDialogParam commonParam;
  int mode;
  int npCommunicationId;
  int *ps3ConnectParam;
  void *groupName;
  int timeoutUs;
  char defaultAgeRestriction;
  char padding[3];
  int ageRestrictionCount;
  const SceNetCheckDialogAgeRestriction *ageRestriction;
  char reserved[104];
} SceNetCheckDialogParam;

static SceUID g_hooks[6];

static tai_hook_ref_t g_sceNetCheckDialogInit_hook;
static int sceNetCheckDialogInit_patched(SceNetCheckDialogParam *param) {
  int ret;
  if (param->mode == SCE_NETCHECK_DIALOG_MODE_PSN || param->mode == SCE_NETCHECK_DIALOG_MODE_PSN_ONLINE) {
    ret = 0;
  } else {
    ret = TAI_CONTINUE(int, g_sceNetCheckDialogInit_hook, param);
  }
  return ret;
}

static tai_hook_ref_t g_sceNetCheckDialogInit2_hook;
static int sceNetCheckDialogInit2_patched(SceNetCheckDialogParam *param, void *opt) {
  int ret;
  if (param->mode == SCE_NETCHECK_DIALOG_MODE_PSN || param->mode == SCE_NETCHECK_DIALOG_MODE_PSN_ONLINE) {
    ret = 0;
  } else {
    ret = TAI_CONTINUE(int, g_sceNetCheckDialogInit2_hook, param, opt);
  }
  return ret;
}

static tai_hook_ref_t g_sceNetCheckDialogAbort_hook;
static int sceNetCheckDialogAbort_patched(void) {
  int ret;
  ret = TAI_CONTINUE(int, g_sceNetCheckDialogAbort_hook);
  if (ret == SCE_COMMON_DIALOG_ERROR_NOT_IN_USE) {
    ret = 0;
  }
  return ret;
}

static tai_hook_ref_t g_sceNetCheckDialogGetResult_hook;
static int sceNetCheckDialogGetResult_patched(void *result) {
  int ret;
  ret = TAI_CONTINUE(int, g_sceNetCheckDialogGetResult_hook, result);
  if (ret == SCE_COMMON_DIALOG_ERROR_NOT_IN_USE) {
    ret = 0;
  }
  return ret;
}

static tai_hook_ref_t g_sceNetCheckDialogGetStatus_hook;
static int sceNetCheckDialogGetStatus_patched(void) {
  int ret;
  ret = TAI_CONTINUE(int, g_sceNetCheckDialogGetStatus_hook);
  if (ret == SCE_COMMON_DIALOG_ERROR_NOT_IN_USE || ret == SCE_COMMON_DIALOG_STATUS_NONE) {
    ret = SCE_COMMON_DIALOG_STATUS_FINISHED;
  }
  return ret;
}

static tai_hook_ref_t g_sceNetCheckDialogTerm_hook;
static int sceNetCheckDialogTerm_patched(void) {
  int ret;
  ret = TAI_CONTINUE(int, g_sceNetCheckDialogTerm_hook);
  if (ret == SCE_COMMON_DIALOG_ERROR_NOT_IN_USE) {
    ret = 0;
  }
  return ret;
}

void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args) {
  g_hooks[0] = taiHookFunctionImport(&g_sceNetCheckDialogInit_hook, 
                                      TAI_MAIN_MODULE, 
                                      0xE537816C, // SceCommonDialog
                                      0xA38A4A0D, 
                                      sceNetCheckDialogInit_patched);
  g_hooks[1] = taiHookFunctionImport(&g_sceNetCheckDialogInit2_hook, 
                                      TAI_MAIN_MODULE, 
                                      0xE537816C, // SceCommonDialog
                                      0x243D6A36, 
                                      sceNetCheckDialogInit2_patched);
  g_hooks[2] = taiHookFunctionImport(&g_sceNetCheckDialogAbort_hook, 
                                      TAI_MAIN_MODULE, 
                                      0xE537816C, // SceCommonDialog
                                      0x2D8EDF09, 
                                      sceNetCheckDialogAbort_patched);
  g_hooks[3] = taiHookFunctionImport(&g_sceNetCheckDialogGetResult_hook, 
                                      TAI_MAIN_MODULE, 
                                      0xE537816C, // SceCommonDialog
                                      0xB05FCE9E, 
                                      sceNetCheckDialogGetResult_patched);
  g_hooks[4] = taiHookFunctionImport(&g_sceNetCheckDialogGetStatus_hook, 
                                      TAI_MAIN_MODULE, 
                                      0xE537816C, // SceCommonDialog
                                      0x8027292A, 
                                      sceNetCheckDialogGetStatus_patched);
  g_hooks[5] = taiHookFunctionImport(&g_sceNetCheckDialogTerm_hook, 
                                      TAI_MAIN_MODULE, 
                                      0xE537816C, // SceCommonDialog
                                      0x8BE51C15, 
                                      sceNetCheckDialogTerm_patched);
  return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args) {
  if (g_hooks[0] >= 0) taiHookRelease(g_hooks[0], g_sceNetCheckDialogInit_hook);
  if (g_hooks[1] >= 0) taiHookRelease(g_hooks[1], g_sceNetCheckDialogInit2_hook);
  if (g_hooks[2] >= 0) taiHookRelease(g_hooks[2], g_sceNetCheckDialogAbort_hook);
  if (g_hooks[3] >= 0) taiHookRelease(g_hooks[3], g_sceNetCheckDialogGetResult_hook);
  if (g_hooks[4] >= 0) taiHookRelease(g_hooks[4], g_sceNetCheckDialogGetStatus_hook);
  if (g_hooks[5] >= 0) taiHookRelease(g_hooks[5], g_sceNetCheckDialogTerm_hook);
  return SCE_KERNEL_STOP_SUCCESS;
}
