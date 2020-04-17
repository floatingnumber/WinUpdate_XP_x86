#include "stdafx.h"
#include "winupdate.h"
#include "nfxdetect.h"
#include "fver.h"
#include "winsku.h"
#include "registry.h"
#include "certificates.h"
#include "kb980195.h"
#include "kb2900986.h"
#include <vector>
#include <string>

#define NN name->push_back
#define XX exe->push_back

using namespace std;

void windowsUpdates(std::vector<std::string>* name, std::vector<std::string>* exe,winsku sku,int sp,
					bool* options,std::vector<std::string> *notifications) {

	int status=0;

	// Index the array
	bool* rdp60   =options+0;
	bool* rdp61   =options+1;
	bool* rdp70   =options+2;
	bool* wmp6cdcs=options+3;
	bool* imapi2  =options+4;
	bool* smartcard=options+5;
	bool* wsearch4=options+6;
	bool* jview   =options+7;
	bool* pshell  =options+8;
	bool* winrms  =options+9;
	bool* msi45   =options+10;
	bool* msxml4  =options+11;
	bool* msxml6  =options+12;
	bool* bitlocker=options+13;
	bool* silverlight=options+14;
	bool* wga = options+15;
	bool* xpeos = options+16;
	bool* rktools = options+17;

	// Detect .NET Framework parameters
	int nfxServicePack[NFX_VERSION_COUNT];
	nfxDetect(nfxServicePack);
	int* nfx11=nfxServicePack+2;

	// Hotfix path and standard arguments
	const std::string sw="start /wait ";
	const std::string p="Packages\\";
	const std::string np="NetFx\\";
	const std::string a1=" /passive /norestart /overwriteoem /nobackup";
	const std::string a2=" /Q";
	const std::string a2l=" /q";
	const std::string a3=" /passive /norestart";
	const std::string a4=" /u /z";
	const std::string a5=" /wuforce /quiet /norestart";
	const std::string a6=" /passive /norestart /o /n";

	// Create SKU masks
	int XP_HOME_PRO = XP_PRO|XP_PRON|XP_HOME|XP_HOMEN;
	int XP_MCE = XP_MCE2002|XP_MCE2004|XP_MCE2005;
	int XP_OTHER_EDITIONS = XP_STARTER|XP_TABLET|XP_MCE;
	int XP_CLIENT = XP_HOME_PRO|XP_OTHER_EDITIONS;
	int XP_PLUS_FLP = XP_HOME_PRO|XP_OTHER_EDITIONS|XPE_FLP;
	int XPE_2009 = XPE_POSREADY2009|XPE_WES2009;
	int XP_EMBEDDED = XPE_2009|XPE_FLP;
	int XP_ALL =XP_HOME_PRO|XP_OTHER_EDITIONS|XP_EMBEDDED;

	// Identify system paths
	int CannotFindSystemRoot=0;
	int CannotFindProgramFiles=0;
	std::wstring SystemRoot = regQueryValue(L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",L"SystemRoot",&CannotFindSystemRoot);
	std::wstring WinSxS = SystemRoot + L"\\WinSxS";
	std::wstring System32 = SystemRoot + L"\\system32";
	std::wstring dllcache = System32 + L"\\dllcache";
	std::wstring msagent = SystemRoot + L"\\msagent";
	std::wstring ehome = SystemRoot + L"\\ehome";
	std::wstring CreateDisc = ehome + L"\\CreateDisc";
	std::wstring ehomeFilters = CreateDisc + L"\\Filters";
	std::wstring Sfxplugins = CreateDisc + L"\\Sfxplugins";
	std::wstring ehHelp = ehome + L"\\ehHelp";
	std::wstring NetworkDiagnostic = SystemRoot + L"\\Network Diagnostic";
	std::wstring usmt = System32 + L"\\usmt";
	std::wstring Flash = System32 + L"\\Macromed\\Flash";
	std::wstring wbem = System32 + L"\\wbem";
	std::wstring Drivers = System32 + L"\\Drivers";
	std::wstring DriverCache = SystemRoot + L"\\Driver Cache\\i386";
	std::wstring inetsrv = System32 + L"\\inetsrv";
	std::wstring Restore = System32 + L"\\Restore";

	std::wstring helpctr_binaries = SystemRoot + L"\\pchealth\\helpctr\\binaries";
	std::wstring UploadLB = SystemRoot + L"\\pchealth\\UploadLB\\Binaries";

	std::wstring ProgramFiles = regQueryValue(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion",L"ProgramFilesDir",&CannotFindProgramFiles);
	std::wstring InternetExplorer = ProgramFiles + L"\\Internet Explorer";
	std::wstring Accessories = ProgramFiles + L"\\Windows NT\\Accessories";
	std::wstring OutlookExpress = ProgramFiles + L"\\Outlook Express";
	std::wstring Messenger = ProgramFiles + L"\\Messenger";
	std::wstring MovieMaker = ProgramFiles + L"\\Movie Maker";
	std::wstring MicrosoftSilverlight = ProgramFiles + L"\\Microsoft Silverlight";
	std::wstring CommonFilesSystem = ProgramFiles + L"\\Common Files\\System";
	std::wstring msadc = CommonFilesSystem + L"\\msadc";
	std::wstring ado = CommonFilesSystem + L"\\ado";
	std::wstring OleDB = CommonFilesSystem + L"\\Ole DB";
	std::wstring TextConv = ProgramFiles + L"\\Common Files\\Microsoft Shared\\TextConv";
	std::wstring Triedit = ProgramFiles + L"\\Common Files\\Microsoft Shared\\Triedit";
	std::wstring VGX = ProgramFiles + L"\\Common Files\\Microsoft Shared\\VGX";
	std::wstring WDS = ProgramFiles + L"\\Windows Desktop Search";
	std::wstring WDS_mui = WDS + L"\\en-US";
	std::wstring jnl = ProgramFiles+L"\\Windows Journal Viewer";
	std::wstring WindowsMediaPlayer = ProgramFiles+L"\\Windows Media Player";
	std::wstring Ink = ProgramFiles+L"\\Common Files\\Microsoft Shared\\Ink";

	std::wstring wxp_x86_0409_v1 = System32+L"\\PreInstall\\WinSE\\wxp_x86_0409_v1";

	bool _secdrv_sys_exist   = fileExists(Drivers+L"\\secdrv.sys");
	bool kb891122 = false;
	bool kb900325 = false;
	bool kb913800 = false;

	// Read file version information
	fver zero = fver(0,0,0,0);

	fver _aclayers_dll = getFileVer(SystemRoot+L"\\AppPatch\\aclayers.dll",&status);
	fver _explorer_exe = getFileVer(SystemRoot+L"\\explorer.exe",&status);
	fver _hh_exe       = getFileVer(SystemRoot+L"\\hh.exe",&status);
	fver _kb913800_exe = getFileVer(SystemRoot+L"\\kb913800.exe",&status);

	fver _6to4svc_dll  = getFileVer(System32+L"\\6to4svc.dll",&status);
	fver _aaclient_dll = getFileVer(System32+L"\\aaclient.dll",&status);
	fver _aaclient_dll_mui = getFileVer(System32+L"\\en-US\\aaclient.dll.mui",&status);
	fver _advapi32_dll = getFileVer(System32+L"\\advapi32.dll",&status);
	fver _advpack_dll  = getFileVer(System32+L"\\advpack.dll",&status);
	fver _asycfilt_dll = getFileVer(System32+L"\\asycfilt.dll",&status);
	fver _atl_dll      = getFileVer(System32+L"\\atl.dll",&status);
	fver _atmfd_dll    = getFileVer(System32+L"\\atmfd.dll",&status);
	fver _authz_dll    = getFileVer(System32+L"\\authz.dll",&status);
	fver _avifil32_dll = getFileVer(System32+L"\\avifil32.dll",&status);
	fver _axaltocm_dll = getFileVer(System32+L"\\axaltocm.dll",&status);
	fver _basecsp_dll  = getFileVer(System32+L"\\basecsp.dll",&status);
	fver _bcsprsrc_dll = getFileVer(System32+L"\\bcsprsrc.dll",&status);
	fver _blackbox_dll = getFileVer(System32+L"\\blackbox.dll",&status);
	fver _browser_dll  = getFileVer(System32+L"\\browser.dll",&status);
	fver _browseui_dll = getFileVer(System32+L"\\browseui.dll",&status);
	fver _cabview_dll  = getFileVer(System32+L"\\cabview.dll",&status);
	fver _catsrv_dll   = getFileVer(System32+L"\\catsrv.dll",&status);
	fver _catsrvut_dll = getFileVer(System32+L"\\catsrvut.dll",&status);
	fver _cdfview_dll  = getFileVer(System32+L"\\cdfview.dll",&status);
	fver _cdosys_dll   = getFileVer(System32+L"\\cdosys.dll",&status);
	fver _cewmdm_dll   = getFileVer(System32+L"\\cewmdm.dll",&status);
	fver _ciodm_dll    = getFileVer(System32+L"\\ciodm.dll",&status);
	fver _clbcatex_dll = getFileVer(System32+L"\\clbcatex.dll",&status);
	fver _clbcatq_dll  = getFileVer(System32+L"\\clbcatq.dll",&status);
	fver _colbact_dll  = getFileVer(System32+L"\\colbact.dll",&status);
	fver _comctl32_dll = getFileVer(System32+L"\\comctl32.dll",&status);
	fver _comrepl_dll  = getFileVer(System32+L"\\comrepl.dll",&status);
	fver _comsvcs_dll  = getFileVer(System32+L"\\comsvcs.dll",&status);
	fver _comuid_dll   = getFileVer(System32+L"\\comuid.dll",&status);
	fver _corpol_dll   = getFileVer(System32+L"\\corpol.dll",&status);
	fver _credssp_dll  = getFileVer(System32+L"\\credssp.dll",&status);
	fver _crypt32_dll  = getFileVer(System32+L"\\crypt32.dll",&status);
	fver _cryptdlg_dll = getFileVer(System32+L"\\cryptdlg.dll",&status);
	fver _cscript_exe  = getFileVer(System32+L"\\cscript.exe",&status);
	fver _csrsrv_dll   = getFileVer(System32+L"\\csrsrv.dll",&status);
	fver _danim_dll    = getFileVer(System32+L"\\danim.dll",&status);
	fver _dao360_dll   = getFileVer(System32+L"\\dao360.dll",&status);
	fver _devenum_dll  = getFileVer(System32+L"\\devenum.dll",&status);
	fver _dhcpcsvc_dll = getFileVer(System32+L"\\dhcpcsvc.dll",&status);
	fver _dnsapi_dll   = getFileVer(System32+L"\\dnsapi.dll",&status);
	fver _dnsrslvr_dll = getFileVer(System32+L"\\dnsrslvr.dll",&status);
	fver _dpnet_dll    = getFileVer(System32+L"\\dpnet.dll",&status);
	fver _drmclien_dll = getFileVer(System32+L"\\drmclien.dll",&status);
	fver _drmstor_dll  = getFileVer(System32+L"\\drmstor.dll",&status);
	fver _drmupgds_exe = getFileVer(System32+L"\\drmupgds.exe",&status);
	fver _drmv2clt_dll = getFileVer(System32+L"\\drmv2clt.dll",&status);
	fver _dssenh_dll   = getFileVer(System32+L"\\dssenh.dll",&status);
	fver _dxmasf_dll   = getFileVer(System32+L"\\dxmasf.dll",&status);
	fver _dxtmsft_dll  = getFileVer(System32+L"\\dxtmsft.dll",&status);
	fver _dxtrans_dll  = getFileVer(System32+L"\\dxtrans.dll",&status);
	fver _encdec_dll   = getFileVer(System32+L"\\encdec.dll",&status);
	fver _es_dll       = getFileVer(System32+L"\\es.dll",&status);
	fver _esent_dll    = getFileVer(System32+L"\\esent.dll",&status);
	fver _extmgr_dll   = getFileVer(System32+L"\\extmgr.dll",&status);
	fver _fltlib_dll   = getFileVer(System32+L"\\fltlib.dll",&status);
	fver _fltMc_exe    = getFileVer(System32+L"\\fltMc.exe",&status);
	fver _fmifs_dll    = getFileVer(System32+L"\\fmifs.dll",&status);
	fver _fontsub_dll  = getFileVer(System32+L"\\fontsub.dll",&status);
	fver _format_com   = getFileVer(System32+L"\\format.com",&status);
	fver _fxscover_exe = getFileVer(System32+L"\\fxscover.exe",&status);
	fver _gdi32_dll    = getFileVer(System32+L"\\gdi32.dll",&status);
	fver _gpkcsp_dll   = getFileVer(System32+L"\\gpkcsp.dll",&status);
	fver _gpprefcl_dll = getFileVer(System32+L"\\gpprefcl.dll",&status);
	fver _hhctrl_ocx   = getFileVer(System32+L"\\hhctrl.ocx",&status);
	fver _hhsetup_dll  = getFileVer(System32+L"\\hhsetup.dll",&status);
	fver _hlink_dll    = getFileVer(System32+L"\\hlink.dll",&status);
	fver _html_iec     = getFileVer(System32+L"\\html.iec",&status);
	fver _httpapi_dll  = getFileVer(System32+L"\\httpapi.dll",&status);
	fver _hypertrm_dll = getFileVer(System32+L"\\hypertrm.dll",&status);
	fver _iacenc_dll   = getFileVer(System32+L"\\iacenc.dll",&status);
	fver _icardie_dll  = getFileVer(System32+L"\\icardie.dll",&status);
	fver _iccvid_dll   = getFileVer(System32+L"\\iccvid.dll",&status);
	fver _icm32_dll    = getFileVer(System32+L"\\icm32.dll",&status);
	fver _ie4uinit_exe = getFileVer(System32+L"\\ie4uinit.exe",&status);
	fver _ieakeng_dll  = getFileVer(System32+L"\\ieakeng.dll",&status);
	fver _ieaksie_dll  = getFileVer(System32+L"\\ieaksie.dll",&status);
	fver _ieakui_dll   = getFileVer(System32+L"\\ieakui.dll",&status);
	fver _ieapfltr_dat = getFileVer(System32+L"\\ieapfltr.dat",&status);
	fver _ieapfltr_dll = getFileVer(System32+L"\\ieapfltr.dll",&status);
	fver _iedkcs32_dll = getFileVer(System32+L"\\iedkcs32.dll",&status);
	fver _ieencode_dll = getFileVer(System32+L"\\ieencode.dll",&status);
	fver _ieframe_dll  = getFileVer(System32+L"\\ieframe.dll",&status);
	fver _ieframe_dll_mui = getFileVer(System32+L"\\ieframe.dll.mui",&status);
	fver _iepeers_dll  = getFileVer(System32+L"\\iepeers.dll",&status);
	fver _iernonce_dll = getFileVer(System32+L"\\iernonce.dll",&status);
	fver _iertutil_dll = getFileVer(System32+L"\\iertutil.dll",&status);
	fver _ieudinit_exe = getFileVer(System32+L"\\ieudinit.exe",&status);
	fver _ifsutil_dll  = getFileVer(System32+L"\\ifsutil.dll",&status);
	fver _ifxcardm_dll = getFileVer(System32+L"\\ifxcardm.dll",&status);
	fver _imagehlp_dll = getFileVer(System32+L"\\imagehlp.dll",&status);
	fver _imapi2_dll   = getFileVer(System32+L"\\imapi2.dll",&status);
	fver _imapi2fs_dll = getFileVer(System32+L"\\imapi2fs.dll",&status);
	fver _imekr61_ime  = getFileVer(System32+L"\\imekr61.ime",&status);
	fver _inetcomm_dll = getFileVer(System32+L"\\inetcomm.dll",&status);
	fver _inetcpl_cpl  = getFileVer(System32+L"\\inetcpl.cpl",&status);
	fver _inetpp_dll   = getFileVer(System32+L"\\inetpp.dll",&status);
	fver _inseng_dll   = getFileVer(System32+L"\\inseng.dll",&status);
	fver _iphlpapi_dll = getFileVer(System32+L"\\iphlpapi.dll",&status);
	fver _iprtrmgr_dll = getFileVer(System32+L"\\iprtrmgr.dll",&status);
	fver _isign32_dll  = getFileVer(System32+L"\\isign32.dll",&status);
	fver _itircl_dll   = getFileVer(System32+L"\\itircl.dll",&status);
	fver _itss_dll     = getFileVer(System32+L"\\itss.dll",&status);
	fver _iyuv_32_dll  = getFileVer(System32+L"\\iyuv_32.dll",&status);
	fver _jgdw400_dll  = getFileVer(System32+L"\\jgdw400.dll",&status);
	fver _jgpl400_dll  = getFileVer(System32+L"\\jgpl400.dll",&status);
	fver _jscript_dll  = getFileVer(System32+L"\\jscript.dll",&status);
	fver _jsproxy_dll  = getFileVer(System32+L"\\jsproxy.dll",&status);
	fver _kerberos_dll = getFileVer(System32+L"\\kerberos.dll",&status);
	fver _kernel32_dll = getFileVer(System32+L"\\kernel32.dll",&status);
	fver _l3codeca_acm = getFileVer(System32+L"\\l3codeca.acm",&status);
	fver _l3codecx_ax  = getFileVer(System32+L"\\l3codecx.ax",&status);
	fver _LAPRXY_dll   = getFileVer(System32+L"\\LAPRXY.dll",&status);
	fver _licmgr10_dll = getFileVer(System32+L"\\licmgr10.dll",&status);
	fver _linkinfo_dll = getFileVer(System32+L"\\linkinfo.dll",&status);
	fver _localspl_dll = getFileVer(System32+L"\\localspl.dll",&status);
	fver _logagent_exe = getFileVer(System32+L"\\logagent.exe",&status);
	fver _lsasrv_dll   = getFileVer(System32+L"\\lsasrv.dll",&status);
	fver _magnify_exe  = getFileVer(System32+L"\\magnify.exe",&status);
	fver _mciseq_dll   = getFileVer(System32+L"\\mciseq.dll",&status);
	fver _mf3216_dll   = getFileVer(System32+L"\\mf3216.dll",&status);
	fver _mfc40_dll    = getFileVer(System32+L"\\mfc40.dll",&status);
	fver _mfc40u_dll   = getFileVer(System32+L"\\mfc40u.dll",&status);
	fver _mfc42_dll    = getFileVer(System32+L"\\mfc42.dll",&status);
	fver _mfc42u_dll   = getFileVer(System32+L"\\mfc42u.dll",&status);
	fver _mfplat_dll   = getFileVer(System32+L"\\mfplat.dll",&status);
	fver _mp4sdecd_dll = getFileVer(System32+L"\\mp4sdecd.dll",&status);
	fver _mp4sdmod_dll = getFileVer(System32+L"\\mp4sdmod.dll",&status);
	fver _mp4sds32_ax  = getFileVer(System32+L"\\mp4sds32.ax",&status);
	fver _mpeg2data_ax = getFileVer(System32+L"\\mpeg2data.ax",&status);
	fver _mpg2splt_ax  = getFileVer(System32+L"\\mpg2splt.ax",&status);
	fver _mpg4ds32_ax  = getFileVer(System32+L"\\mpg4ds32.ax",&status);
	fver _mqqm_dll     = getFileVer(System32+L"\\mqqm.dll",&status);
	fver _mqad_dll     = getFileVer(System32+L"\\mqad.dll",&status);
	fver _mqbkup_exe   = getFileVer(System32+L"\\mqbkup.exe",&status);
	fver _mqdscli_dll  = getFileVer(System32+L"\\mqdscli.dll",&status);
	fver _mqise_dll    = getFileVer(System32+L"\\mqise.dll",&status);
	fver _mqoa_dll     = getFileVer(System32+L"\\mqoa.dll",&status);
	fver _mqrt_dll     = getFileVer(System32+L"\\mqrt.dll",&status);
	fver _mqrtdep_dll  = getFileVer(System32+L"\\mqrtdep.dll",&status);
	fver _mqsec_dll    = getFileVer(System32+L"\\mqsec.dll",&status);
	fver _mqsnap_dll   = getFileVer(System32+L"\\mqsnap.dll",&status);
	fver _mqsvc_exe    = getFileVer(System32+L"\\mqsvc.exe",&status);
	fver _mqtgsvc_exe  = getFileVer(System32+L"\\mqtgsvc.exe",&status);
	fver _mqtrig_dll   = getFileVer(System32+L"\\mqtrig.dll",&status);
	fver _mqupgrd_dll  = getFileVer(System32+L"\\mqupgrd.dll",&status);
	fver _mqutil_dll   = getFileVer(System32+L"\\mqutil.dll",&status);
	fver _mrt_exe      = getFileVer(System32+L"\\mrt.exe",&status);
	fver _msasn1_dll   = getFileVer(System32+L"\\msasn1.dll",&status);
	fver _msaud32_acm  = getFileVer(System32+L"\\msaud32.acm",&status);
	fver _mscms_dll    = getFileVer(System32+L"\\mscms.dll",&status);
	fver _msctf_dll    = getFileVer(System32+L"\\msctf.dll",&status);
	fver _msdrm_dll    = getFileVer(System32+L"\\msdrm.dll",&status);
	fver _msdtclog_dll = getFileVer(System32+L"\\msdtclog.dll",&status);
	fver _msdtcprx_dll = getFileVer(System32+L"\\msdtcprx.dll",&status);
	fver _msdtctm_dll  = getFileVer(System32+L"\\msdtctm.dll",&status);
	fver _msdtcuiu_dll = getFileVer(System32+L"\\msdtcuiu.dll",&status);
	fver _msdvbnp_ax   = getFileVer(System32+L"\\msdvbnp.ax",&status);
	fver _msexch40_dll = getFileVer(System32+L"\\msexch40.dll",&status);
	fver _msexcl40_dll = getFileVer(System32+L"\\msexcl40.dll",&status);
	fver _msfeeds_dll  = getFileVer(System32+L"\\msfeeds.dll",&status);
	fver _msfeedsbs_dll= getFileVer(System32+L"\\msfeedsbs.dll",&status);
	fver _msftedit_dll = getFileVer(System32+L"\\msftedit.dll",&status);
	fver _mshtml_dll   = getFileVer(System32+L"\\mshtml.dll",&status);
	fver _mshtmled_dll = getFileVer(System32+L"\\mshtmled.dll",&status);
	fver _msi_dll      = getFileVer(System32+L"\\msi.dll",&status);
	fver _msiexec_exe  = getFileVer(System32+L"\\msiexec.exe",&status);
	fver _msihnd_dll   = getFileVer(System32+L"\\msihnd.dll",&status);
	fver _msimsg_dll   = getFileVer(System32+L"\\msimsg.dll",&status);
	fver _msisip_dll   = getFileVer(System32+L"\\msisip.dll",&status);
	fver _msjet40_dll  = getFileVer(System32+L"\\msjet40.dll",&status);
	fver _msjetol1_dll = getFileVer(System32+L"\\msjetol1.dll",&status);
	fver _msjint40_dll = getFileVer(System32+L"\\msjint40.dll",&status);
	fver _msjter40_dll = getFileVer(System32+L"\\msjter40.dll",&status);
	fver _msjtes40_dll = getFileVer(System32+L"\\msjtes40.dll",&status);
	fver _msltus40_dll = getFileVer(System32+L"\\msltus40.dll",&status);
	fver _msmqocm_dll  = getFileVer(System32+L"\\Setup\\msmqocm.dll",&status);
	fver _msnetobj_dll = getFileVer(System32+L"\\msnetobj.dll",&status);
	fver _msnp_ax      = getFileVer(System32+L"\\msnp.ax",&status);
	fver _msorcl32_dll = getFileVer(System32+L"\\msorcl32.dll",&status);
	fver _mspaint_exe  = getFileVer(System32+L"\\mspaint.exe",&status);
	fver _mspbde40_dll = getFileVer(System32+L"\\mspbde40.dll",&status);
	fver _mspmsnsv_dll = getFileVer(System32+L"\\mspmsnsv.dll",&status);
	fver _mspmsp_dll   = getFileVer(System32+L"\\mspmsp.dll",&status);
	fver _msrating_dll = getFileVer(System32+L"\\msrating.dll",&status);
	fver _msrd2x40_dll = getFileVer(System32+L"\\msrd2x40.dll",&status);
	fver _msrd3x40_dll = getFileVer(System32+L"\\msrd3x40.dll",&status);
	fver _msrepl40_dll = getFileVer(System32+L"\\msrepl40.dll",&status);
	fver _msrle32_dll  = getFileVer(System32+L"\\msrle32.dll",&status);
	fver _msscp_dll    = getFileVer(System32+L"\\msscp.dll",&status);
	fver _mstext40_dll = getFileVer(System32+L"\\mstext40.dll",&status);
	fver _mstime_dll   = getFileVer(System32+L"\\mstime.dll",&status);
	fver _mstsc_exe    = getFileVer(System32+L"\\mstsc.exe",&status);
	fver _mstsc_exe_mui = getFileVer(System32+L"\\en-US\\mstsc.exe.mui",&status);
	fver _mstscax_dll  = getFileVer(System32+L"\\mstscax.dll",&status);
	fver _mstscax_dll_mui = getFileVer(System32+L"\\en-US\\mstscax.dll.mui",&status);
	fver _msv1_0_dll   = getFileVer(System32+L"\\msv1_0.dll",&status);
	fver _msvidc32_dll = getFileVer(System32+L"\\msvidc32.dll",&status);
	fver _msvidctl_dll = getFileVer(System32+L"\\msvidctl.dll",&status);
	fver _msw3prt_dll  = getFileVer(System32+L"\\msw3prt.dll",&status);
	fver _mswdat10_dll = getFileVer(System32+L"\\mswdat10.dll",&status);
	fver _mswebdvd_dll = getFileVer(System32+L"\\mswebdvd.dll",&status);
	fver _mswmdm_dll   = getFileVer(System32+L"\\mswmdm.dll",&status);
	fver _mswsock_dll  = getFileVer(System32+L"\\mswsock.dll",&status);
	fver _mswstr10_dll = getFileVer(System32+L"\\mswstr10.dll",&status);
	fver _msxbde40_dll = getFileVer(System32+L"\\msxbde40.dll",&status);
	fver _msxml3_dll   = getFileVer(System32+L"\\msxml3.dll",&status);
	fver _msxml4_dll   = getFileVer(System32+L"\\msxml4.dll",&status);
	fver _msxml4r_dll  = getFileVer(System32+L"\\msxml4r.dll",&status);
	fver _msxml6_dll   = getFileVer(System32+L"\\msxml6.dll",&status);
	fver _msxml6r_dll   = getFileVer(System32+L"\\msxml6r.dll",&status);
	fver _msyuv_dll    = getFileVer(System32+L"\\msyuv.dll",&status);
	fver _mtxclu_dll   = getFileVer(System32+L"\\mtxclu.dll",&status);
	fver _mtxoci_dll   = getFileVer(System32+L"\\mtxoci.dll",&status);
	fver _narrator_exe = getFileVer(System32+L"\\narrator.exe",&status);
	fver _netapi32_dll = getFileVer(System32+L"\\netapi32.dll",&status);
	fver _netman_dll   = getFileVer(System32+L"\\netman.dll",&status);
	fver _netshell_dll = getFileVer(System32+L"\\netshell.dll",&status);
	fver _ntdll_dll    = getFileVer(System32+L"\\ntdll.dll",&status);
	fver _ntkrnlmp_exe = getFileVer(System32+L"\\ntkrnlmp.exe",&status);
	fver _ntkrnlpa_exe = getFileVer(System32+L"\\ntkrnlpa.exe",&status);
	fver _ntkrpamp_exe = getFileVer(System32+L"\\ntkrpamp.exe",&status);
	fver _ntoskrnl_exe = getFileVer(System32+L"\\ntoskrnl.exe",&status);
	fver _nwapi32_dll  = getFileVer(System32+L"\\nwapi32.dll",&status);
	fver _nwprovau_dll = getFileVer(System32+L"\\nwprovau.dll",&status);
	fver _nwwks_dll    = getFileVer(System32+L"\\nwwks.dll",&status);
	fver _oakley_dll   = getFileVer(System32+L"\\oakley.dll",&status);
	fver _odbc32_dll   = getFileVer(System32+L"\\odbc32.dll",&status);
	fver _occache_dll  = getFileVer(System32+L"\\occache.dll",&status);
	fver _ole32_dll    = getFileVer(System32+L"\\ole32.dll",&status);
	fver _oleacc_dll   = getFileVer(System32+L"\\oleacc.dll",&status);
	fver _oleaccrc_dll = getFileVer(System32+L"\\oleaccrc.dll",&status);
	fver _oleaut32_dll = getFileVer(System32+L"\\oleaut32.dll",&status);
	fver _olecli32_dll = getFileVer(System32+L"\\olecli32.dll",&status);
	fver _olecnv32_dll = getFileVer(System32+L"\\olecnv32.dll",&status);
	fver _oledlg_dll   = getFileVer(System32+L"\\oledlg.dll",&status);
	fver _oleprn_dll   = getFileVer(System32+L"\\oleprn.dll",&status);
	fver _olepro32_dll = getFileVer(System32+L"\\olepro32.dll",&status);
	fver _osk_exe      = getFileVer(System32+L"\\osk.exe",&status);
	fver _p2p_dll      = getFileVer(System32+L"\\p2p.dll",&status);
	fver _p2pgasvc_dll = getFileVer(System32+L"\\p2pgasvc.dll",&status);
	fver _p2pgraph_dll = getFileVer(System32+L"\\p2pgraph.dll",&status);
	fver _p2pnetsh_dll = getFileVer(System32+L"\\p2pnetsh.dll",&status);
	fver _p2psvc_dll   = getFileVer(System32+L"\\p2psvc.dll",&status);
	fver _packager_exe = getFileVer(System32+L"\\packager.exe",&status);
	fver _photometadatahandler_dll = getFileVer(System32+L"\\photometadatahandler.dll",&status);
	fver _pintool_exe  = getFileVer(System32+L"\\pintool.exe",&status);
	fver _pnrpnsp_dll  = getFileVer(System32+L"\\pnrpnsp.dll",&status);
	fver _pdh_dll      = getFileVer(System32+L"\\pdh.dll",&status);
	fver _pngfilt_dll  = getFileVer(System32+L"\\pngfilt.dll",&status);
	fver _printui_dll  = getFileVer(System32+L"\\printui.dll",&status);
	fver _psisdecd_dll = getFileVer(System32+L"\\psisdecd.dll",&status);
	fver _psisrndr_ax  = getFileVer(System32+L"\\psisrndr.ax",&status);
	fver _qasf_dll     = getFileVer(System32+L"\\qasf.dll",&status);
	fver _qdvd_dll     = getFileVer(System32+L"\\qdvd.dll",&status);
	fver _qedit_dll    = getFileVer(System32+L"\\qedit.dll",&status);
	fver _quartz_dll   = getFileVer(System32+L"\\quartz.dll",&status);
	fver _query_dll    = getFileVer(System32+L"\\query.dll",&status);
	fver _rasadhlp_dll = getFileVer(System32+L"\\rasadhlp.dll",&status);
	fver _raschap_dll  = getFileVer(System32+L"\\raschap.dll",&status);
	fver _rasctrs_dll  = getFileVer(System32+L"\\rasctrs.dll",&status);
	fver _rasdlg_dll   = getFileVer(System32+L"\\rasdlg.dll",&status);
	fver _rasmans_dll  = getFileVer(System32+L"\\rasmans.dll",&status);
	fver _rasmxs_dll   = getFileVer(System32+L"\\rasmxs.dll",&status);
	fver _rastls_dll   = getFileVer(System32+L"\\rastls.dll",&status);
	fver _remotesp_tsp = getFileVer(System32+L"\\remotesp.tsp",&status);
	fver _rhttpaa_dll  = getFileVer(System32+L"\\rhttpaa.dll",&status);
	fver _riched20_dll = getFileVer(System32+L"\\riched20.dll",&status);
	fver _RmActivate_exe = getFileVer(System32+L"\\RmActivate.exe",&status);
	fver _RmActivate_isv_exe = getFileVer(System32+L"\\RmActivate_isv.exe",&status);
	fver _RmActivate_ssp_exe = getFileVer(System32+L"\\RmActivate_ssp.exe",&status);
	fver _RmActivate_ssp_isv_exe = getFileVer(System32+L"\\RmActivate_ssp_isv.exe",&status);
	fver _rpcrt4_dll   = getFileVer(System32+L"\\rpcrt4.dll",&status);
	fver _rpcss_dll    = getFileVer(System32+L"\\rpcss.dll",&status);
	fver _rsaenh_dll   = getFileVer(System32+L"\\rsaenh.dll",&status);
	fver _safrslv_dll  = getFileVer(System32+L"\\safrslv.dll",&status);
	fver _samsrv_dll   = getFileVer(System32+L"\\samsrv.dll",&status);
	fver _sbe_dll      = getFileVer(System32+L"\\sbe.dll",&status);
	fver _sc_exe       = getFileVer(System32+L"\\sc.exe",&status);
	fver _scesrv_dll   = getFileVer(System32+L"\\scesrv.dll",&status);
	fver _schannel_dll = getFileVer(System32+L"\\schannel.dll",&status);
	fver _scrobj_dll   = getFileVer(System32+L"\\scrobj.dll",&status);
	fver _scrrun_dll   = getFileVer(System32+L"\\scrrun.dll",&status);
	fver _seclogon_dll = getFileVer(System32+L"\\seclogon.dll",&status);
	fver _SecProc_dll  = getFileVer(System32+L"\\SecProc.dll",&status);
	fver _SecProc_isv_dll = getFileVer(System32+L"\\SecProc_isv.dll",&status);
	fver _SecProc_ssp_dll = getFileVer(System32+L"\\SecProc_ssp.dll",&status);
	fver _SecProc_ssp_isv_dll = getFileVer(System32+L"\\SecProc_ssp_isv.dll",&status);
	fver _secur32_dll  = getFileVer(System32+L"\\secur32.dll",&status);
	fver _services_exe = getFileVer(System32+L"\\services.exe",&status);
	fver _shdocvw_dll  = getFileVer(System32+L"\\shdocvw.dll",&status);
	fver _shell32_dll  = getFileVer(System32+L"\\shell32.dll",&status);
	fver _shimgvw_dll  = getFileVer(System32+L"\\shimgvw.dll",&status);
	fver _shlwapi_dll  = getFileVer(System32+L"\\shlwapi.dll",&status);
	fver _shsvcs_dll   = getFileVer(System32+L"\\shsvcs.dll",&status);
	fver _snmp_exe     = getFileVer(System32+L"\\snmp.exe",&status);
	fver _spoolsv_exe  = getFileVer(System32+L"\\spoolsv.exe",&status);
	fver _srvsvc_dll   = getFileVer(System32+L"\\srvsvc.dll",&status);
	fver _stclient_dll = getFileVer(System32+L"\\stclient.dll",&status);
	fver _strmdll_dll  = getFileVer(System32+L"\\strmdll.dll",&status);
	fver _strmfilt_dll = getFileVer(System32+L"\\strmfilt.dll",&status);
	fver _sxs_dll      = getFileVer(System32+L"\\sxs.dll",&status);
	fver _synceng_dll  = getFileVer(System32+L"\\synceng.dll",&status);
	fver _t2embed_dll  = getFileVer(System32+L"\\t2embed.dll",&status);
	fver _tapisrv_dll  = getFileVer(System32+L"\\tapisrv.dll",&status);
	fver _tdc_ocx      = getFileVer(System32+L"\\tdc.ocx",&status);
	fver _telnet_exe   = getFileVer(System32+L"\\telnet.exe",&status);
	fver _termsrv_dll  = getFileVer(System32+L"\\termsrv.dll",&status);
	fver _themeui_dll  = getFileVer(System32+L"\\themeui.dll",&status);
	fver _tlntsess_exe = getFileVer(System32+L"\\tlntsess.exe",&status);
	fver _tsbyuv_dll   = getFileVer(System32+L"\\tsbyuv.dll",&status);
	fver _tsgqec_dll   = getFileVer(System32+L"\\tsgqec.dll",&status);
	fver _tspkg_dll    = getFileVer(System32+L"\\tspkg.dll",&status);
	fver _txflog_dll   = getFileVer(System32+L"\\txflog.dll",&status);
	fver _tzchange_exe = getFileVer(System32+L"\\tzchange.exe",&status);
	fver _uexfat_dll   = getFileVer(System32+L"\\uexfat.dll",&status);
	fver _uiautomationcore_dll = getFileVer(System32+L"\\uiautomationcore.dll",&status);
	fver _ulib_dll     = getFileVer(System32+L"\\ulib.dll",&status);
	fver _umandlg_dll  = getFileVer(System32+L"\\umandlg.dll",&status);
	fver _umpnpmgr_dll = getFileVer(System32+L"\\umpnpmgr.dll",&status);
	fver _unregmp2_exe = getFileVer(SystemRoot+L"\\INF\\unregmp2.exe",&status);
	fver _upnphost_dll = getFileVer(System32+L"\\upnphost.dll",&status);
	fver _url_dll      = getFileVer(System32+L"\\url.dll",&status);
	fver _urlmon_dll   = getFileVer(System32+L"\\urlmon.dll",&status);
	fver _user32_dll   = getFileVer(System32+L"\\user32.dll",&status);
	fver _userenv_dll  = getFileVer(System32+L"\\userenv.dll",&status);
	fver _usp10_dll    = getFileVer(System32+L"\\usp10.dll",&status);
	fver _utilman_exe  = getFileVer(System32+L"\\utilman.exe",&status);
	fver _uwdf_exe     = getFileVer(System32+L"\\uwdf.exe",&status);
	fver _vbicodec_ax  = getFileVer(System32+L"\\vbicodec.ax",&status);
	fver _vbscript_dll = getFileVer(System32+L"\\vbscript.dll",&status);
	fver _verclsid_exe = getFileVer(System32+L"\\verclsid.exe",&status);
	fver _wdfapi_dll   = getFileVer(System32+L"\\wdfapi.dll",&status);
	fver _WdfCoInstaller01007_dll = getFileVer(System32+L"\\WdfCoInstaller01007.dll",&status);
	fver _wdfmgr_exe   = getFileVer(System32+L"\\wdfmgr.exe",&status);
	fver _wdigest_dll  = getFileVer(System32+L"\\wdigest.dll",&status);
	fver _webcheck_dll = getFileVer(System32+L"\\webcheck.dll",&status);
	fver _webclnt_dll  = getFileVer(System32+L"\\webclnt.dll",&status);
	fver _wiaservc_dll = getFileVer(System32+L"\\wiaservc.dll",&status);
	fver _win32k_sys   = getFileVer(System32+L"\\win32k.sys",&status);
	fver _win32spl_dll = getFileVer(System32+L"\\win32spl.dll",&status);
	fver _windowscodecs_dll = getFileVer(System32+L"\\windowscodecs.dll",&status);
	fver _windowscodecsext_dll = getFileVer(System32+L"\\windowscodecsext.dll",&status);
	fver _winhttp_dll  = getFileVer(System32+L"\\winhttp.dll",&status);
	fver _wininet_dll  = getFileVer(System32+L"\\wininet.dll",&status);
	fver _winmm_dll    = getFileVer(System32+L"\\winmm.dll",&status);
	fver _winspool_drv = getFileVer(System32+L"\\winspool.drv",&status);
	fver _winsrv_dll   = getFileVer(System32+L"\\winsrv.dll",&status);
	fver _wintrust_dll = getFileVer(System32+L"\\wintrust.dll",&status);
	fver _winusbcoinstaller_dll = getFileVer(System32+L"\\winusbcoinstaller.dll",&status);
	fver _wksprt_exe   = getFileVer(System32+L"\\wksprt.exe",&status);
	fver _wkssvc_dll   = getFileVer(System32+L"\\wkssvc.dll",&status);
	fver _wldap32_dll  = getFileVer(System32+L"\\wldap32.dll",&status);
	fver _WMADMOD_dll  = getFileVer(System32+L"\\WMADMOD.dll",&status);
	fver _WMADMOE_dll  = getFileVer(System32+L"\\WMADMOE.dll",&status);
	fver _wmasf_dll    = getFileVer(System32+L"\\wmasf.dll",&status);
	fver _wmavds32_ax  = getFileVer(System32+L"\\wmavds32.ax",&status);
	fver _wmdmlog_dll  = getFileVer(System32+L"\\wmdmlog.dll",&status);
	fver _wmdmps_dll   = getFileVer(System32+L"\\wmdmps.dll",&status);
	fver _wmdrmdev_dll = getFileVer(System32+L"\\wmdrmdev.dll",&status);
	fver _wmdrmnet_dll = getFileVer(System32+L"\\wmdrmnet.dll",&status);
	fver _wmdrmsdk_dll = getFileVer(System32+L"\\wmdrmsdk.dll",&status);
	fver _wmidx_dll    = getFileVer(System32+L"\\wmidx.dll",&status);
	fver _wmnetmgr_dll = getFileVer(System32+L"\\wmnetmgr.dll",&status);
	fver _wmp_dll      = getFileVer(System32+L"\\wmp.dll",&status);
	fver _wmpdxm_dll   = getFileVer(System32+L"\\wmpdxm.dll",&status);
	fver _wmpeffects_dll = getFileVer(System32+L"\\wmpeffects.dll",&status);
	fver _wmphoto_dll  = getFileVer(System32+L"\\wmphoto.dll",&status);
	fver _wmploc_dll   = getFileVer(System32+L"\\wmploc.dll",&status);
	fver _wmsdmod_dll  = getFileVer(System32+L"\\wmsdmod.dll",&status);
	fver _wmsdmoe2_dll = getFileVer(System32+L"\\wmsdmoe2.dll",&status);
	fver _wmspdmod_dll = getFileVer(System32+L"\\wmspdmod.dll",&status);
	fver _WMSPDMOE_dll = getFileVer(System32+L"\\WMSPDMOE.dll",&status);
	fver _wmv9vcm_dll  = getFileVer(System32+L"\\wmv9vcm.dll",&status);
	fver _WMVADVD_dll  = getFileVer(System32+L"\\WMVADVD.dll",&status);
	fver _WMVADVE_DLL  = getFileVer(System32+L"\\WMVADVE.DLL",&status);
	fver _wmvcore_dll  = getFileVer(System32+L"\\wmvcore.dll",&status);
	fver _wmvdecod_dll = getFileVer(System32+L"\\wmvdecod.dll",&status);
	fver _wmvdmod_dll  = getFileVer(System32+L"\\wmvdmod.dll",&status);
	fver _wmvdmoe2_dll = getFileVer(System32+L"\\wmvdmoe2.dll",&status);
	fver _wpdconns_dll = getFileVer(System32+L"\\wpdconns.dll",&status);
	fver _wpdmtp_dll   = getFileVer(System32+L"\\wpdmtp.dll",&status);
	fver _wpdmtpdr_dll = getFileVer(System32+L"\\drivers\\UMDF\\wpdmtpdr.dll",&status);
	fver _wpdmtpus_dll = getFileVer(System32+L"\\wpdmtpus.dll",&status);
	fver _wpdsp_dll    = getFileVer(System32+L"\\wpdsp.dll",&status);
	fver _wpdtrace_dll = getFileVer(System32+L"\\wpdtrace.dll",&status);
	fver _wpd_ci_dll   = getFileVer(System32+L"\\wpd_ci.dll",&status);
	fver _ws2_32_dll   = getFileVer(System32+L"\\ws2_32.dll",&status);
	fver _wscript_exe  = getFileVer(System32+L"\\wscript.exe",&status);
	fver _wshext_dll   = getFileVer(System32+L"\\wshext.dll",&status);
	fver _wshom_ocx    = getFileVer(System32+L"\\wshom.ocx",&status);
	fver _wsnmp32_dll  = getFileVer(System32+L"\\wsnmp32.dll",&status);
	fver _wstpager_ax  = getFileVer(System32+L"\\wstpager.ax",&status);
	fver _wstrenderer_ax = getFileVer(System32+L"\\wstrenderer.ax",&status);
	fver _wudfupdate_01007_dll  = getFileVer(System32+L"\\wudfupdate_01007.dll",&status);
	fver _wudfusbcciddriver_dll = getFileVer(System32+L"\\drivers\\UMDF\\wudfusbcciddriver.dll",&status);
	fver _wzcdlg_dll   = getFileVer(System32+L"\\wzcdlg.dll",&status);
	fver _wzcsapi_dll  = getFileVer(System32+L"\\wzcsapi.dll",&status);
	fver _wzcsvc_dll   = getFileVer(System32+L"\\wzcsvc.dll",&status);
	fver _xmllite_dll  = getFileVer(System32+L"\\xmllite.dll",&status);
	fver _xolehlp_dll  = getFileVer(System32+L"\\xolehlp.dll",&status);
	fver _xp_eos_exe   = getFileVer(System32+L"\\xp_eos.exe",&status);
	fver _xpsp4res_dll = getFileVer(System32+L"\\xpsp4res.dll",&status);
	fver _zipfldr_dll  = getFileVer(System32+L"\\zipfldr.dll",&status);

	// Windows Search System32 Files
	fver _msscntrs_dll  = getFileVer(System32+L"\\msscntrs.dll",&status);
	fver _mssitlb_dll  = getFileVer(System32+L"\\mssitlb.dll",&status);
	fver _searchprotocolhost_exe  = getFileVer(System32+L"\\searchprotocolhost.exe",&status);
	fver _searchfilterhost_exe  = getFileVer(System32+L"\\searchfilterhost.exe",&status);
	fver _searchindexer_exe  = getFileVer(System32+L"\\searchindexer.exe",&status);
	fver _mssph_dll  = getFileVer(System32+L"\\mssph.dll",&status);
	fver _mssphtb_dll  = getFileVer(System32+L"\\mssphtb.dll",&status);
	fver _mssprxy_dll  = getFileVer(System32+L"\\mssprxy.dll",&status);
	fver _mssrch_dll  = getFileVer(System32+L"\\mssrch.dll",&status);
	fver _msstrc_dll  = getFileVer(System32+L"\\msstrc.dll",&status);
	fver _propdefs_dll  = getFileVer(System32+L"\\propdefs.dll",&status);
	fver _tquery_dll  = getFileVer(System32+L"\\tquery.dll",&status);
	fver _msscb_dll  = getFileVer(System32+L"\\msscb.dll",&status);
	fver _msshooks_dll  = getFileVer(System32+L"\\msshooks.dll",&status);
	fver _msshsq_dll  = getFileVer(System32+L"\\msshsq.dll",&status);
	fver _xmlfilter_dll  = getFileVer(System32+L"\\xmlfilter.dll",&status);
	fver _rtffilt_dll  = getFileVer(System32+L"\\rtffilt.dll",&status);
	fver _srchadmin_dll  = getFileVer(System32+L"\\srchadmin.dll",&status);
	fver _propsys_dll  = getFileVer(System32+L"\\propsys.dll",&status);
	fver _searchindexer_exe_mui  = getFileVer(System32+L"\\searchindexer.exe.mui",&status);
	fver _mssrch_dll_mui  = getFileVer(System32+L"\\mssrch.dll.mui",&status);
	fver _tquery_dll_mui  = getFileVer(System32+L"\\tquery.dll.mui",&status);
	fver _mssphtb_dll_mui  = getFileVer(System32+L"\\mssphtb.dll.mui",&status);
	fver _mssph_dll_mui  = getFileVer(System32+L"\\mssph.dll.mui",&status);
	fver _srchadmin_dll_mui  = getFileVer(System32+L"\\srchadmin.dll.mui",&status);
	fver _propsys_dll_mui  = getFileVer(System32+L"\\propsys.dll.mui",&status);
	fver _oeph_dll  = getFileVer(System32+L"\\oeph.dll",&status);
	fver _oephres_dll  = getFileVer(System32+L"\\oephres.dll",&status);
	fver _uncph_dll  = getFileVer(System32+L"\\uncph.dll",&status);
	fver _uncres_dll  = getFileVer(System32+L"\\uncres.dll",&status);
	fver _unccplext_dll  = getFileVer(System32+L"\\unccplext.dll",&status);
	fver _uncdms_dll  = getFileVer(System32+L"\\uncdms.dll",&status);
	fver _uncne_dll  = getFileVer(System32+L"\\uncne.dll",&status);

	// Windows XP Media Center Edition files
	fver _audiodepthconverter_ax = getFileVer(ehomeFilters+L"\\audiodepthconverter.ax",&status);
	fver _directshowtap_ax       = getFileVer(ehomeFilters+L"\\directshowtap.ax",&status);
	fver _sonicmpegaudio_dll     = getFileVer(ehomeFilters+L"\\sonicmpegaudio.dll",&status);
	fver _sonicmpegsplitter_dll  = getFileVer(ehomeFilters+L"\\sonicmpegsplitter.dll",&status);
	fver _sonicmpegvideo_dll     = getFileVer(ehomeFilters+L"\\sonicmpegvideo.dll",&status);
	fver _rtstreamsink_ax        = getFileVer(ehomeFilters+L"\\rtstreamsink.ax",&status);

	fver _bdatunepia_dll=getFileVer(ehome+L"\\bdatunepia.dll",&status);
	fver _ehcett_dll   = getFileVer(ehome+L"\\ehcett.dll",&status);
	fver _ehchhime_dll = getFileVer(ehome+L"\\ehchhime.dll",&status);
	fver _ehchtime_dll = getFileVer(ehome+L"\\ehchtime.dll",&status);
	fver _ehcir_dll    = getFileVer(ehome+L"\\ehcir.dll",&status);
	fver _ehcm_dll     = getFileVer(ehome+L"\\ehcm.dll",&status);
	fver _ehcommon_dll = getFileVer(ehome+L"\\ehcommon.dll",&status);
	fver _ehcyrtt_dll  = getFileVer(ehome+L"\\ehcyrtt.dll",&status);
	fver _ehdebug_dll  = getFileVer(ehome+L"\\ehdebug.dll",&status);
	fver _ehdrop_dll   = getFileVer(ehome+L"\\ehdrop.dll",&status);
	fver _ehentt_dll   = getFileVer(ehome+L"\\ehentt.dll",&status);
	fver _ehepg_dll    = getFileVer(ehome+L"\\ehepg.dll",&status);
	fver _ehepgdat_dll = getFileVer(ehome+L"\\ehepgdat.dll",&status);
	fver _ehextcom_dll = getFileVer(ehome+L"\\ehextcom.dll",&status);
	fver _ehexthost_exe= getFileVer(ehome+L"\\ehexthost.exe",&status);
	fver _ehglid_dll   = getFileVer(ehome+L"\\ehglid.dll",&status);
	fver _ehiextcom_dll= getFileVer(ehome+L"\\ehiextcom.dll",&status);
	fver _ehiextens_dll= getFileVer(ehome+L"\\ehiextens.dll",&status);
	fver _ehiplay_dll  = getFileVer(ehome+L"\\ehiplay.dll",&status);
	fver _ehiproxy_dll = getFileVer(ehome+L"\\ehiproxy.dll",&status);
	fver _ehiuserxp_dll= getFileVer(ehome+L"\\ehiuserxp.dll",&status);
	fver _ehividctl_dll= getFileVer(ehome+L"\\ehividctl.dll",&status);
	fver _ehiwuapi_dll = getFileVer(ehome+L"\\ehiwuapi.dll",&status);
	fver _ehjpnime_dll = getFileVer(ehome+L"\\ehjpnime.dll",&status);
	fver _ehkeyctl_dll = getFileVer(ehome+L"\\ehkeyctl.dll",&status);
	fver _ehkorime_dll = getFileVer(ehome+L"\\ehkorime.dll",&status);
	fver _ehmcxins_exe = getFileVer(ehome+L"\\ehmcxins.exe",&status);
	fver _ehmsas_exe   = getFileVer(ehome+L"\\ehmsas.exe",&status);
	fver _ehplayer_dll = getFileVer(ehome+L"\\ehplayer.dll",&status);
	fver _ehproxy_dll  = getFileVer(ehome+L"\\ehproxy.dll",&status);
	fver _ehrec_exe    = getFileVer(ehome+L"\\ehrec.exe",&status);
	fver _ehrecobj_dll = getFileVer(ehome+L"\\ehrecobj.dll",&status);
	fver _ehrecvr_exe  = getFileVer(ehome+L"\\ehrecvr.exe",&status);
	fver _ehres_dll    = getFileVer(ehome+L"\\ehres.dll",&status);
	fver _ehreschs_dll  = getFileVer(ehome+L"\\zh-chs\\ehepg.resources.dll",&status);
	fver _ehrescht_dll  = getFileVer(ehome+L"\\zh-cht\\ehepg.resources.dll",&status);
	fver _ehrescs_dll  = getFileVer(ehome+L"\\cs\\ehepg.resources.dll",&status);
	fver _ehresda_dll  = getFileVer(ehome+L"\\da\\ehepg.resources.dll",&status);
	fver _ehresde_dll  = getFileVer(ehome+L"\\de\\ehepg.resources.dll",&status);
	fver _ehreses_dll  = getFileVer(ehome+L"\\es\\ehepg.resources.dll",&status);
	fver _ehresfi_dll  = getFileVer(ehome+L"\\fi\\ehepg.resources.dll",&status);
	fver _ehresfr_dll  = getFileVer(ehome+L"\\fr\\ehepg.resources.dll",&status);
	fver _ehresit_dll  = getFileVer(ehome+L"\\it\\ehepg.resources.dll",&status);
	fver _ehresja_dll  = getFileVer(ehome+L"\\ja\\ehepg.resources.dll",&status);
	fver _ehresko_dll  = getFileVer(ehome+L"\\ko\\ehepg.resources.dll",&status);
	fver _ehresnl_dll  = getFileVer(ehome+L"\\nl\\ehepg.resources.dll",&status);
	fver _ehresno_dll  = getFileVer(ehome+L"\\no\\ehepg.resources.dll",&status);
	fver _ehrespl_dll  = getFileVer(ehome+L"\\pl\\ehepg.resources.dll",&status);
	fver _ehrespt_dll  = getFileVer(ehome+L"\\pt\\ehepg.resources.dll",&status);
	fver _ehresru_dll  = getFileVer(ehome+L"\\ru\\ehepg.resources.dll",&status);
	fver _ehressv_dll  = getFileVer(ehome+L"\\sv\\ehepg.resources.dll",&status);
	fver _ehrestr_dll  = getFileVer(ehome+L"\\tr\\ehepg.resources.dll",&status);
	fver _ehsched_exe  = getFileVer(ehome+L"\\ehsched.exe",&status);
	fver _ehshell_exe  = getFileVer(ehome+L"\\ehshell.exe",&status);
	fver _ehtktt_dll   = getFileVer(ehome+L"\\ehtktt.dll",&status);
	fver _ehtrace_dll  = getFileVer(ehome+L"\\ehtrace.dll",&status);
	fver _ehtray_exe   = getFileVer(ehome+L"\\ehtray.exe",&status);
	fver _ehui_dll     = getFileVer(ehome+L"\\ehui.dll",&status);
	fver _ehuihlp_dll  = getFileVer(ehome+L"\\ehuihlp.dll",&status);
	fver _mcrdsvc_exe  = getFileVer(ehome+L"\\mcrdsvc.exe",&status);
	fver _mcrmgr_dll   = getFileVer(ehome+L"\\mcrmgr.dll",&status);
	fver _mcrmgr_exe   = getFileVer(ehome+L"\\mcrmgr.exe",&status);
	fver _medctrro_exe = getFileVer(ehome+L"\\medctrro.exe",&status);
	fver _microsoft_mediacenter_dll = getFileVer(ehome+L"\\Microsoft.MediaCenter.dll",&status);
	fver _mstvcapn_dll = getFileVer(ehome+L"\\mstvcapn.dll",&status);
	fver _sqldb20_dll  = getFileVer(ehome+L"\\sqldb20.dll",&status);
	fver _sqlqp20_dll  = getFileVer(ehome+L"\\sqlqp20.dll",&status);
	fver _sqlse20_dll  = getFileVer(ehome+L"\\sqlse20.dll",&status);

	fver _ContextTagger_dll = getFileVer(Ink+L"\\ContextTagger.dll",&status);
	fver _TabTip_exe        = getFileVer(Ink+L"\\TabTip.exe",&status);
	fver _TCServer_exe      = getFileVer(Ink+L"\\TCServer.exe",&status);
	fver _TipLibrary_dll    = getFileVer(Ink+L"\\TipLibrary.dll",&status);
	fver _tiptsf_dll        = getFileVer(Ink+L"\\tiptsf.dll",&status);

	fver _standardfx_plugin_dll = getFileVer(Sfxplugins+L"\\standardfx_plugin.dll",&status);

	fver _tenfoothelp_exe = getFileVer(ehHelp+L"\\tenfoothelp.exe",&status);

	fver _authorscript_dll       = getFileVer(CreateDisc+L"\\authorscript.dll",&status);
	fver _createdisc_dll         = getFileVer(CreateDisc+L"\\createdisc.dll",&status);
	fver _mce_wmadrm_dll         = getFileVer(CreateDisc+L"\\mce_wmadrm.dll",&status);
	fver _primosdk_dll           = getFileVer(CreateDisc+L"\\primosdk.dll",&status);
	fver _px_dll                 = getFileVer(CreateDisc+L"\\px.dll",&status);
	fver _pxdrv_dll              = getFileVer(CreateDisc+L"\\pxdrv.dll",&status);
	fver _pxhpinst_exe           = getFileVer(CreateDisc+L"\\pxhpinst.exe",&status);
	fver _pxmas_dll              = getFileVer(CreateDisc+L"\\pxmas.dll",&status);
	fver _pxwavedec_dll          = getFileVer(CreateDisc+L"\\pxwavedec.dll",&status);
	fver _pxwma_dll              = getFileVer(CreateDisc+L"\\pxwma.dll",&status);
	fver _sbeserver_exe          = getFileVer(CreateDisc+L"\\sbeserver.exe",&status);
	fver _sbeserverps_dll        = getFileVer(CreateDisc+L"\\sbeserverps.dll",&status);
	fver _simpleburnuires_dll    = getFileVer(CreateDisc+L"\\simpleburnuires.dll",&status);
	fver _sonicmceburnengine_dll = getFileVer(CreateDisc+L"\\sonicmceburnengine.dll",&status);
	fver _sonicmmburnengine_exe  = getFileVer(CreateDisc+L"\\sonicmmburnengine.exe",&status);
	fver _vxblock_dll            = getFileVer(CreateDisc+L"\\vxblock.dll",&status);

	// Windows Genuine Advantge (WGA) Files
	fver _LegitCheckControl_dll = getFileVer(System32+L"\\LegitCheckControl.dll",&status);
	fver _WgaLogon_dll          = getFileVer(System32+L"\\WgaLogon.dll",&status);
	fver _WgaTray_exe           = getFileVer(System32+L"\\WgaTray.exe",&status);

	// Windows Search WDS files
	fver _deskbar_dll  = getFileVer(WDS+L"\\deskbar.dll",&status);
	fver _dbsetup_dll  = getFileVer(WDS+L"\\dbsetup.dll",&status);
	fver _dbres_dll  = getFileVer(WDS+L"\\dbres.dll",&status);
	fver _wordwheel_dll  = getFileVer(WDS+L"\\wordwheel.dll",&status);
	fver _wwres_dll  = getFileVer(WDS+L"\\wwres.dll",&status);
	fver _msnlext_dll  = getFileVer(WDS+L"\\msnlext.dll",&status);
	fver _msnlextres_dll  = getFileVer(WDS+L"\\msnlextres.dll",&status);
	fver _msnldl_dll  = getFileVer(WDS+L"\\msnldl.dll",&status);
	fver _msnldlps_dll  = getFileVer(WDS+L"\\msnldlps.dll",&status);
	fver _msnlnamespacemgr_dll  = getFileVer(WDS+L"\\msnlnamespacemgr.dll",&status);
	fver _wdsshell_dll  = getFileVer(WDS+L"\\wdsshell.dll",&status);
	fver _wdsview_dll  = getFileVer(WDS+L"\\wdsview.dll",&status);
	fver _mapine_dll  = getFileVer(WDS+L"\\mapine.dll",&status);
	fver _mapineres_dll  = getFileVer(WDS+L"\\mapineres.dll",&status);
	fver _xppreviewproxy_dll  = getFileVer(WDS+L"\\xppreviewproxy.dll",&status);
	fver _msnlred_dll  = getFileVer(WDS+L"\\msnlred.dll",&status);
	fver _wdsmkttools_dll  = getFileVer(WDS+L"\\wdsmkttools.dll",&status);
	fver _windowssearchres_dll  = getFileVer(WDS+L"\\windowssearchres.dll",&status);
	fver _windowssearch_exe  = getFileVer(WDS+L"\\windowssearch.exe",&status);

	fver _msnlextres_dll_mui  = getFileVer(WDS_mui+L"\\msnlextres.dll.mui",&status);
	fver _dbres_dll_mui  = getFileVer(WDS_mui+L"\\dbres.dll.mui",&status);
	fver _wwres_dll_mui  = getFileVer(WDS_mui+L"\\wwres.dll.mui",&status);
	fver _mapineres_dll_mui  = getFileVer(WDS_mui+L"\\mapineres.dll.mui",&status);
	fver _windowssearchres_dll_mui  = getFileVer(WDS_mui+L"\\windowssearchres.dll.mui",&status);
	
	fver _oephres_dll_mui  = getFileVer(System32+L"\\en-US\\oephres.dll.mui",&status);
	fver _uncres_dll_mui  = getFileVer(System32+L"\\en-US\\uncres.dll.mui",&status);

	fver _comadmin_dll = getFileVer(System32+L"\\Com\\comadmin.dll",&status);
	fver _migregdb_exe = getFileVer(System32+L"\\Com\\migregdb.exe",&status);

	fver _aec_sys      = getFileVer(Drivers+L"\\aec.sys",&status);
	fver _afd_sys      = getFileVer(Drivers+L"\\afd.sys",&status);
	fver _bthport_sys  = getFileVer(Drivers+L"\\bthport.sys",&status);
	fver _cdrom_sys    = getFileVer(Drivers+L"\\cdrom.sys",&status);
	fver _hidir_sys    = getFileVer(Drivers+L"\\hidir.sys",&status);
	fver _hidparse_sys = getFileVer(Drivers+L"\\hidparse.sys",&status);
	fver _exfat_sys    = getFileVer(Drivers+L"\\exfat.sys",&status);
	fver _fastfat_sys  = getFileVer(Drivers+L"\\fastfat.sys",&status);
	fver _fltMgr_sys   = getFileVer(Drivers+L"\\fltMgr.sys",&status);
	fver _fs_rec_sys   = getFileVer(Drivers+L"\\fs_rec.sys",&status);
	fver _http_sys     = getFileVer(Drivers+L"\\http.sys",&status);
	fver _ipnat_sys    = getFileVer(Drivers+L"\\ipnat.sys",&status);
	fver _irbus_sys    = getFileVer(Drivers+L"\\irbus.sys",&status);
	fver _kmixer_sys   = getFileVer(Drivers+L"\\kmixer.sys",&status);
	fver _ks_sys       = getFileVer(Drivers+L"\\ks.sys",&status);
	fver _ksecdd_sys   = getFileVer(Drivers+L"\\ksecdd.sys",&status);
	fver _mrxdav_sys   = getFileVer(Drivers+L"\\mrxdav.sys",&status);
	fver _mrxsmb_sys   = getFileVer(Drivers+L"\\mrxsmb.sys",&status);
	fver _mqac_sys     = getFileVer(Drivers+L"\\mqac.sys",&status);
	fver _msfs_sys     = getFileVer(Drivers+L"\\msfs.sys",&status);
	fver _mup_sys      = getFileVer(Drivers+L"\\mup.sys",&status);
	fver _ndistapi_sys = getFileVer(Drivers+L"\\ndistapi.sys",&status);
	fver _ndisuio_sys  = getFileVer(Drivers+L"\\ndisuio.sys",&status);
	fver _ndiswan_sys  = getFileVer(Drivers+L"\\ndiswan.sys",&status);
	fver _ndproxy_sys  = getFileVer(Drivers+L"\\ndproxy.sys",&status);
	fver _netbt_sys    = getFileVer(Drivers+L"\\netbt.sys",&status);
	fver _npfs_sys     = getFileVer(Drivers+L"\\npfs.sys",&status);
	fver _ntfs_sys     = getFileVer(Drivers+L"\\ntfs.sys",&status);
	fver _nwrdr_sys    = getFileVer(Drivers+L"\\nwrdr.sys",&status);
	fver _powerfil_sys = getFileVer(Drivers+L"\\powerfil.sys",&status);
	fver _pxhelp20_sys = getFileVer(Drivers+L"\\pxhelp20.sys",&status);
	fver _rmcast_sys   = getFileVer(Drivers+L"\\rmcast.sys",&status);
	fver _rdbss_sys    = getFileVer(Drivers+L"\\rdbss.sys",&status);
	fver _rdpwd_sys    = getFileVer(Drivers+L"\\rdpwd.sys",&status);
	fver _sbp2port_sys = getFileVer(Drivers+L"\\sbp2port.sys",&status);
	fver _secdrv_sys   = getFileVer(Drivers+L"\\secdrv.sys",&status);
	fver _splitter_sys = getFileVer(Drivers+L"\\splitter.sys",&status);
	fver _srv_sys      = getFileVer(Drivers+L"\\srv.sys",&status);
	fver _tcpip6_sys   = getFileVer(Drivers+L"\\tcpip6.sys",&status);
	fver _tcpip_sys    = getFileVer(Drivers+L"\\tcpip.sys",&status);
	fver _termdd_sys   = getFileVer(Drivers+L"\\termdd.sys",&status);
	fver _update_sys   = getFileVer(Drivers+L"\\update.sys",&status);
	fver _usb8023_sys  = getFileVer(Drivers+L"\\usb8023.sys",&status);
	fver _usb8023x_sys = getFileVer(Drivers+L"\\usb8023x.sys",&status);
	fver _usbaudio_sys = getFileVer(Drivers+L"\\usbaudio.sys",&status);
	fver _usbccgp_sys  = getFileVer(Drivers+L"\\usbccgp.sys",&status);
	fver _usbd_sys     = getFileVer(Drivers+L"\\usbd.sys",&status);
	fver _usbehci_sys  = getFileVer(Drivers+L"\\usbehci.sys",&status);
	fver _usbport_sys  = getFileVer(Drivers+L"\\usbport.sys",&status);
	fver _usbstor_sys  = getFileVer(Drivers+L"\\usbstor.sys",&status);
	fver _usbscan_sys  = getFileVer(Drivers+L"\\usbscan.sys",&status);
	fver _usbvideo_sys = getFileVer(Drivers+L"\\usbvideo.sys",&status);
	fver _wdmaud_sys   = getFileVer(Drivers+L"\\wdmaud.sys",&status);
	fver _wpdusb_sys   = getFileVer(Drivers+L"\\wpdusb.sys",&status);

	fver _hidir_sys_cache    = getFileVer(DriverCache+L"\\hidir.sys",&status);
	fver _irbus_sys_cache    = getFileVer(DriverCache+L"\\irbus.sys",&status);
	fver _powerfil_sys_cache = getFileVer(DriverCache+L"\\powerfil.sys",&status);
	fver _sbp2port_sys_cache = getFileVer(DriverCache+L"\\sbp2port.sys",&status);

	fver _cdm_dll      = getFileVer(System32+L"\\cdm.dll",&status);
	fver _wuapi_dll    = getFileVer(System32+L"\\wuapi.dll",&status);
	fver _wuauclt_exe  = getFileVer(System32+L"\\wuauclt.exe",&status);
	fver _wuaucpl_cpl  = getFileVer(System32+L"\\wuaucpl.cpl",&status);
	fver _wuaueng_dll  = getFileVer(System32+L"\\wuaueng.dll",&status);
	fver _wuauserv_dll = getFileVer(System32+L"\\wuauserv.dll",&status);
	fver _wucltui_dll  = getFileVer(System32+L"\\wucltui.dll",&status);
	fver _wups2_dll    = getFileVer(System32+L"\\wups2.dll",&status);
	fver _wups_dll     = getFileVer(System32+L"\\wups.dll",&status);
	fver _wuweb_dll    = getFileVer(System32+L"\\wuweb.dll",&status);

	fver _asp_dll      = getFileVer(inetsrv+L"\\asp.dll",&status);
	fver _asp51_dll    = getFileVer(inetsrv+L"\\asp51.dll",&status);
	fver _asp51_dll_dllcache    = getFileVer(dllcache+L"\\asp51.dll",&status);
	fver _ftpsvc2_dll  = getFileVer(inetsrv+L"\\ftpsvc2.dll",&status);
	fver _httpext_dll  = getFileVer(inetsrv+L"\\httpext.dll",&status);
	fver _infocomm_dll = getFileVer(inetsrv+L"\\infocomm.dll",&status);
	fver _smtpsvc_dll  = getFileVer(inetsrv+L"\\smtpsvc.dll",&status);
	fver _w3svc_dll    = getFileVer(inetsrv+L"\\w3svc.dll",&status);

	fver _iecompat_dll = getFileVer(InternetExplorer+L"\\iecompat.dll",&status);
	fver _iedvtool_dll = getFileVer(InternetExplorer+L"\\iedvtool.dll",&status);
	fver _iedw_exe     = getFileVer(InternetExplorer+L"\\iedw.exe",&status);
	fver _ieproxy_dll  = getFileVer(InternetExplorer+L"\\ieproxy.dll",&status);
	fver _iexplore_exe = getFileVer(InternetExplorer+L"\\iexplore.exe",&status);
	fver _jsdbgui_dll  = getFileVer(InternetExplorer+L"\\jsdbgui.dll",&status);
	fver _jsprofilerui_dll = getFileVer(InternetExplorer+L"\\jsprofilerui.dll",&status);
	fver _xpshims_dll  = getFileVer(InternetExplorer+L"\\xpshims.dll",&status);

	fver _npdsplay_dll = getFileVer(WindowsMediaPlayer+L"\\npdsplay.dll",&status);
	fver _setup_wm_exe = getFileVer(WindowsMediaPlayer+L"\\setup_wm.exe",&status);
	fver _wmplayer_exe = getFileVer(WindowsMediaPlayer+L"\\wmplayer.exe",&status);
	fver _wmsetsdk_exe = getFileVer(WindowsMediaPlayer+L"\\wmsetsdk.exe",&status);

	fver _vgx_dll      = getFileVer(VGX+L"\\vgx.dll",&status);

	fver _custsat_dll   = getFileVer(NetworkDiagnostic+L"\\custsat.dll",&status);
	fver _xpnetdiag_exe = getFileVer(NetworkDiagnostic+L"\\xpnetdiag.exe",&status);

	fver _migload_exe  = getFileVer(usmt+L"\\migload.exe",&status);
	fver _migwiz_exe   = getFileVer(usmt+L"\\migwiz.exe",&status);
	fver _migwiza_exe  = getFileVer(usmt+L"\\migwiza.exe",&status);
	fver _cobramsg_dll = getFileVer(usmt+L"\\cobramsg.dll",&status);
	fver _guitrn_dll   = getFileVer(usmt+L"\\guitrn.dll",&status);
	fver _guitrna_dll  = getFileVer(usmt+L"\\guitrna.dll",&status);
	fver _iconlib_dll  = getFileVer(usmt+L"\\iconlib.dll",&status);
	fver _log_dll      = getFileVer(usmt+L"\\log.dll",&status);
	fver _migism_dll   = getFileVer(usmt+L"\\migism.dll",&status);
	fver _migisma_dll  = getFileVer(usmt+L"\\migisma.dll",&status);
	fver _script_dll   = getFileVer(usmt+L"\\script.dll",&status);
	fver _scripta_dll  = getFileVer(usmt+L"\\scripta.dll",&status);
	fver _sysmod_dll   = getFileVer(usmt+L"\\sysmod.dll",&status);
	fver _sysmoda_dll  = getFileVer(usmt+L"\\sysmoda.dll",&status);

	fver _msmsgs_exe   = getFileVer(Messenger+L"\\msmsgs.exe",&status);
	fver _msgsc_dll    = getFileVer(Messenger+L"\\msgsc.dll",&status);

	fver _msoe_dll    = getFileVer(OutlookExpress+L"\\msoe.dll",&status);
	fver _wab_exe     = getFileVer(OutlookExpress+L"\\wab.exe",&status);
	fver _wabfind_dll = getFileVer(OutlookExpress+L"\\wabfind.dll",&status);
	fver _wabimp_dll  = getFileVer(OutlookExpress+L"\\wabimp.dll",&status);
	fver _wabmig_exe  = getFileVer(OutlookExpress+L"\\wabmig.exe",&status);

	fver _dhtmled_ocx  = getFileVer(Triedit+L"\\dhtmled.ocx",&status);
	fver _triedit_dll  = getFileVer(Triedit+L"\\triedit.dll",&status);
	fver _wordpad_exe  = getFileVer(Accessories+L"\\wordpad.exe",&status);
	fver _mswrd6_wpc = getFileVer(Accessories+L"\\mswrd6.wpc",&status);
	fver _mswrd8_wpc = getFileVer(Accessories+L"\\mswrd8.wpc",&status);
	fver _write_wpc = getFileVer(Accessories+L"\\write.wpc",&status);
	fver _msconv97_dll = getFileVer(TextConv+L"\\msconv97.dll",&status);
	fver _html32_cnv   = getFileVer(TextConv+L"\\html32.cnv",&status);
	fver _mswrd632_wpc   = getFileVer(TextConv+L"\\mswrd632.wpc",&status);
	fver _mswrd832_cnv   = getFileVer(TextConv+L"\\mswrd832.cnv",&status);
	fver _write32_wpc   = getFileVer(TextConv+L"\\write32.wpc",&status);
	fver _flash_ocx = getFileVer(Flash+L"\\flash.ocx",&status);
	fver _flash6_ocx = getFileVer(Flash+L"\\Flash6.ocx",&status);
	fver _moviemk_exe = getFileVer(MovieMaker+L"\\moviemk.exe",&status);

	fver _helpctr_exe  = getFileVer(helpctr_binaries+L"\\helpctr.exe",&status);
	fver _helphost_exe = getFileVer(helpctr_binaries+L"\\helphost.exe",&status);
	fver _helpsvc_exe  = getFileVer(helpctr_binaries+L"\\helpsvc.exe",&status);
	fver _pchsvc_dll   = getFileVer(helpctr_binaries+L"\\pchsvc.dll",&status);
	fver _srdiag_exe   = getFileVer(Restore+L"\\srdiag.exe",&status);
	fver _uploadm_exe  = getFileVer(UploadLB+L"\\uploadm.exe",&status);

	fver _directdb_dll = getFileVer(CommonFilesSystem+L"\\directdb.dll",&status);
	fver _wab32_dll    = getFileVer(CommonFilesSystem+L"\\wab32.dll",&status);

	fver _msadce_dll = getFileVer(msadc+L"\\msadce.dll",&status);
	fver _msadco_dll = getFileVer(msadc+L"\\msadco.dll",&status);
	fver _msado15_dll = getFileVer(ado+L"\\msado15.dll",&status);
	fver _msadomd_dll = getFileVer(ado+L"\\msadomd.dll",&status);
	fver _msador15_dll = getFileVer(ado+L"\\msador15.dll",&status);
	fver _msadox_dll = getFileVer(ado+L"\\msadox.dll",&status);
	fver _msjro_dll = getFileVer(ado+L"\\msjro.dll",&status);
	fver _msado20_tlb = getFileVer(ado+L"\\msado20.tlb",&status);
	fver _msado21_tlb = getFileVer(ado+L"\\msado21.tlb",&status);
	fver _msado25_tlb = getFileVer(ado+L"\\msado25.tlb",&status);
	fver _msado26_tlb = getFileVer(ado+L"\\msado26.tlb",&status);
	fver _msado27_tlb = getFileVer(ado+L"\\msado27.tlb",&status);
	fver _msado28_tlb = getFileVer(ado+L"\\msado28.tlb",&status);
	fver _msdaora_dll = getFileVer(OleDB+L"\\msdaora.dll",&status);

	fver _fastprox_dll = getFileVer(wbem+L"\\fastprox.dll",&status);
	fver _wmiprvsd_dll = getFileVer(wbem+L"\\wmiprvsd.dll",&status);
	fver _wmiprvse_exe = getFileVer(wbem+L"\\wmiprvse.exe",&status);

	fver _agcore_dll = getFileVer(MicrosoftSilverlight+L"\\agcore.dll",&status);
	fver _slup_exe = getFileVer(MicrosoftSilverlight+L"\\slup.exe",&status);
	fver _sllauncher_exe = getFileVer(MicrosoftSilverlight+L"\\sllauncher.exe",&status);

	fver _agcore_debug_dll_2_0_31005 = getFileVer(MicrosoftSilverlight+L"\\2.0.31005.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_2_0_40115 = getFileVer(MicrosoftSilverlight+L"\\2.0.40115.0\\agcore.debug.dll",&status);

	fver _agcore_dll_2_0_31005 = getFileVer(MicrosoftSilverlight+L"\\2.0.31005.0\\agcore.dll",&status);
	fver _agcore_dll_2_0_40115 = getFileVer(MicrosoftSilverlight+L"\\2.0.40115.0\\agcore.dll",&status);
	fver _agcore_dll_3_0_40307 = getFileVer(MicrosoftSilverlight+L"\\3.0.40307.0\\agcore.dll",&status);

	fver _agcore_debug_dll_3_0_40307 = getFileVer(MicrosoftSilverlight+L"\\3.0.40307.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_3_0_40624 = getFileVer(MicrosoftSilverlight+L"\\3.0.40624.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_3_0_40723 = getFileVer(MicrosoftSilverlight+L"\\3.0.40723.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_3_0_40818 = getFileVer(MicrosoftSilverlight+L"\\3.0.40818.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_3_0_50106 = getFileVer(MicrosoftSilverlight+L"\\3.0.50106.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_3_0_50611 = getFileVer(MicrosoftSilverlight+L"\\3.0.50611.0\\agcore.debug.dll",&status);

	fver _agcore_debug_dll_4_0_50401 = getFileVer(MicrosoftSilverlight+L"\\4.0.50401.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_4_0_50524 = getFileVer(MicrosoftSilverlight+L"\\4.0.50524.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_4_0_50826 = getFileVer(MicrosoftSilverlight+L"\\4.0.50826.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_4_0_50917 = getFileVer(MicrosoftSilverlight+L"\\4.0.50917.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_4_0_51204 = getFileVer(MicrosoftSilverlight+L"\\4.0.51204.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_4_0_60129 = getFileVer(MicrosoftSilverlight+L"\\4.0.60129.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_4_0_60310 = getFileVer(MicrosoftSilverlight+L"\\4.0.60310.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_4_0_60531 = getFileVer(MicrosoftSilverlight+L"\\4.0.60531.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_4_0_60831 = getFileVer(MicrosoftSilverlight+L"\\4.0.60831.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_4_1_10111 = getFileVer(MicrosoftSilverlight+L"\\4.1.10111.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_4_1_10329 = getFileVer(MicrosoftSilverlight+L"\\4.1.10329.0\\agcore.debug.dll",&status);

	fver _agcore_debug_dll_5_0_60401 = getFileVer(MicrosoftSilverlight+L"\\5.0.60401.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_5_0_60818 = getFileVer(MicrosoftSilverlight+L"\\5.0.60818.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_5_0_61118 = getFileVer(MicrosoftSilverlight+L"\\5.0.61118.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_5_1_10411 = getFileVer(MicrosoftSilverlight+L"\\5.1.10411.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_5_1_20125 = getFileVer(MicrosoftSilverlight+L"\\5.1.20125.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_5_1_20513 = getFileVer(MicrosoftSilverlight+L"\\5.1.20513.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_5_1_20913 = getFileVer(MicrosoftSilverlight+L"\\5.1.20913.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_5_1_30214 = getFileVer(MicrosoftSilverlight+L"\\5.1.30214.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_5_1_30514 = getFileVer(MicrosoftSilverlight+L"\\5.1.30514.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_5_1_40416 = getFileVer(MicrosoftSilverlight+L"\\5.1.40416.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_5_1_40728 = getFileVer(MicrosoftSilverlight+L"\\5.1.40728.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_5_1_41105 = getFileVer(MicrosoftSilverlight+L"\\5.1.41105.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_5_1_41212 = getFileVer(MicrosoftSilverlight+L"\\5.1.41212.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_5_1_50428 = getFileVer(MicrosoftSilverlight+L"\\5.1.50428.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_5_1_50709 = getFileVer(MicrosoftSilverlight+L"\\5.1.50709.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_5_1_50901 = getFileVer(MicrosoftSilverlight+L"\\5.1.50901.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_5_1_50905 = getFileVer(MicrosoftSilverlight+L"\\5.1.50905.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_5_1_50906 = getFileVer(MicrosoftSilverlight+L"\\5.1.50906.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_5_1_50907 = getFileVer(MicrosoftSilverlight+L"\\5.1.50907.0\\agcore.debug.dll",&status);
	fver _agcore_debug_dll_5_1_50918 = getFileVer(MicrosoftSilverlight+L"\\5.1.50918.0\\agcore.debug.dll",&status);

	if( _agcore_dll_2_0_31005 > _agcore_dll ) { _agcore_dll = _agcore_dll_2_0_31005; }
	if( _agcore_dll_2_0_40115 > _agcore_dll ) { _agcore_dll = _agcore_dll_2_0_40115; }
	if( _agcore_dll_3_0_40307 > _agcore_dll ) { _agcore_dll = _agcore_dll_3_0_40307; }

	fver _agcore_debug_dll = zero;

	if( _agcore_debug_dll_2_0_31005 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_2_0_31005; }
	if( _agcore_debug_dll_2_0_40115 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_2_0_40115; }

	if( _agcore_debug_dll_3_0_40624 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_3_0_40624; }
	if( _agcore_debug_dll_3_0_40723 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_3_0_40723; }
	if( _agcore_debug_dll_3_0_40818 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_3_0_40818; }
	if( _agcore_debug_dll_3_0_50106 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_3_0_50106; }
	if( _agcore_debug_dll_3_0_50611 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_3_0_50611; }

	if( _agcore_debug_dll_4_0_50401 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_4_0_50401; }
	if( _agcore_debug_dll_4_0_50524 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_4_0_50524; }
	if( _agcore_debug_dll_4_0_50826 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_4_0_50826; }
	if( _agcore_debug_dll_4_0_50917 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_4_0_50917; }
	if( _agcore_debug_dll_4_0_51204 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_4_0_51204; }
	if( _agcore_debug_dll_4_0_60129 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_4_0_60129; }
	if( _agcore_debug_dll_4_0_60310 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_4_0_60310; }
	if( _agcore_debug_dll_4_0_60531 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_4_0_60531; }
	if( _agcore_debug_dll_4_0_60831 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_4_0_60831; }
	if( _agcore_debug_dll_4_1_10111 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_4_1_10111; }
	if( _agcore_debug_dll_4_1_10329 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_4_1_10329; }

	if( _agcore_debug_dll_5_0_60401 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_5_0_60401; }
	if( _agcore_debug_dll_5_0_60818 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_5_0_60818; }
	if( _agcore_debug_dll_5_0_61118 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_5_0_61118; }
	if( _agcore_debug_dll_5_1_10411 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_5_1_10411; }
	if( _agcore_debug_dll_5_1_20125 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_5_1_20125; }
	if( _agcore_debug_dll_5_1_20513 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_5_1_20513; }
	if( _agcore_debug_dll_5_1_20913 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_5_1_20913; }
	if( _agcore_debug_dll_5_1_30214 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_5_1_30214; }
	if( _agcore_debug_dll_5_1_30514 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_5_1_30514; }
	if( _agcore_debug_dll_5_1_40416 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_5_1_40416; }
	if( _agcore_debug_dll_5_1_40728 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_5_1_40728; }
	if( _agcore_debug_dll_5_1_41105 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_5_1_41105; }
	if( _agcore_debug_dll_5_1_41212 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_5_1_41212; }
	if( _agcore_debug_dll_5_1_50428 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_5_1_50428; }
	if( _agcore_debug_dll_5_1_50709 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_5_1_50709; }
	if( _agcore_debug_dll_5_1_50901 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_5_1_50901; }
	if( _agcore_debug_dll_5_1_50905 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_5_1_50905; }
	if( _agcore_debug_dll_5_1_50906 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_5_1_50906; }
	if( _agcore_debug_dll_5_1_50907 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_5_1_50907; }
	if( _agcore_debug_dll_5_1_50918 > _agcore_debug_dll ) { _agcore_debug_dll = _agcore_debug_dll_5_1_50918; }
 
	fver _jntview_exe            = getFileVer(jnl+L"\\jntview.exe",&status);

	fver _spcustom_dll_ref = getFileVer(wxp_x86_0409_v1+L"\\spcustom.dll.ref",&status);
	fver _spmsg_dll_ref    = getFileVer(wxp_x86_0409_v1+L"\\spmsg.dll.ref",&status);
	fver _spuninst_exe_ref = getFileVer(wxp_x86_0409_v1+L"\\spuninst.exe.ref",&status);
	fver _spupdsvc_exe_ref = getFileVer(wxp_x86_0409_v1+L"\\spupdsvc.exe.ref",&status);
	fver _update_exe_ref   = getFileVer(wxp_x86_0409_v1+L"\\update.exe.ref",&status);
	fver _updspapi_dll_ref = getFileVer(wxp_x86_0409_v1+L"\\updspapi.dll.ref",&status);

	fver _BitLockerToGo_exe = getFileVer(SystemRoot+L"\\BitLockerDiscoveryVolumeContents\\BitLockerToGo.exe",&status);

	fver _agentdp2_dll = getFileVer(msagent+L"\\agentdp2.dll",&status);
	fver _agentdpv_dll = getFileVer(msagent+L"\\agentdpv.dll",&status);
	fver _agentsvr_exe = getFileVer(msagent+L"\\agentsvr.exe",&status);

	fver _gdiplus_dll_22319 = getFileVer(WinSxS+L"\\x86_Microsoft.Windows.GdiPlus_6595b64144ccf1df_1.0.6001.22319_x-ww_f0b4c2df\\GdiPlus.dll",&status);
	fver _gdiplus_dll_22791 = getFileVer(WinSxS+L"\\x86_Microsoft.Windows.GdiPlus_6595b64144ccf1df_1.0.6002.22791_x-ww_c8dff154\\GdiPlus.dll",&status);
	fver _gdiplus_dll_23084 = getFileVer(WinSxS+L"\\x86_Microsoft.Windows.GdiPlus_6595b64144ccf1df_1.0.6002.23084_x-ww_f3f35550\\GdiPlus.dll",&status);
	fver _gdiplus_dll_23386 = getFileVer(WinSxS+L"\\x86_Microsoft.Windows.GdiPlus_6595b64144ccf1df_1.0.6002.23386_x-ww_f56d83d5\\GdiPlus.dll",&status);
	fver _gdiplus_dll_23535 = getFileVer(WinSxS+L"\\x86_Microsoft.Windows.GdiPlus_6595b64144ccf1df_1.0.6002.23535_x-ww_f664a19b\\GdiPlus.dll",&status);
	fver _gdiplus_dll_23588 = getFileVer(WinSxS+L"\\x86_Microsoft.Windows.GdiPlus_6595b64144ccf1df_1.0.6002.23588_x-ww_f669a2d9\\GdiPlus.dll",&status);
	fver _gdiplus_dll_23609 = getFileVer(WinSxS+L"\\x86_Microsoft.Windows.GdiPlus_6595b64144ccf1df_1.0.6002.23609_x-ww_f6dfb063\\GdiPlus.dll",&status);
	fver _gdiplus_dll_23693 = getFileVer(WinSxS+L"\\x86_Microsoft.Windows.GdiPlus_6595b64144ccf1df_1.0.6002.23693_x-ww_f6e8b294\\GdiPlus.dll",&status);
	fver _gdiplus_dll_23746 = getFileVer(WinSxS+L"\\x86_Microsoft.Windows.GdiPlus_6595b64144ccf1df_1.0.6002.23746_x-ww_f761c0dd\\GdiPlus.dll",&status);
	fver _gdiplus_dll_23930 = getFileVer(WinSxS+L"\\x86_Microsoft.Windows.GdiPlus_6595b64144ccf1df_1.0.6002.23930_x-ww_f85cdf9a\\GdiPlus.dll",&status);
	fver _gdiplus_dll_23984 = getFileVer(WinSxS+L"\\x86_Microsoft.Windows.GdiPlus_6595b64144ccf1df_1.0.6002.23984_x-ww_f861e0d9\\GdiPlus.dll",&status);
	fver _gdiplus_dll_24064 = getFileVer(WinSxS+L"\\x86_Microsoft.Windows.GdiPlus_6595b64144ccf1df_1.0.6002.24064_x-ww_22782591\\GdiPlus.dll",&status);
	fver _gdiplus_dll_24072 = getFileVer(WinSxS+L"\\x86_Microsoft.Windows.GdiPlus_6595b64144ccf1df_1.0.6002.24072_x-ww_227925ce\\GdiPlus.dll",&status);
	fver _gdiplus_dll_24180 = getFileVer(WinSxS+L"\\x86_Microsoft.Windows.GdiPlus_6595b64144ccf1df_1.0.6002.24180_x-ww_22f8358c\\GdiPlus.dll",&status);
	fver _gdiplus_dll_24439 = getFileVer(WinSxS+L"\\x86_Microsoft.Windows.GdiPlus_6595b64144ccf1df_1.0.6002.24439_x-ww_246d62dd\\GdiPlus.dll",&status);
	fver _gdiplus_dll_24465 = getFileVer(WinSxS+L"\\x86_Microsoft.Windows.GdiPlus_6595b64144ccf1df_1.0.6002.24465_x-ww_24706396\\GdiPlus.dll",&status);
	fver _gdiplus_dll_24489 = getFileVer(WinSxS+L"\\x86_Microsoft.Windows.GdiPlus_6595b64144ccf1df_1.0.6002.24489_x-ww_24726418\\GdiPlus.dll",&status);
	fver _gdiplus_dll_24533 = getFileVer(WinSxS+L"\\x86_Microsoft.Windows.GdiPlus_6595b64144ccf1df_1.0.6002.24533_x-ww_24eb7258\\GdiPlus.dll",&status);
	fver _gdiplus_dll_24561 = getFileVer(WinSxS+L"\\x86_Microsoft.Windows.GdiPlus_6595b64144ccf1df_1.0.6002.24561_x-ww_24ee7313\\GdiPlus.dll",&status);
	fver _gdiplus_dll_24563 = getFileVer(WinSxS+L"\\x86_Microsoft.Windows.GdiPlus_6595b64144ccf1df_1.0.6002.24563_x-ww_24ee7315\\GdiPlus.dll",&status);
	fver _gdiplus_dll_20488 = getFileVer(WinSxS+L"\\x86_Microsoft.Windows.GdiPlus_6595b64144ccf1df_1.0.6003.20488_x-ww_40820f9c\\GdiPlus.dll",&status);

	fver _comctl32_dll_2982 = getFileVer(WinSxS+L"\\x86_Microsoft.Windows.Common-Controls_6595b64144ccf1df_6.0.2600.2982_x-ww_ac3f9c03\\comctl32.dll",&status);
	fver _comctl32_dll_7660 = getFileVer(WinSxS+L"\\x86_Microsoft.Windows.Common-Controls_6595b64144ccf1df_6.0.2600.7660_x-ww_936580bb\\comctl32.dll",&status);

	fver _atl_dll_9792     = getFileVer(WinSxS+L"\\x86_Microsoft.Tools.VisualCPlusPlus.Runtime-Libraries_6595b64144ccf1df_6.0.9792.0_x-ww_08a6620a\\atl.dll",&status);
	fver _mfc42_dll_9792   = getFileVer(WinSxS+L"\\x86_Microsoft.Tools.VisualCPlusPlus.Runtime-Libraries_6595b64144ccf1df_6.0.9792.0_x-ww_08a6620a\\mfc42.dll",&status);
	fver _mfc42u_dll_9792  = getFileVer(WinSxS+L"\\x86_Microsoft.Tools.VisualCPlusPlus.Runtime-Libraries_6595b64144ccf1df_6.0.9792.0_x-ww_08a6620a\\mfc42u.dll",&status);
	fver _msvcp60_dll_9792 = getFileVer(WinSxS+L"\\x86_Microsoft.Tools.VisualCPlusPlus.Runtime-Libraries_6595b64144ccf1df_6.0.9792.0_x-ww_08a6620a\\msvcp60.dll",&status);

	std::wstring    SharedVC = ProgramFiles+L"\\Common Files\\Microsoft Shared\\VC";
	fver _msdia80_dll = getFileVer(SharedVC+L"\\msdia80.dll",&status);

	std::wstring      ATL8_42 = WinSxS+L"\\x86_Microsoft.VC80.ATL_1fc8b3b9a1e18e3b_8.0.50727.42_x-ww_6e805841";
	std::wstring      CRT8_42 = WinSxS+L"\\x86_Microsoft.VC80.CRT_1fc8b3b9a1e18e3b_8.0.50727.42_x-ww_0de06acd";
	std::wstring      MFC8_42 = WinSxS+L"\\x86_Microsoft.VC80.MFC_1fc8b3b9a1e18e3b_8.0.50727.42_x-ww_dec6ddd2";
	std::wstring   MFCLOC8_42 = WinSxS+L"\\x86_Microsoft.VC80.MFCLOC_1fc8b3b9a1e18e3b_8.0.50727.42_x-ww_3415f6d0";
	std::wstring   OpenMP8_42 = WinSxS+L"\\x86_Microsoft.VC80.OpenMP_1fc8b3b9a1e18e3b_8.0.50727.42_x-ww_0ee63867";
	fver _ATL80_dll_42 = getFileVer(ATL8_42+L"\\ATL80.dll",&status);
	fver _mfc80_dll_42 = getFileVer(MFC8_42+L"\\mfc80.dll",&status);
	fver _mfc80u_dll_42 = getFileVer(MFC8_42+L"\\mfc80u.dll",&status);
	fver _mfcm80_dll_42   = getFileVer(MFC8_42+L"\\mfcm80.dll",&status);
	fver _mfcm80u_dll_42  = getFileVer(MFC8_42+L"\\mfcm80u.dll",&status);
	fver _mfc80CHS_dll_42 = getFileVer(MFCLOC8_42+L"\\mfc80CHS.dll",&status);
	fver _mfc80CHT_dll_42 = getFileVer(MFCLOC8_42+L"\\mfc80CHT.dll",&status);
	fver _mfc80DEU_dll_42 = getFileVer(MFCLOC8_42+L"\\mfc80DEU.dll",&status);
	fver _mfc80ENU_dll_42 = getFileVer(MFCLOC8_42+L"\\mfc80ENU.dll",&status);
	fver _mfc80ESP_dll_42 = getFileVer(MFCLOC8_42+L"\\mfc80ESP.dll",&status);
	fver _mfc80FRA_dll_42 = getFileVer(MFCLOC8_42+L"\\mfc80FRA.dll",&status);
	fver _mfc80ITA_dll_42 = getFileVer(MFCLOC8_42+L"\\mfc80ITA.dll",&status);
	fver _mfc80JPN_dll_42 = getFileVer(MFCLOC8_42+L"\\mfc80JPN.dll",&status);
	fver _mfc80KOR_dll_42 = getFileVer(MFCLOC8_42+L"\\mfc80KOR.dll",&status);
	fver _msvcm80_dll_42 = getFileVer(CRT8_42+L"\\msvcm80.dll",&status);
	fver _msvcp80_dll_42 = getFileVer(CRT8_42+L"\\msvcp80.dll",&status);
	fver _msvcr80_dll_42 = getFileVer(CRT8_42+L"\\msvcr80.dll",&status);
	fver _vcomp_dll_42 = getFileVer(OpenMP8_42+L"\\vcomp.dll",&status);
	fver _ATL80_dll = _ATL80_dll_42;
	fver _mfc80_dll = _mfc80_dll_42;
	fver _mfc80u_dll = _mfc80u_dll_42;
	fver _mfcm80_dll = _mfcm80_dll_42;
	fver _mfcm80u_dll = _mfcm80u_dll_42;
	fver _mfc80CHS_dll = _mfc80CHS_dll_42;
	fver _mfc80CHT_dll = _mfc80CHT_dll_42;
	fver _mfc80DEU_dll = _mfc80DEU_dll_42;
	fver _mfc80ENU_dll = _mfc80ENU_dll_42;
	fver _mfc80ESP_dll = _mfc80ESP_dll_42;
	fver _mfc80FRA_dll = _mfc80FRA_dll_42;
	fver _mfc80ITA_dll = _mfc80ITA_dll_42;
	fver _mfc80JPN_dll = _mfc80JPN_dll_42;
	fver _mfc80KOR_dll = _mfc80KOR_dll_42;
	fver _msvcm80_dll = _msvcm80_dll_42;
	fver _msvcp80_dll = _msvcp80_dll_42;
	fver _msvcr80_dll = _msvcr80_dll_42;
	fver _vcomp_dll = _vcomp_dll_42;


	std::wstring     ATL8_762 = WinSxS+L"\\x86_Microsoft.VC80.ATL_1fc8b3b9a1e18e3b_8.0.50727.762_x-ww_cbb27474";
	std::wstring     CRT8_762 = WinSxS+L"\\x86_Microsoft.VC80.CRT_1fc8b3b9a1e18e3b_8.0.50727.762_x-ww_6b128700";
	std::wstring     MFC8_762 = WinSxS+L"\\x86_Microsoft.VC80.MFC_1fc8b3b9a1e18e3b_8.0.50727.762_x-ww_3bf8fa05";
	std::wstring  MFCLOC8_762 = WinSxS+L"\\x86_Microsoft.VC80.MFCLOC_1fc8b3b9a1e18e3b_8.0.50727.762_x-ww_91481303";
	std::wstring  OpenMP8_762 = WinSxS+L"\\x86_Microsoft.VC80.OpenMP_1fc8b3b9a1e18e3b_8.0.50727.762_x-ww_6c18549a";
	fver _ATL80_dll_762 = getFileVer(ATL8_762+L"\\ATL80.dll",&status);
	fver _mfc80_dll_762 = getFileVer(MFC8_762+L"\\mfc80.dll",&status);
	fver _mfc80u_dll_762 = getFileVer(MFC8_762+L"\\mfc80u.dll",&status);
	fver _mfcm80_dll_762   = getFileVer(MFC8_762+L"\\mfcm80.dll",&status);
	fver _mfcm80u_dll_762  = getFileVer(MFC8_762+L"\\mfcm80u.dll",&status);
	fver _mfc80CHS_dll_762 = getFileVer(MFCLOC8_762+L"\\mfc80CHS.dll",&status);
	fver _mfc80CHT_dll_762 = getFileVer(MFCLOC8_762+L"\\mfc80CHT.dll",&status);
	fver _mfc80DEU_dll_762 = getFileVer(MFCLOC8_762+L"\\mfc80DEU.dll",&status);
	fver _mfc80ENU_dll_762 = getFileVer(MFCLOC8_762+L"\\mfc80ENU.dll",&status);
	fver _mfc80ESP_dll_762 = getFileVer(MFCLOC8_762+L"\\mfc80ESP.dll",&status);
	fver _mfc80FRA_dll_762 = getFileVer(MFCLOC8_762+L"\\mfc80FRA.dll",&status);
	fver _mfc80ITA_dll_762 = getFileVer(MFCLOC8_762+L"\\mfc80ITA.dll",&status);
	fver _mfc80JPN_dll_762 = getFileVer(MFCLOC8_762+L"\\mfc80JPN.dll",&status);
	fver _mfc80KOR_dll_762 = getFileVer(MFCLOC8_762+L"\\mfc80KOR.dll",&status);
	fver _msvcm80_dll_762 = getFileVer(CRT8_762+L"\\msvcm80.dll",&status);
	fver _msvcp80_dll_762 = getFileVer(CRT8_762+L"\\msvcp80.dll",&status);
	fver _msvcr80_dll_762 = getFileVer(CRT8_762+L"\\msvcr80.dll",&status);
	fver _vcomp_dll_762 = getFileVer(OpenMP8_762+L"\\vcomp.dll",&status);
		if (_ATL80_dll_762   >_ATL80_dll   ) { _ATL80_dll    = _ATL80_dll_762;    }
		if (_mfc80_dll_762   >_mfc80_dll   ) { _mfc80_dll    = _mfc80_dll_762;    }
		if (_mfc80u_dll_762  >_mfc80u_dll  ) { _mfc80u_dll   = _mfc80u_dll_762;   }
		if (_mfcm80_dll_762  >_mfcm80_dll  ) { _mfcm80_dll   = _mfcm80_dll_762;   }
		if (_mfcm80u_dll_762 >_mfcm80u_dll ) { _mfcm80u_dll  = _mfcm80u_dll_762;  }
		if (_mfc80CHS_dll_762>_mfc80CHS_dll) { _mfc80CHS_dll = _mfc80CHS_dll_762; }
		if (_mfc80CHT_dll_762>_mfc80CHT_dll) { _mfc80CHT_dll = _mfc80CHT_dll_762; }
		if (_mfc80DEU_dll_762>_mfc80DEU_dll) { _mfc80DEU_dll = _mfc80DEU_dll_762; }
		if (_mfc80ENU_dll_762>_mfc80ENU_dll) { _mfc80ENU_dll = _mfc80ENU_dll_762; }
		if (_mfc80ESP_dll_762>_mfc80ESP_dll) { _mfc80ESP_dll = _mfc80ESP_dll_762; }
		if (_mfc80FRA_dll_762>_mfc80FRA_dll) { _mfc80FRA_dll = _mfc80FRA_dll_762; }
		if (_mfc80ITA_dll_762>_mfc80ITA_dll) { _mfc80ITA_dll = _mfc80ITA_dll_762; }
		if (_mfc80JPN_dll_762>_mfc80JPN_dll) { _mfc80JPN_dll = _mfc80JPN_dll_762; }
		if (_mfc80KOR_dll_762>_mfc80KOR_dll) { _mfc80KOR_dll = _mfc80KOR_dll_762; }
		if (_msvcm80_dll_762 >_msvcm80_dll ) { _msvcm80_dll  = _msvcm80_dll_762;  }
		if (_msvcp80_dll_762 >_msvcp80_dll ) { _msvcp80_dll  = _msvcp80_dll_762;  }
		if (_msvcr80_dll_762 >_msvcr80_dll ) { _msvcr80_dll  = _msvcr80_dll_762;  }
		if (_vcomp_dll_762   >_vcomp_dll   ) { _vcomp_dll    = _vcomp_dll_762;    }


	/* These files are installed with .NET Framework, do not use to search for updates
	std::wstring    CRT8_3053 = WinSxS+L"\\x86_Microsoft.VC80.CRT_1fc8b3b9a1e18e3b_8.0.50727.3053_x-ww_b80fa8ca";
	fver _msvcm80_dll_3053 = getFileVer(CRT8_3053+L"\\msvcm80.dll",&status);
	fver _msvcp80_dll_3053 = getFileVer(CRT8_3053+L"\\msvcp80.dll",&status);
	fver _msvcr80_dll_3053 = getFileVer(CRT8_3053+L"\\msvcr80.dll",&status);
	if (_msvcm80_dll_3053>_msvcm80_dll) { _msvcm80_dll = _msvcm80_dll_3053; }
	if (_msvcp80_dll_3053>_msvcp80_dll) { _msvcp80_dll = _msvcp80_dll_3053; }
	if (_msvcr80_dll_3053>_msvcr80_dll) { _msvcr80_dll = _msvcr80_dll_3053; }
	*/


	std::wstring    ATL8_4053 = WinSxS+L"\\x86_Microsoft.VC80.ATL_1fc8b3b9a1e18e3b_8.0.50727.4053_x-ww_473666fd";
	std::wstring    CRT8_4053 = WinSxS+L"\\x86_Microsoft.VC80.CRT_1fc8b3b9a1e18e3b_8.0.50727.4053_x-ww_e6967989";
	std::wstring    MFC8_4053 = WinSxS+L"\\x86_Microsoft.VC80.MFC_1fc8b3b9a1e18e3b_8.0.50727.4053_x-ww_b77cec8e";
	std::wstring MFCLOC8_4053 = WinSxS+L"\\x86_Microsoft.VC80.MFCLOC_1fc8b3b9a1e18e3b_8.0.50727.4053_x-ww_0ccc058c";
	std::wstring OpenMP8_4053 = WinSxS+L"\\x86_Microsoft.VC80.OpenMP_1fc8b3b9a1e18e3b_8.0.50727.4053_x-ww_e79c4723";
	fver _ATL80_dll_4053 = getFileVer(ATL8_4053+L"\\ATL80.dll",&status);
	fver _mfc80_dll_4053 = getFileVer(MFC8_4053+L"\\mfc80.dll",&status);
	fver _mfc80u_dll_4053 = getFileVer(MFC8_4053+L"\\mfc80u.dll",&status);
	fver _mfcm80_dll_4053   = getFileVer(MFC8_4053+L"\\mfcm80.dll",&status);
	fver _mfcm80u_dll_4053  = getFileVer(MFC8_4053+L"\\mfcm80u.dll",&status);
	fver _mfc80CHS_dll_4053 = getFileVer(MFCLOC8_4053+L"\\mfc80CHS.dll",&status);
	fver _mfc80CHT_dll_4053 = getFileVer(MFCLOC8_4053+L"\\mfc80CHT.dll",&status);
	fver _mfc80DEU_dll_4053 = getFileVer(MFCLOC8_4053+L"\\mfc80DEU.dll",&status);
	fver _mfc80ENU_dll_4053 = getFileVer(MFCLOC8_4053+L"\\mfc80ENU.dll",&status);
	fver _mfc80ESP_dll_4053 = getFileVer(MFCLOC8_4053+L"\\mfc80ESP.dll",&status);
	fver _mfc80FRA_dll_4053 = getFileVer(MFCLOC8_4053+L"\\mfc80FRA.dll",&status);
	fver _mfc80ITA_dll_4053 = getFileVer(MFCLOC8_4053+L"\\mfc80ITA.dll",&status);
	fver _mfc80JPN_dll_4053 = getFileVer(MFCLOC8_4053+L"\\mfc80JPN.dll",&status);
	fver _mfc80KOR_dll_4053 = getFileVer(MFCLOC8_4053+L"\\mfc80KOR.dll",&status);
	fver _msvcm80_dll_4053 = getFileVer(CRT8_4053+L"\\msvcm80.dll",&status);
	fver _msvcp80_dll_4053 = getFileVer(CRT8_4053+L"\\msvcp80.dll",&status);
	fver _msvcr80_dll_4053 = getFileVer(CRT8_4053+L"\\msvcr80.dll",&status);
	fver _vcomp_dll_4053 = getFileVer(OpenMP8_4053+L"\\vcomp.dll",&status);

		if (_ATL80_dll_4053>_ATL80_dll) { _ATL80_dll = _ATL80_dll_4053; }
		if (_mfc80_dll_4053>_mfc80_dll) { _mfc80_dll = _mfc80_dll_4053; }
		if (_mfc80u_dll_4053>_mfc80u_dll) { _mfc80u_dll = _mfc80u_dll_4053; }
		if (_mfcm80_dll_4053>_mfcm80_dll) { _mfcm80_dll = _mfcm80_dll_4053; }
		if (_mfcm80u_dll_4053>_mfcm80u_dll) { _mfcm80u_dll = _mfcm80u_dll_4053; }
		if (_mfc80CHS_dll_4053>_mfc80CHS_dll) { _mfc80CHS_dll = _mfc80CHS_dll_4053; }
		if (_mfc80CHT_dll_4053>_mfc80CHT_dll) { _mfc80CHT_dll = _mfc80CHT_dll_4053; }
		if (_mfc80DEU_dll_4053>_mfc80DEU_dll) { _mfc80DEU_dll = _mfc80DEU_dll_4053; }
		if (_mfc80ENU_dll_4053>_mfc80ENU_dll) { _mfc80ENU_dll = _mfc80ENU_dll_4053; }
		if (_mfc80ESP_dll_4053>_mfc80ESP_dll) { _mfc80ESP_dll = _mfc80ESP_dll_4053; }
		if (_mfc80FRA_dll_4053>_mfc80FRA_dll) { _mfc80FRA_dll = _mfc80FRA_dll_4053; }
		if (_mfc80ITA_dll_4053>_mfc80ITA_dll) { _mfc80ITA_dll = _mfc80ITA_dll_4053; }
		if (_mfc80JPN_dll_4053>_mfc80JPN_dll) { _mfc80JPN_dll = _mfc80JPN_dll_4053; }
		if (_mfc80KOR_dll_4053>_mfc80KOR_dll) { _mfc80KOR_dll = _mfc80KOR_dll_4053; }
		if (_msvcm80_dll_4053>_msvcm80_dll) { _msvcm80_dll = _msvcm80_dll_4053; }
		if (_msvcp80_dll_4053>_msvcp80_dll) { _msvcp80_dll = _msvcp80_dll_4053; }
		if (_msvcr80_dll_4053>_msvcr80_dll) { _msvcr80_dll = _msvcr80_dll_4053; }
		if (_vcomp_dll_4053>_vcomp_dll) { _vcomp_dll = _vcomp_dll_4053; }


	std::wstring    ATL8_5592 = WinSxS+L"\\x86_Microsoft.VC80.ATL_1fc8b3b9a1e18e3b_8.0.50727.5592_x-ww_7837863c";
	std::wstring    CRT8_5592 = WinSxS+L"\\x86_Microsoft.VC80.CRT_1fc8b3b9a1e18e3b_8.0.50727.5592_x-ww_179798c8";
	std::wstring    MFC8_5592 = WinSxS+L"\\x86_Microsoft.VC80.MFC_1fc8b3b9a1e18e3b_8.0.50727.5592_x-ww_e87e0bcd";
	std::wstring MFCLOC8_5592 = WinSxS+L"\\x86_Microsoft.VC80.MFCLOC_1fc8b3b9a1e18e3b_8.0.50727.5592_x-ww_3dcd24cb";
	std::wstring OpenMP8_5592 = WinSxS+L"\\x86_Microsoft.VC80.OpenMP_1fc8b3b9a1e18e3b_8.0.50727.5592_x-ww_189d6662";
	fver _ATL80_dll_5592 = getFileVer(ATL8_5592+L"\\ATL80.dll",&status);
	fver _mfc80_dll_5592 = getFileVer(MFC8_5592+L"\\mfc80.dll",&status);
	fver _mfc80u_dll_5592 = getFileVer(MFC8_5592+L"\\mfc80u.dll",&status);
	fver _mfcm80_dll_5592   = getFileVer(MFC8_5592+L"\\mfcm80.dll",&status);
	fver _mfcm80u_dll_5592  = getFileVer(MFC8_5592+L"\\mfcm80u.dll",&status);
	fver _mfc80CHS_dll_5592 = getFileVer(MFCLOC8_5592+L"\\mfc80CHS.dll",&status);
	fver _mfc80CHT_dll_5592 = getFileVer(MFCLOC8_5592+L"\\mfc80CHT.dll",&status);
	fver _mfc80DEU_dll_5592 = getFileVer(MFCLOC8_5592+L"\\mfc80DEU.dll",&status);
	fver _mfc80ENU_dll_5592 = getFileVer(MFCLOC8_5592+L"\\mfc80ENU.dll",&status);
	fver _mfc80ESP_dll_5592 = getFileVer(MFCLOC8_5592+L"\\mfc80ESP.dll",&status);
	fver _mfc80FRA_dll_5592 = getFileVer(MFCLOC8_5592+L"\\mfc80FRA.dll",&status);
	fver _mfc80ITA_dll_5592 = getFileVer(MFCLOC8_5592+L"\\mfc80ITA.dll",&status);
	fver _mfc80JPN_dll_5592 = getFileVer(MFCLOC8_5592+L"\\mfc80JPN.dll",&status);
	fver _mfc80KOR_dll_5592 = getFileVer(MFCLOC8_5592+L"\\mfc80KOR.dll",&status);
	fver _msvcm80_dll_5592 = getFileVer(CRT8_5592+L"\\msvcm80.dll",&status);
	fver _msvcp80_dll_5592 = getFileVer(CRT8_5592+L"\\msvcp80.dll",&status);
	fver _msvcr80_dll_5592 = getFileVer(CRT8_5592+L"\\msvcr80.dll",&status);
	fver _vcomp_dll_5592 = getFileVer(OpenMP8_5592+L"\\vcomp.dll",&status);

		if (_ATL80_dll_5592>_ATL80_dll) { _ATL80_dll = _ATL80_dll_5592; }
		if (_mfc80_dll_5592>_mfc80_dll) { _mfc80_dll = _mfc80_dll_5592; }
		if (_mfc80u_dll_5592>_mfc80u_dll) { _mfc80u_dll = _mfc80u_dll_5592; }
		if (_mfcm80_dll_5592>_mfcm80_dll) { _mfcm80_dll = _mfcm80_dll_5592; }
		if (_mfcm80u_dll_5592>_mfcm80u_dll) { _mfcm80u_dll = _mfcm80u_dll_5592; }
		if (_mfc80CHS_dll_5592>_mfc80CHS_dll) { _mfc80CHS_dll = _mfc80CHS_dll_5592; }
		if (_mfc80CHT_dll_5592>_mfc80CHT_dll) { _mfc80CHT_dll = _mfc80CHT_dll_5592; }
		if (_mfc80DEU_dll_5592>_mfc80DEU_dll) { _mfc80DEU_dll = _mfc80DEU_dll_5592; }
		if (_mfc80ENU_dll_5592>_mfc80ENU_dll) { _mfc80ENU_dll = _mfc80ENU_dll_5592; }
		if (_mfc80ESP_dll_5592>_mfc80ESP_dll) { _mfc80ESP_dll = _mfc80ESP_dll_5592; }
		if (_mfc80FRA_dll_5592>_mfc80FRA_dll) { _mfc80FRA_dll = _mfc80FRA_dll_5592; }
		if (_mfc80ITA_dll_5592>_mfc80ITA_dll) { _mfc80ITA_dll = _mfc80ITA_dll_5592; }
		if (_mfc80JPN_dll_5592>_mfc80JPN_dll) { _mfc80JPN_dll = _mfc80JPN_dll_5592; }
		if (_mfc80KOR_dll_5592>_mfc80KOR_dll) { _mfc80KOR_dll = _mfc80KOR_dll_5592; }
		if (_msvcm80_dll_5592>_msvcm80_dll) { _msvcm80_dll = _msvcm80_dll_5592; }
		if (_msvcp80_dll_5592>_msvcp80_dll) { _msvcp80_dll = _msvcp80_dll_5592; }
		if (_msvcr80_dll_5592>_msvcr80_dll) { _msvcr80_dll = _msvcr80_dll_5592; }
		if (_vcomp_dll_5592>_vcomp_dll) { _vcomp_dll = _vcomp_dll_5592; }


	std::wstring    ATL8_6195 = WinSxS+L"\\x86_Microsoft.VC80.ATL_1fc8b3b9a1e18e3b_8.0.50727.6195_x-ww_a4c618fa";
	std::wstring    CRT8_6195 = WinSxS+L"\\x86_Microsoft.VC80.CRT_1fc8b3b9a1e18e3b_8.0.50727.6195_x-ww_44262b86";
	std::wstring    MFC8_6195 = WinSxS+L"\\x86_Microsoft.VC80.MFC_1fc8b3b9a1e18e3b_8.0.50727.6195_x-ww_150c9e8b";
	std::wstring MFCLOC8_6195 = WinSxS+L"\\x86_Microsoft.VC80.MFCLOC_1fc8b3b9a1e18e3b_8.0.50727.6195_x-ww_6a5bb789";
	std::wstring OpenMP8_6195 = WinSxS+L"\\x86_Microsoft.VC80.OpenMP_1fc8b3b9a1e18e3b_8.0.50727.6195_x-ww_452bf920";
	fver _ATL80_dll_6195 = getFileVer(ATL8_6195+L"\\ATL80.dll",&status);
	fver _mfc80_dll_6195 = getFileVer(MFC8_6195+L"\\mfc80.dll",&status);
	fver _mfc80u_dll_6195 = getFileVer(MFC8_6195+L"\\mfc80u.dll",&status);
	fver _mfcm80_dll_6195   = getFileVer(MFC8_6195+L"\\mfcm80.dll",&status);
	fver _mfcm80u_dll_6195  = getFileVer(MFC8_6195+L"\\mfcm80u.dll",&status);
	fver _mfc80CHS_dll_6195 = getFileVer(MFCLOC8_6195+L"\\mfc80CHS.dll",&status);
	fver _mfc80CHT_dll_6195 = getFileVer(MFCLOC8_6195+L"\\mfc80CHT.dll",&status);
	fver _mfc80DEU_dll_6195 = getFileVer(MFCLOC8_6195+L"\\mfc80DEU.dll",&status);
	fver _mfc80ENU_dll_6195 = getFileVer(MFCLOC8_6195+L"\\mfc80ENU.dll",&status);
	fver _mfc80ESP_dll_6195 = getFileVer(MFCLOC8_6195+L"\\mfc80ESP.dll",&status);
	fver _mfc80FRA_dll_6195 = getFileVer(MFCLOC8_6195+L"\\mfc80FRA.dll",&status);
	fver _mfc80ITA_dll_6195 = getFileVer(MFCLOC8_6195+L"\\mfc80ITA.dll",&status);
	fver _mfc80JPN_dll_6195 = getFileVer(MFCLOC8_6195+L"\\mfc80JPN.dll",&status);
	fver _mfc80KOR_dll_6195 = getFileVer(MFCLOC8_6195+L"\\mfc80KOR.dll",&status);
	fver _msvcm80_dll_6195 = getFileVer(CRT8_6195+L"\\msvcm80.dll",&status);
	fver _msvcp80_dll_6195 = getFileVer(CRT8_6195+L"\\msvcp80.dll",&status);
	fver _msvcr80_dll_6195 = getFileVer(CRT8_6195+L"\\msvcr80.dll",&status);
	fver _vcomp_dll_6195 = getFileVer(OpenMP8_6195+L"\\vcomp.dll",&status);

		if (_ATL80_dll_6195>_ATL80_dll) { _ATL80_dll = _ATL80_dll_6195; }
		if (_mfc80_dll_6195>_mfc80_dll) { _mfc80_dll = _mfc80_dll_6195; }
		if (_mfc80u_dll_6195>_mfc80u_dll) { _mfc80u_dll = _mfc80u_dll_6195; }
		if (_mfcm80_dll_6195>_mfcm80_dll) { _mfcm80_dll = _mfcm80_dll_6195; }
		if (_mfcm80u_dll_6195>_mfcm80u_dll) { _mfcm80u_dll = _mfcm80u_dll_6195; }
		if (_mfc80CHS_dll_6195>_mfc80CHS_dll) { _mfc80CHS_dll = _mfc80CHS_dll_6195; }
		if (_mfc80CHT_dll_6195>_mfc80CHT_dll) { _mfc80CHT_dll = _mfc80CHT_dll_6195; }
		if (_mfc80DEU_dll_6195>_mfc80DEU_dll) { _mfc80DEU_dll = _mfc80DEU_dll_6195; }
		if (_mfc80ENU_dll_6195>_mfc80ENU_dll) { _mfc80ENU_dll = _mfc80ENU_dll_6195; }
		if (_mfc80ESP_dll_6195>_mfc80ESP_dll) { _mfc80ESP_dll = _mfc80ESP_dll_6195; }
		if (_mfc80FRA_dll_6195>_mfc80FRA_dll) { _mfc80FRA_dll = _mfc80FRA_dll_6195; }
		if (_mfc80ITA_dll_6195>_mfc80ITA_dll) { _mfc80ITA_dll = _mfc80ITA_dll_6195; }
		if (_mfc80JPN_dll_6195>_mfc80JPN_dll) { _mfc80JPN_dll = _mfc80JPN_dll_6195; }
		if (_mfc80KOR_dll_6195>_mfc80KOR_dll) { _mfc80KOR_dll = _mfc80KOR_dll_6195; }
		if (_msvcm80_dll_6195>_msvcm80_dll) { _msvcm80_dll = _msvcm80_dll_6195; }
		if (_msvcp80_dll_6195>_msvcp80_dll) { _msvcp80_dll = _msvcp80_dll_6195; }
		if (_msvcr80_dll_6195>_msvcr80_dll) { _msvcr80_dll = _msvcr80_dll_6195; }
		if (_vcomp_dll_6195>_vcomp_dll) { _vcomp_dll = _vcomp_dll_6195; }
/*
	printf("Microsoft Visual C++ 2005 DEBUG:\n");
	printf("%s\t%s\n","ATL80.dll",_ATL80_dll.format().c_str());
	printf("%s\t%s\n","msdia80.dll",_msdia80_dll.format().c_str());
	printf("%s\t%s\n","mfc80.dll",_mfc80_dll.format().c_str());
	printf("%s\t%s\n","mfc80u.dll",_mfc80u_dll.format().c_str());
	printf("%s\t%s\n","mfcm80.dll",_mfcm80_dll.format().c_str());
	printf("%s\t%s\n","mfcm80u.dll",_mfcm80u_dll.format().c_str());
	printf("%s\t%s\n","mfc80CHS.dll",_mfc80CHS_dll.format().c_str());
	printf("%s\t%s\n","mfc80CHT.dll",_mfc80CHT_dll.format().c_str());
	printf("%s\t%s\n","mfc80DEU.dll",_mfc80DEU_dll.format().c_str());
	printf("%s\t%s\n","mfc80ENU.dll",_mfc80ENU_dll.format().c_str());
	printf("%s\t%s\n","mfc80ESP.dll",_mfc80ESP_dll.format().c_str());
	printf("%s\t%s\n","mfc80FRA.dll",_mfc80FRA_dll.format().c_str());
	printf("%s\t%s\n","mfc80ITA.dll",_mfc80ITA_dll.format().c_str());
	printf("%s\t%s\n","mfc80JPN.dll",_mfc80JPN_dll.format().c_str());
	printf("%s\t%s\n","mfc80KOR.dll",_mfc80KOR_dll.format().c_str());
	printf("%s\t%s\n","msvcm80.dll",_msvcm80_dll.format().c_str());
	printf("%s\t%s\n","msvcp80.dll",_msvcp80_dll.format().c_str());
	printf("%s\t%s\n","msvcr80.dll",_msvcr80_dll.format().c_str());
	printf("%s\t%s\n","vcomp.dll",_vcomp_dll.format().c_str());
	printf("\n");
*/
	std::wstring      ATL9_8 = WinSxS+L"\\x86_Microsoft.VC90.ATL_1fc8b3b9a1e18e3b_9.0.21022.8_x-ww_312cf0e9";
	std::wstring      CRT9_8 = WinSxS+L"\\x86_Microsoft.VC90.CRT_1fc8b3b9a1e18e3b_9.0.21022.8_x-ww_d08d0375";
	std::wstring      MFC9_8 = WinSxS+L"\\x86_Microsoft.VC90.MFC_1fc8b3b9a1e18e3b_9.0.21022.8_x-ww_a173767a";
	std::wstring   MFCLOC9_8 = WinSxS+L"\\x86_Microsoft.VC90.MFCLOC_1fc8b3b9a1e18e3b_9.0.21022.8_x-ww_11f3ea3a";
	std::wstring   OpenMP9_8 = WinSxS+L"\\x86_Microsoft.VC90.OpenMP_1fc8b3b9a1e18e3b_9.0.21022.8_x-ww_ecc42bd1";

	fver _atl90_dll_8 = getFileVer(ATL9_8+L"\\atl90.dll",&status);
	fver _msvcm90_dll_8 = getFileVer(CRT9_8+L"\\msvcm90.dll",&status);
	fver _msvcp90_dll_8 = getFileVer(CRT9_8+L"\\msvcp90.dll",&status);
	fver _msvcr90_dll_8 = getFileVer(CRT9_8+L"\\msvcr90.dll",&status);
	fver _mfc90_dll_8 = getFileVer(MFC9_8+L"\\mfc90.dll",&status);
	fver _mfc90u_dll_8 = getFileVer(MFC9_8+L"\\mfc90u.dll",&status);
	fver _mfcm90_dll_8 = getFileVer(MFC9_8+L"\\mfcm90.dll",&status);
	fver _mfcm90u_dll_8 = getFileVer(MFC9_8+L"\\mfcm90u.dll",&status);
	fver _mfc90chs_dll_8 = getFileVer(MFCLOC9_8+L"\\mfc90chs.dll",&status);
	fver _mfc90cht_dll_8 = getFileVer(MFCLOC9_8+L"\\mfc90cht.dll",&status);
	fver _mfc90deu_dll_8 = getFileVer(MFCLOC9_8+L"\\mfc90deu.dll",&status);
	fver _mfc90enu_dll_8 = getFileVer(MFCLOC9_8+L"\\mfc90enu.dll",&status);
	fver _mfc90esn_dll_8 = getFileVer(MFCLOC9_8+L"\\mfc90esn.dll",&status);
	fver _mfc90esp_dll_8 = getFileVer(MFCLOC9_8+L"\\mfc90esp.dll",&status);
	fver _mfc90fra_dll_8 = getFileVer(MFCLOC9_8+L"\\mfc90fra.dll",&status);
	fver _mfc90ita_dll_8 = getFileVer(MFCLOC9_8+L"\\mfc90ita.dll",&status);
	fver _mfc90jpn_dll_8 = getFileVer(MFCLOC9_8+L"\\mfc90jpn.dll",&status);
	fver _mfc90kor_dll_8 = getFileVer(MFCLOC9_8+L"\\mfc90kor.dll",&status);
	fver _vcomp90_dll_8 = getFileVer(OpenMP9_8+L"\\vcomp90.dll",&status);

	fver _atl90_dll = _atl90_dll_8;

	// These three files installed with .NET Framework
	fver _msvcm90_dll = fver(); //_msvcm90_dll_8;
	fver _msvcp90_dll = fver(); //_msvcp90_dll_8;
	fver _msvcr90_dll = fver(); //_msvcr90_dll_8;

	fver _mfc90_dll = _mfc90_dll_8;
	fver _mfc90u_dll = _mfc90u_dll_8;
	fver _mfcm90_dll = _mfcm90_dll_8;
	fver _mfcm90u_dll = _mfcm90u_dll_8;
	fver _mfc90chs_dll = _mfc90chs_dll_8;
	fver _mfc90cht_dll = _mfc90cht_dll_8;
	fver _mfc90deu_dll = _mfc90deu_dll_8;
	fver _mfc90enu_dll = _mfc90enu_dll_8;
	fver _mfc90esn_dll = _mfc90esn_dll_8;
	fver _mfc90esp_dll = _mfc90esp_dll_8;
	fver _mfc90fra_dll = _mfc90fra_dll_8;
	fver _mfc90ita_dll = _mfc90ita_dll_8;
	fver _mfc90jpn_dll = _mfc90jpn_dll_8;
	fver _mfc90kor_dll = _mfc90kor_dll_8;
	fver _vcomp90_dll = _vcomp90_dll_8;

	std::wstring      ATL9_218 = WinSxS+L"\\x86_Microsoft.VC90.ATL_1fc8b3b9a1e18e3b_9.0.21022.218_x-ww_42101c2a";
	std::wstring      CRT9_218 = WinSxS+L"\\x86_Microsoft.VC90.CRT_1fc8b3b9a1e18e3b_9.0.21022.218_x-ww_e1702eb6";
	std::wstring      MFC9_218 = WinSxS+L"\\x86_Microsoft.VC90.MFC_1fc8b3b9a1e18e3b_9.0.21022.218_x-ww_b256a1bb";
	std::wstring   MFCLOC9_218 = WinSxS+L"\\x86_Microsoft.VC90.MFCLOC_1fc8b3b9a1e18e3b_9.0.21022.218_x-ww_22d7157b";
	std::wstring   OpenMP9_218 = WinSxS+L"\\x86_Microsoft.VC90.OpenMP_1fc8b3b9a1e18e3b_9.0.21022.218_x-ww_fda75712";

	fver _atl90_dll_218 = getFileVer(ATL9_218+L"\\atl90.dll",&status);
	fver _msvcm90_dll_218 = getFileVer(CRT9_218+L"\\msvcm90.dll",&status);
	fver _msvcp90_dll_218 = getFileVer(CRT9_218+L"\\msvcp90.dll",&status);
	fver _msvcr90_dll_218 = getFileVer(CRT9_218+L"\\msvcr90.dll",&status);
	fver _mfc90_dll_218 = getFileVer(MFC9_218+L"\\mfc90.dll",&status);
	fver _mfc90u_dll_218 = getFileVer(MFC9_218+L"\\mfc90u.dll",&status);
	fver _mfcm90_dll_218 = getFileVer(MFC9_218+L"\\mfcm90.dll",&status);
	fver _mfcm90u_dll_218 = getFileVer(MFC9_218+L"\\mfcm90u.dll",&status);
	fver _mfc90chs_dll_218 = getFileVer(MFCLOC9_218+L"\\mfc90chs.dll",&status);
	fver _mfc90cht_dll_218 = getFileVer(MFCLOC9_218+L"\\mfc90cht.dll",&status);
	fver _mfc90deu_dll_218 = getFileVer(MFCLOC9_218+L"\\mfc90deu.dll",&status);
	fver _mfc90enu_dll_218 = getFileVer(MFCLOC9_218+L"\\mfc90enu.dll",&status);
	fver _mfc90esn_dll_218 = getFileVer(MFCLOC9_218+L"\\mfc90esn.dll",&status);
	fver _mfc90esp_dll_218 = getFileVer(MFCLOC9_218+L"\\mfc90esp.dll",&status);
	fver _mfc90fra_dll_218 = getFileVer(MFCLOC9_218+L"\\mfc90fra.dll",&status);
	fver _mfc90ita_dll_218 = getFileVer(MFCLOC9_218+L"\\mfc90ita.dll",&status);
	fver _mfc90jpn_dll_218 = getFileVer(MFCLOC9_218+L"\\mfc90jpn.dll",&status);
	fver _mfc90kor_dll_218 = getFileVer(MFCLOC9_218+L"\\mfc90kor.dll",&status);
	fver _vcomp90_dll_218 = getFileVer(OpenMP9_218+L"\\vcomp90.dll",&status);

	if(_atl90_dll_218    >_atl90_dll   ) {_atl90_dll    = _atl90_dll_218;   }
	if(_msvcm90_dll_218  >_msvcm90_dll ) {_msvcm90_dll  = _msvcm90_dll_218; }
	if(_msvcp90_dll_218  >_msvcp90_dll ) {_msvcp90_dll  = _msvcp90_dll_218; }
	if(_msvcr90_dll_218  >_msvcr90_dll ) {_msvcr90_dll  = _msvcr90_dll_218; }
	if(_mfc90_dll_218    >_mfc90_dll   ) {_mfc90_dll    = _mfc90_dll_218;   }
	if(_mfc90u_dll_218   >_mfc90u_dll  ) {_mfc90u_dll   = _mfc90u_dll_218;  }
	if(_mfcm90_dll_218   >_mfcm90_dll  ) {_mfcm90_dll   = _mfcm90_dll_218;  }
	if(_mfcm90u_dll_218  >_mfcm90u_dll ) {_mfcm90u_dll  = _mfcm90u_dll_218; }
	if(_mfc90chs_dll_218 >_mfc90chs_dll) {_mfc90chs_dll = _mfc90chs_dll_218;}
	if(_mfc90cht_dll_218 >_mfc90cht_dll) {_mfc90cht_dll = _mfc90cht_dll_218;}
	if(_mfc90deu_dll_218 >_mfc90deu_dll) {_mfc90deu_dll = _mfc90deu_dll_218;}
	if(_mfc90enu_dll_218 >_mfc90enu_dll) {_mfc90enu_dll = _mfc90enu_dll_218;}
	if(_mfc90esn_dll_218 >_mfc90esn_dll) {_mfc90esn_dll = _mfc90esn_dll_218;}
	if(_mfc90esp_dll_218 >_mfc90esp_dll) {_mfc90esp_dll = _mfc90esp_dll_218;}
	if(_mfc90fra_dll_218 >_mfc90fra_dll) {_mfc90fra_dll = _mfc90fra_dll_218;}
	if(_mfc90ita_dll_218 >_mfc90ita_dll) {_mfc90ita_dll = _mfc90ita_dll_218;}
	if(_mfc90jpn_dll_218 >_mfc90jpn_dll) {_mfc90jpn_dll = _mfc90jpn_dll_218;}
	if(_mfc90kor_dll_218 >_mfc90kor_dll) {_mfc90kor_dll = _mfc90kor_dll_218;}
	if(_vcomp90_dll_218  >_vcomp90_dll ) {_vcomp90_dll  = _vcomp90_dll_218; }

	std::wstring      ATL9_001 = WinSxS+L"\\x86_Microsoft.VC90.ATL_1fc8b3b9a1e18e3b_9.0.30729.1_x-ww_d01483b2";
	std::wstring      CRT9_001 = WinSxS+L"\\x86_Microsoft.VC90.CRT_1fc8b3b9a1e18e3b_9.0.30729.1_x-ww_6f74963e";
	std::wstring      MFC9_001 = WinSxS+L"\\x86_Microsoft.VC90.MFC_1fc8b3b9a1e18e3b_9.0.30729.1_x-ww_405b0943";
	std::wstring   MFCLOC9_001 = WinSxS+L"\\x86_Microsoft.VC90.MFCLOC_1fc8b3b9a1e18e3b_9.0.30729.1_x-ww_b0db7d03";
//	std::wstring   OpenMP9_001 = WinSxS+L"\\";

	fver _atl90_dll_001 = getFileVer(ATL9_001+L"\\atl90.dll",&status);
	fver _msvcm90_dll_001 = getFileVer(CRT9_001+L"\\msvcm90.dll",&status);
	fver _msvcp90_dll_001 = getFileVer(CRT9_001+L"\\msvcp90.dll",&status);
	fver _msvcr90_dll_001 = getFileVer(CRT9_001+L"\\msvcr90.dll",&status);
	fver _mfc90_dll_001 = getFileVer(MFC9_001+L"\\mfc90.dll",&status);
	fver _mfc90u_dll_001 = getFileVer(MFC9_001+L"\\mfc90u.dll",&status);
	fver _mfcm90_dll_001 = getFileVer(MFC9_001+L"\\mfcm90.dll",&status);
	fver _mfcm90u_dll_001 = getFileVer(MFC9_001+L"\\mfcm90u.dll",&status);
	fver _mfc90chs_dll_001 = getFileVer(MFCLOC9_001+L"\\mfc90chs.dll",&status);
	fver _mfc90cht_dll_001 = getFileVer(MFCLOC9_001+L"\\mfc90cht.dll",&status);
	fver _mfc90deu_dll_001 = getFileVer(MFCLOC9_001+L"\\mfc90deu.dll",&status);
	fver _mfc90enu_dll_001 = getFileVer(MFCLOC9_001+L"\\mfc90enu.dll",&status);
	fver _mfc90esn_dll_001 = getFileVer(MFCLOC9_001+L"\\mfc90esn.dll",&status);
	fver _mfc90esp_dll_001 = getFileVer(MFCLOC9_001+L"\\mfc90esp.dll",&status);
	fver _mfc90fra_dll_001 = getFileVer(MFCLOC9_001+L"\\mfc90fra.dll",&status);
	fver _mfc90ita_dll_001 = getFileVer(MFCLOC9_001+L"\\mfc90ita.dll",&status);
	fver _mfc90jpn_dll_001 = getFileVer(MFCLOC9_001+L"\\mfc90jpn.dll",&status);
	fver _mfc90kor_dll_001 = getFileVer(MFCLOC9_001+L"\\mfc90kor.dll",&status);
//	fver _vcomp90_dll_001 = getFileVer(OpenMP9_001+L"\\vcomp90.dll",&status);

	if(_atl90_dll_001    >_atl90_dll   ) {_atl90_dll    = _atl90_dll_001;   }
	if(_msvcm90_dll_001  >_msvcm90_dll ) {_msvcm90_dll  = _msvcm90_dll_001; }
	if(_msvcp90_dll_001  >_msvcp90_dll ) {_msvcp90_dll  = _msvcp90_dll_001; }
	if(_msvcr90_dll_001  >_msvcr90_dll ) {_msvcr90_dll  = _msvcr90_dll_001; }
	if(_mfc90_dll_001    >_mfc90_dll   ) {_mfc90_dll    = _mfc90_dll_001;   }
	if(_mfc90u_dll_001   >_mfc90u_dll  ) {_mfc90u_dll   = _mfc90u_dll_001;  }
	if(_mfcm90_dll_001   >_mfcm90_dll  ) {_mfcm90_dll   = _mfcm90_dll_001;  }
	if(_mfcm90u_dll_001  >_mfcm90u_dll ) {_mfcm90u_dll  = _mfcm90u_dll_001; }
	if(_mfc90chs_dll_001 >_mfc90chs_dll) {_mfc90chs_dll = _mfc90chs_dll_001;}
	if(_mfc90cht_dll_001 >_mfc90cht_dll) {_mfc90cht_dll = _mfc90cht_dll_001;}
	if(_mfc90deu_dll_001 >_mfc90deu_dll) {_mfc90deu_dll = _mfc90deu_dll_001;}
	if(_mfc90enu_dll_001 >_mfc90enu_dll) {_mfc90enu_dll = _mfc90enu_dll_001;}
	if(_mfc90esn_dll_001 >_mfc90esn_dll) {_mfc90esn_dll = _mfc90esn_dll_001;}
	if(_mfc90esp_dll_001 >_mfc90esp_dll) {_mfc90esp_dll = _mfc90esp_dll_001;}
	if(_mfc90fra_dll_001 >_mfc90fra_dll) {_mfc90fra_dll = _mfc90fra_dll_001;}
	if(_mfc90ita_dll_001 >_mfc90ita_dll) {_mfc90ita_dll = _mfc90ita_dll_001;}
	if(_mfc90jpn_dll_001 >_mfc90jpn_dll) {_mfc90jpn_dll = _mfc90jpn_dll_001;}
	if(_mfc90kor_dll_001 >_mfc90kor_dll) {_mfc90kor_dll = _mfc90kor_dll_001;}
//	if(_vcomp90_dll_001  >_vcomp90_dll ) {_vcomp90_dll  = _vcomp90_dll_001; }

	std::wstring      ATL9_4148 = WinSxS+L"\\x86_Microsoft.VC90.ATL_1fc8b3b9a1e18e3b_9.0.30729.4148_x-ww_353599c2";
	std::wstring      CRT9_4148 = WinSxS+L"\\x86_Microsoft.VC90.CRT_1fc8b3b9a1e18e3b_9.0.30729.4148_x-ww_d495ac4e";
	std::wstring      MFC9_4148 = WinSxS+L"\\x86_Microsoft.VC90.MFC_1fc8b3b9a1e18e3b_9.0.30729.4148_x-ww_a57c1f53";
	std::wstring   MFCLOC9_4148 = WinSxS+L"\\x86_Microsoft.VC90.MFCLOC_1fc8b3b9a1e18e3b_9.0.30729.4148_x-ww_15fc9313";
	std::wstring   OpenMP9_4148 = WinSxS+L"\\x86_Microsoft.VC90.OpenMP_1fc8b3b9a1e18e3b_9.0.30729.4148_x-ww_f0ccd4aa";

	fver _atl90_dll_4148 = getFileVer(ATL9_4148+L"\\atl90.dll",&status);
	fver _msvcm90_dll_4148 = getFileVer(CRT9_4148+L"\\msvcm90.dll",&status);
	fver _msvcp90_dll_4148 = getFileVer(CRT9_4148+L"\\msvcp90.dll",&status);
	fver _msvcr90_dll_4148 = getFileVer(CRT9_4148+L"\\msvcr90.dll",&status);
	fver _mfc90_dll_4148 = getFileVer(MFC9_4148+L"\\mfc90.dll",&status);
	fver _mfc90u_dll_4148 = getFileVer(MFC9_4148+L"\\mfc90u.dll",&status);
	fver _mfcm90_dll_4148 = getFileVer(MFC9_4148+L"\\mfcm90.dll",&status);
	fver _mfcm90u_dll_4148 = getFileVer(MFC9_4148+L"\\mfcm90u.dll",&status);
	fver _mfc90chs_dll_4148 = getFileVer(MFCLOC9_4148+L"\\mfc90chs.dll",&status);
	fver _mfc90cht_dll_4148 = getFileVer(MFCLOC9_4148+L"\\mfc90cht.dll",&status);
	fver _mfc90deu_dll_4148 = getFileVer(MFCLOC9_4148+L"\\mfc90deu.dll",&status);
	fver _mfc90enu_dll_4148 = getFileVer(MFCLOC9_4148+L"\\mfc90enu.dll",&status);
	fver _mfc90esn_dll_4148 = getFileVer(MFCLOC9_4148+L"\\mfc90esn.dll",&status);
	fver _mfc90esp_dll_4148 = getFileVer(MFCLOC9_4148+L"\\mfc90esp.dll",&status);
	fver _mfc90fra_dll_4148 = getFileVer(MFCLOC9_4148+L"\\mfc90fra.dll",&status);
	fver _mfc90ita_dll_4148 = getFileVer(MFCLOC9_4148+L"\\mfc90ita.dll",&status);
	fver _mfc90jpn_dll_4148 = getFileVer(MFCLOC9_4148+L"\\mfc90jpn.dll",&status);
	fver _mfc90kor_dll_4148 = getFileVer(MFCLOC9_4148+L"\\mfc90kor.dll",&status);
	fver _vcomp90_dll_4148 = getFileVer(OpenMP9_4148+L"\\vcomp90.dll",&status);

	if(_atl90_dll_4148    >_atl90_dll   ) {_atl90_dll    = _atl90_dll_4148;   }
	if(_msvcm90_dll_4148  >_msvcm90_dll ) {_msvcm90_dll  = _msvcm90_dll_4148; }
	if(_msvcp90_dll_4148  >_msvcp90_dll ) {_msvcp90_dll  = _msvcp90_dll_4148; }
	if(_msvcr90_dll_4148  >_msvcr90_dll ) {_msvcr90_dll  = _msvcr90_dll_4148; }
	if(_mfc90_dll_4148    >_mfc90_dll   ) {_mfc90_dll    = _mfc90_dll_4148;   }
	if(_mfc90u_dll_4148   >_mfc90u_dll  ) {_mfc90u_dll   = _mfc90u_dll_4148;  }
	if(_mfcm90_dll_4148   >_mfcm90_dll  ) {_mfcm90_dll   = _mfcm90_dll_4148;  }
	if(_mfcm90u_dll_4148  >_mfcm90u_dll ) {_mfcm90u_dll  = _mfcm90u_dll_4148; }
	if(_mfc90chs_dll_4148 >_mfc90chs_dll) {_mfc90chs_dll = _mfc90chs_dll_4148;}
	if(_mfc90cht_dll_4148 >_mfc90cht_dll) {_mfc90cht_dll = _mfc90cht_dll_4148;}
	if(_mfc90deu_dll_4148 >_mfc90deu_dll) {_mfc90deu_dll = _mfc90deu_dll_4148;}
	if(_mfc90enu_dll_4148 >_mfc90enu_dll) {_mfc90enu_dll = _mfc90enu_dll_4148;}
	if(_mfc90esn_dll_4148 >_mfc90esn_dll) {_mfc90esn_dll = _mfc90esn_dll_4148;}
	if(_mfc90esp_dll_4148 >_mfc90esp_dll) {_mfc90esp_dll = _mfc90esp_dll_4148;}
	if(_mfc90fra_dll_4148 >_mfc90fra_dll) {_mfc90fra_dll = _mfc90fra_dll_4148;}
	if(_mfc90ita_dll_4148 >_mfc90ita_dll) {_mfc90ita_dll = _mfc90ita_dll_4148;}
	if(_mfc90jpn_dll_4148 >_mfc90jpn_dll) {_mfc90jpn_dll = _mfc90jpn_dll_4148;}
	if(_mfc90kor_dll_4148 >_mfc90kor_dll) {_mfc90kor_dll = _mfc90kor_dll_4148;}
	if(_vcomp90_dll_4148  >_vcomp90_dll ) {_vcomp90_dll  = _vcomp90_dll_4148; }

	std::wstring      ATL9_5570 = WinSxS+L"\\x86_Microsoft.VC90.ATL_1fc8b3b9a1e18e3b_9.0.30729.5570_x-ww_65b7a93a";
	std::wstring      CRT9_5570 = WinSxS+L"\\x86_Microsoft.VC90.CRT_1fc8b3b9a1e18e3b_9.0.30729.5570_x-ww_0517bbc6";
	std::wstring      MFC9_5570 = WinSxS+L"\\x86_Microsoft.VC90.MFC_1fc8b3b9a1e18e3b_9.0.30729.5570_x-ww_d5fe2ecb";
	std::wstring   MFCLOC9_5570 = WinSxS+L"\\x86_Microsoft.VC90.MFCLOC_1fc8b3b9a1e18e3b_9.0.30729.5570_x-ww_467ea28b";
	std::wstring   OpenMP9_5570 = WinSxS+L"\\x86_Microsoft.VC90.OpenMP_1fc8b3b9a1e18e3b_9.0.30729.5570_x-ww_214ee422";

	fver _atl90_dll_5570 = getFileVer(ATL9_5570+L"\\atl90.dll",&status);
	fver _msvcm90_dll_5570 = getFileVer(CRT9_5570+L"\\msvcm90.dll",&status);
	fver _msvcp90_dll_5570 = getFileVer(CRT9_5570+L"\\msvcp90.dll",&status);
	fver _msvcr90_dll_5570 = getFileVer(CRT9_5570+L"\\msvcr90.dll",&status);
	fver _mfc90_dll_5570 = getFileVer(MFC9_5570+L"\\mfc90.dll",&status);
	fver _mfc90u_dll_5570 = getFileVer(MFC9_5570+L"\\mfc90u.dll",&status);
	fver _mfcm90_dll_5570 = getFileVer(MFC9_5570+L"\\mfcm90.dll",&status);
	fver _mfcm90u_dll_5570 = getFileVer(MFC9_5570+L"\\mfcm90u.dll",&status);
	fver _mfc90chs_dll_5570 = getFileVer(MFCLOC9_5570+L"\\mfc90chs.dll",&status);
	fver _mfc90cht_dll_5570 = getFileVer(MFCLOC9_5570+L"\\mfc90cht.dll",&status);
	fver _mfc90deu_dll_5570 = getFileVer(MFCLOC9_5570+L"\\mfc90deu.dll",&status);
	fver _mfc90enu_dll_5570 = getFileVer(MFCLOC9_5570+L"\\mfc90enu.dll",&status);
	fver _mfc90esn_dll_5570 = getFileVer(MFCLOC9_5570+L"\\mfc90esn.dll",&status);
	fver _mfc90esp_dll_5570 = getFileVer(MFCLOC9_5570+L"\\mfc90esp.dll",&status);
	fver _mfc90fra_dll_5570 = getFileVer(MFCLOC9_5570+L"\\mfc90fra.dll",&status);
	fver _mfc90ita_dll_5570 = getFileVer(MFCLOC9_5570+L"\\mfc90ita.dll",&status);
	fver _mfc90jpn_dll_5570 = getFileVer(MFCLOC9_5570+L"\\mfc90jpn.dll",&status);
	fver _mfc90kor_dll_5570 = getFileVer(MFCLOC9_5570+L"\\mfc90kor.dll",&status);
	fver _vcomp90_dll_5570 = getFileVer(OpenMP9_5570+L"\\vcomp90.dll",&status);

	if(_atl90_dll_5570    >_atl90_dll   ) {_atl90_dll    = _atl90_dll_5570;   }
	if(_msvcm90_dll_5570  >_msvcm90_dll ) {_msvcm90_dll  = _msvcm90_dll_5570; }
	if(_msvcp90_dll_5570  >_msvcp90_dll ) {_msvcp90_dll  = _msvcp90_dll_5570; }
	if(_msvcr90_dll_5570  >_msvcr90_dll ) {_msvcr90_dll  = _msvcr90_dll_5570; }
	if(_mfc90_dll_5570    >_mfc90_dll   ) {_mfc90_dll    = _mfc90_dll_5570;   }
	if(_mfc90u_dll_5570   >_mfc90u_dll  ) {_mfc90u_dll   = _mfc90u_dll_5570;  }
	if(_mfcm90_dll_5570   >_mfcm90_dll  ) {_mfcm90_dll   = _mfcm90_dll_5570;  }
	if(_mfcm90u_dll_5570  >_mfcm90u_dll ) {_mfcm90u_dll  = _mfcm90u_dll_5570; }
	if(_mfc90chs_dll_5570 >_mfc90chs_dll) {_mfc90chs_dll = _mfc90chs_dll_5570;}
	if(_mfc90cht_dll_5570 >_mfc90cht_dll) {_mfc90cht_dll = _mfc90cht_dll_5570;}
	if(_mfc90deu_dll_5570 >_mfc90deu_dll) {_mfc90deu_dll = _mfc90deu_dll_5570;}
	if(_mfc90enu_dll_5570 >_mfc90enu_dll) {_mfc90enu_dll = _mfc90enu_dll_5570;}
	if(_mfc90esn_dll_5570 >_mfc90esn_dll) {_mfc90esn_dll = _mfc90esn_dll_5570;}
	if(_mfc90esp_dll_5570 >_mfc90esp_dll) {_mfc90esp_dll = _mfc90esp_dll_5570;}
	if(_mfc90fra_dll_5570 >_mfc90fra_dll) {_mfc90fra_dll = _mfc90fra_dll_5570;}
	if(_mfc90ita_dll_5570 >_mfc90ita_dll) {_mfc90ita_dll = _mfc90ita_dll_5570;}
	if(_mfc90jpn_dll_5570 >_mfc90jpn_dll) {_mfc90jpn_dll = _mfc90jpn_dll_5570;}
	if(_mfc90kor_dll_5570 >_mfc90kor_dll) {_mfc90kor_dll = _mfc90kor_dll_5570;}
	if(_vcomp90_dll_5570  >_vcomp90_dll ) {_vcomp90_dll  = _vcomp90_dll_5570; }

	std::wstring      ATL9_6161 = WinSxS+L"\\x86_Microsoft.VC90.ATL_1fc8b3b9a1e18e3b_9.0.30729.6161_x-ww_92453bb7";
	std::wstring      CRT9_6161 = WinSxS+L"\\x86_Microsoft.VC90.CRT_1fc8b3b9a1e18e3b_9.0.30729.6161_x-ww_31a54e43";
	std::wstring      MFC9_6161 = WinSxS+L"\\x86_Microsoft.VC90.MFC_1fc8b3b9a1e18e3b_9.0.30729.6161_x-ww_028bc148";
	std::wstring   MFCLOC9_6161 = WinSxS+L"\\x86_Microsoft.VC90.MFCLOC_1fc8b3b9a1e18e3b_9.0.30729.6161_x-ww_730c3508";
	std::wstring   OpenMP9_6161 = WinSxS+L"\\x86_Microsoft.VC90.OpenMP_1fc8b3b9a1e18e3b_9.0.30729.6161_x-ww_4ddc769f";

	fver _atl90_dll_6161 = getFileVer(ATL9_6161+L"\\atl90.dll",&status);
	fver _msvcm90_dll_6161 = getFileVer(CRT9_6161+L"\\msvcm90.dll",&status);
	fver _msvcp90_dll_6161 = getFileVer(CRT9_6161+L"\\msvcp90.dll",&status);
	fver _msvcr90_dll_6161 = getFileVer(CRT9_6161+L"\\msvcr90.dll",&status);
	fver _mfc90_dll_6161 = getFileVer(MFC9_6161+L"\\mfc90.dll",&status);
	fver _mfc90u_dll_6161 = getFileVer(MFC9_6161+L"\\mfc90u.dll",&status);
	fver _mfcm90_dll_6161 = getFileVer(MFC9_6161+L"\\mfcm90.dll",&status);
	fver _mfcm90u_dll_6161 = getFileVer(MFC9_6161+L"\\mfcm90u.dll",&status);
	fver _mfc90chs_dll_6161 = getFileVer(MFCLOC9_6161+L"\\mfc90chs.dll",&status);
	fver _mfc90cht_dll_6161 = getFileVer(MFCLOC9_6161+L"\\mfc90cht.dll",&status);
	fver _mfc90deu_dll_6161 = getFileVer(MFCLOC9_6161+L"\\mfc90deu.dll",&status);
	fver _mfc90enu_dll_6161 = getFileVer(MFCLOC9_6161+L"\\mfc90enu.dll",&status);
	fver _mfc90esn_dll_6161 = getFileVer(MFCLOC9_6161+L"\\mfc90esn.dll",&status);
	fver _mfc90esp_dll_6161 = getFileVer(MFCLOC9_6161+L"\\mfc90esp.dll",&status);
	fver _mfc90fra_dll_6161 = getFileVer(MFCLOC9_6161+L"\\mfc90fra.dll",&status);
	fver _mfc90ita_dll_6161 = getFileVer(MFCLOC9_6161+L"\\mfc90ita.dll",&status);
	fver _mfc90jpn_dll_6161 = getFileVer(MFCLOC9_6161+L"\\mfc90jpn.dll",&status);
	fver _mfc90kor_dll_6161 = getFileVer(MFCLOC9_6161+L"\\mfc90kor.dll",&status);
	fver _vcomp90_dll_6161 = getFileVer(OpenMP9_6161+L"\\vcomp90.dll",&status);

	if(_atl90_dll_6161    >_atl90_dll   ) {_atl90_dll    = _atl90_dll_6161;   }
	if(_msvcm90_dll_6161  >_msvcm90_dll ) {_msvcm90_dll  = _msvcm90_dll_6161; }
	if(_msvcp90_dll_6161  >_msvcp90_dll ) {_msvcp90_dll  = _msvcp90_dll_6161; }
	if(_msvcr90_dll_6161  >_msvcr90_dll ) {_msvcr90_dll  = _msvcr90_dll_6161; }
	if(_mfc90_dll_6161    >_mfc90_dll   ) {_mfc90_dll    = _mfc90_dll_6161;   }
	if(_mfc90u_dll_6161   >_mfc90u_dll  ) {_mfc90u_dll   = _mfc90u_dll_6161;  }
	if(_mfcm90_dll_6161   >_mfcm90_dll  ) {_mfcm90_dll   = _mfcm90_dll_6161;  }
	if(_mfcm90u_dll_6161  >_mfcm90u_dll ) {_mfcm90u_dll  = _mfcm90u_dll_6161; }
	if(_mfc90chs_dll_6161 >_mfc90chs_dll) {_mfc90chs_dll = _mfc90chs_dll_6161;}
	if(_mfc90cht_dll_6161 >_mfc90cht_dll) {_mfc90cht_dll = _mfc90cht_dll_6161;}
	if(_mfc90deu_dll_6161 >_mfc90deu_dll) {_mfc90deu_dll = _mfc90deu_dll_6161;}
	if(_mfc90enu_dll_6161 >_mfc90enu_dll) {_mfc90enu_dll = _mfc90enu_dll_6161;}
	if(_mfc90esn_dll_6161 >_mfc90esn_dll) {_mfc90esn_dll = _mfc90esn_dll_6161;}
	if(_mfc90esp_dll_6161 >_mfc90esp_dll) {_mfc90esp_dll = _mfc90esp_dll_6161;}
	if(_mfc90fra_dll_6161 >_mfc90fra_dll) {_mfc90fra_dll = _mfc90fra_dll_6161;}
	if(_mfc90ita_dll_6161 >_mfc90ita_dll) {_mfc90ita_dll = _mfc90ita_dll_6161;}
	if(_mfc90jpn_dll_6161 >_mfc90jpn_dll) {_mfc90jpn_dll = _mfc90jpn_dll_6161;}
	if(_mfc90kor_dll_6161 >_mfc90kor_dll) {_mfc90kor_dll = _mfc90kor_dll_6161;}
	if(_vcomp90_dll_6161  >_vcomp90_dll ) {_vcomp90_dll  = _vcomp90_dll_6161; }
/*
	printf("Microsoft Visual C++ 2008 DEBUG:\n");
	printf("%s\t%s\n","atl90.dll",_atl90_dll.format().c_str());
	printf("%s\t%s\n","msvcm90.dll",_msvcm90_dll.format().c_str());
	printf("%s\t%s\n","msvcp90.dll",_msvcp90_dll.format().c_str());
	printf("%s\t%s\n","msvcr90.dll",_msvcr90_dll.format().c_str());
	printf("%s\t%s\n","mfc90.dll",_mfc90_dll.format().c_str());
	printf("%s\t%s\n","mfc90u.dll",_mfc90u_dll.format().c_str());
	printf("%s\t%s\n","mfcm90.dll",_mfcm90_dll.format().c_str());
	printf("%s\t%s\n","mfcm90u.dll",_mfcm90u_dll.format().c_str());
	printf("%s\t%s\n","mfc90chs.dll",_mfc90chs_dll.format().c_str());
	printf("%s\t%s\n","mfc90cht.dll",_mfc90cht_dll.format().c_str());
	printf("%s\t%s\n","mfc90deu.dll",_mfc90deu_dll.format().c_str());
	printf("%s\t%s\n","mfc90enu.dll",_mfc90enu_dll.format().c_str());
	printf("%s\t%s\n","mfc90esn.dll",_mfc90esn_dll.format().c_str());
	printf("%s\t%s\n","mfc90esp.dll",_mfc90esp_dll.format().c_str());
	printf("%s\t%s\n","mfc90fra.dll",_mfc90fra_dll.format().c_str());
	printf("%s\t%s\n","mfc90ita.dll",_mfc90ita_dll.format().c_str());
	printf("%s\t%s\n","mfc90jpn.dll",_mfc90jpn_dll.format().c_str());
	printf("%s\t%s\n","mfc90kor.dll",_mfc90kor_dll.format().c_str());
	printf("%s\t%s\n","vcomp90.dll",_vcomp90_dll.format().c_str());
	printf("\n");
*/
	fver _atl100_dll    = getFileVer(System32+L"\\atl100.dll",&status);
	fver _msdia100_dll  = getFileVer(SharedVC+L"\\msdia100.dll",&status);
	//fver _msvcm100_dll  = getFileVer(System32+L"\\msvcm100.dll",&status);
	fver _msvcp100_dll  = getFileVer(System32+L"\\msvcp100.dll",&status);
	fver _msvcr100_dll  = getFileVer(System32+L"\\msvcr100.dll",&status);
	fver _mfc100_dll    = getFileVer(System32+L"\\mfc100.dll",&status);
	fver _mfc100u_dll   = getFileVer(System32+L"\\mfc100u.dll",&status);
	fver _mfcm100_dll   = getFileVer(System32+L"\\mfcm100.dll",&status);
	fver _mfcm100u_dll  = getFileVer(System32+L"\\mfcm100u.dll",&status);
	fver _mfc100chs_dll = getFileVer(System32+L"\\mfc100chs.dll",&status);
	fver _mfc100cht_dll = getFileVer(System32+L"\\mfc100cht.dll",&status);
	fver _mfc100deu_dll = getFileVer(System32+L"\\mfc100deu.dll",&status);
	fver _mfc100enu_dll = getFileVer(System32+L"\\mfc100enu.dll",&status);
	fver _mfc100esn_dll = getFileVer(System32+L"\\mfc100esn.dll",&status);
	//fver _mfc100esp_dll = getFileVer(System32+L"\\mfc100esp.dll",&status);
	fver _mfc100fra_dll = getFileVer(System32+L"\\mfc100fra.dll",&status);
	fver _mfc100ita_dll = getFileVer(System32+L"\\mfc100ita.dll",&status);
	fver _mfc100jpn_dll = getFileVer(System32+L"\\mfc100jpn.dll",&status);
	fver _mfc100kor_dll = getFileVer(System32+L"\\mfc100kor.dll",&status);
	fver _mfc100rus_dll = getFileVer(System32+L"\\mfc100rus.dll",&status);
	fver _vcomp100_dll  = getFileVer(System32+L"\\vcomp100.dll",&status);

	bool vs2010sp1 = (  _atl100_dll   >= fver(10,0,40219,1)
	                 || _msdia100_dll >= fver(10,0,40219,1)
	               //|| _msvcm100_dll >= fver(10,0,40219,1)
	                 || _msvcp100_dll >= fver(10,0,40219,1)
	                 || _msvcr100_dll >= fver(10,0,40219,1)
	                 || _mfc100_dll   >= fver(10,0,40219,1)
	                 || _mfc100u_dll  >= fver(10,0,40219,1)
	                 || _mfcm100_dll  >= fver(10,0,40219,1)
	                 || _mfcm100u_dll >= fver(10,0,40219,1)
	                 || _mfc100chs_dll>= fver(10,0,40219,1)
	                 || _mfc100cht_dll>= fver(10,0,40219,1)
	                 || _mfc100deu_dll>= fver(10,0,40219,1)
	                 || _mfc100enu_dll>= fver(10,0,40219,1)
	                 || _mfc100esn_dll>= fver(10,0,40219,1)
	               //|| _mfc100esp_dll>= fver(10,0,40219,1)
	                 || _mfc100fra_dll>= fver(10,0,40219,1)
	                 || _mfc100ita_dll>= fver(10,0,40219,1)
	                 || _mfc100jpn_dll>= fver(10,0,40219,1)
	                 || _mfc100kor_dll>= fver(10,0,40219,1)
	                 || _mfc100rus_dll>= fver(10,0,40219,1)
	                 || _vcomp100_dll >= fver(10,0,40219,1));
/*
	printf("Microsoft Visual C++ 2010 %sDEBUG:\n",vs2010sp1?"SP1 ":"");
	printf("%s\t%s\n","atl100.dll",_atl100_dll.format().c_str());
	printf("%s\t%s\n","msdia100.dll",_msdia100_dll.format().c_str());
	//printf("%s\t%s\n","msvcm100.dll",_msvcm100_dll.format().c_str());
	printf("%s\t%s\n","msvcp100.dll",_msvcp100_dll.format().c_str());
	printf("%s\t%s\n","msvcr100.dll",_msvcr100_dll.format().c_str());
	printf("%s\t%s\n","mfc100.dll",_mfc100_dll.format().c_str());
	printf("%s\t%s\n","mfc100u.dll",_mfc100u_dll.format().c_str());
	printf("%s\t%s\n","mfcm100.dll",_mfcm100_dll.format().c_str());
	printf("%s\t%s\n","mfcm100u.dll",_mfcm100u_dll.format().c_str());
	printf("%s\t%s\n","mfc100chs.dll",_mfc100chs_dll.format().c_str());
	printf("%s\t%s\n","mfc100cht.dll",_mfc100cht_dll.format().c_str());
	printf("%s\t%s\n","mfc100deu.dll",_mfc100deu_dll.format().c_str());
	printf("%s\t%s\n","mfc100enu.dll",_mfc100enu_dll.format().c_str());
	printf("%s\t%s\n","mfc100esn.dll",_mfc100esn_dll.format().c_str());
	//printf("%s\t%s\n","mfc100esp.dll",_mfc100esp_dll.format().c_str());
	printf("%s\t%s\n","mfc100fra.dll",_mfc100fra_dll.format().c_str());
	printf("%s\t%s\n","mfc100ita.dll",_mfc100ita_dll.format().c_str());
	printf("%s\t%s\n","mfc100jpn.dll",_mfc100jpn_dll.format().c_str());
	printf("%s\t%s\n","mfc100kor.dll",_mfc100kor_dll.format().c_str());
	printf("%s\t%s\n","mfc100rus.dll",_mfc100rus_dll.format().c_str());
	printf("%s\t%s\n","vcomp100.dll",_vcomp100_dll.format().c_str());
	printf("\n");
*/
	std::wstring vs2012reg = regQueryValue(L"SOFTWARE\\Classes\\Installer\\Dependencies\\Microsoft.VS.VC_RuntimeMinimumVSU_x86,v11",L"Version",&status);
	fver _vs2012reg     = fver(vs2012reg.c_str());
	fver _atl110_dll    = getFileVer(System32+L"\\atl110.dll",&status);
	fver _msvcp110_dll  = getFileVer(System32+L"\\msvcp110.dll",&status);
	fver _msvcr110_dll  = getFileVer(System32+L"\\msvcr110.dll",&status);
	fver _mfc110_dll    = getFileVer(System32+L"\\mfc110.dll",&status);
	fver _mfc110u_dll   = getFileVer(System32+L"\\mfc110u.dll",&status);
	fver _mfcm110_dll   = getFileVer(System32+L"\\mfcm110.dll",&status);
	fver _mfcm110u_dll  = getFileVer(System32+L"\\mfcm110u.dll",&status);
	fver _mfc110chs_dll = getFileVer(System32+L"\\mfc110chs.dll",&status);
	fver _mfc110cht_dll = getFileVer(System32+L"\\mfc110cht.dll",&status);
	fver _mfc110deu_dll = getFileVer(System32+L"\\mfc110deu.dll",&status);
	fver _mfc110enu_dll = getFileVer(System32+L"\\mfc110enu.dll",&status);
	fver _mfc110esn_dll = getFileVer(System32+L"\\mfc110esn.dll",&status);
	fver _mfc110fra_dll = getFileVer(System32+L"\\mfc110fra.dll",&status);
	fver _mfc110ita_dll = getFileVer(System32+L"\\mfc110ita.dll",&status);
	fver _mfc110jpn_dll = getFileVer(System32+L"\\mfc110jpn.dll",&status);
	fver _mfc110kor_dll = getFileVer(System32+L"\\mfc110kor.dll",&status);
	fver _mfc110rus_dll = getFileVer(System32+L"\\mfc110rus.dll",&status);
	fver _vcomp110_dll  = getFileVer(System32+L"\\vcomp110.dll",&status);
/*
	printf("Microsoft Visual C++ 2012 DEBUG:\n");
	printf("Registry\t%s\n",_vs2012reg.format().c_str());
	printf("%s\t%s\n","atl110.dll",_atl110_dll.format().c_str());
	printf("%s\t%s\n","msvcp110.dll",_msvcp110_dll.format().c_str());
	printf("%s\t%s\n","msvcr110.dll",_msvcr110_dll.format().c_str());
	printf("%s\t%s\n","mfc110.dll",_mfc110_dll.format().c_str());
	printf("%s\t%s\n","mfc110u.dll",_mfc110u_dll.format().c_str());
	printf("%s\t%s\n","mfcm110.dll",_mfcm110_dll.format().c_str());
	printf("%s\t%s\n","mfcm110u.dll",_mfcm110u_dll.format().c_str());
	printf("%s\t%s\n","mfc110chs.dll",_mfc110chs_dll.format().c_str());
	printf("%s\t%s\n","mfc110cht.dll",_mfc110cht_dll.format().c_str());
	printf("%s\t%s\n","mfc110deu.dll",_mfc110deu_dll.format().c_str());
	printf("%s\t%s\n","mfc110enu.dll",_mfc110enu_dll.format().c_str());
	printf("%s\t%s\n","mfc110esn.dll",_mfc110esn_dll.format().c_str());
	printf("%s\t%s\n","mfc110fra.dll",_mfc110fra_dll.format().c_str());
	printf("%s\t%s\n","mfc110ita.dll",_mfc110ita_dll.format().c_str());
	printf("%s\t%s\n","mfc110jpn.dll",_mfc110jpn_dll.format().c_str());
	printf("%s\t%s\n","mfc110kor.dll",_mfc110kor_dll.format().c_str());
	printf("%s\t%s\n","mfc110rus.dll",_mfc110rus_dll.format().c_str());
	printf("%s\t%s\n","vcomp110.dll",_vcomp110_dll.format().c_str());
	printf("\n");
*/
	std::wstring vs2013reg1 = regQueryValue(L"SOFTWARE\\Classes\\Installer\\Dependencies\\Microsoft.VS.VC_RuntimeMinimumVSU_x86,v12",L"Version",&status);
	std::wstring vs2013reg2 = regQueryValue(L"SOFTWARE\\Classes\\Installer\\Dependencies\\{f65db027-aff3-4070-886a-0d87064aabb1}",L"Version",&status);
	fver _vs2013reg1 = fver(vs2013reg1.c_str());
	fver _vs2013reg2 = fver(vs2013reg2.c_str());
	fver _vs2013reg  = _vs2013reg1;
	if(_vs2013reg2 > _vs2013reg) { _vs2013reg = _vs2013reg2; }
	fver _mfc120_dll    = getFileVer(System32+L"\\mfc120.dll",&status);
	fver _mfc120chs_dll = getFileVer(System32+L"\\mfc120chs.dll",&status);
	fver _mfc120cht_dll = getFileVer(System32+L"\\mfc120cht.dll",&status);
	fver _mfc120deu_dll = getFileVer(System32+L"\\mfc120deu.dll",&status);
	fver _mfc120enu_dll = getFileVer(System32+L"\\mfc120enu.dll",&status);
	fver _mfc120esn_dll = getFileVer(System32+L"\\mfc120esn.dll",&status);
	fver _mfc120fra_dll = getFileVer(System32+L"\\mfc120fra.dll",&status);
	fver _mfc120ita_dll = getFileVer(System32+L"\\mfc120ita.dll",&status);
	fver _mfc120jpn_dll = getFileVer(System32+L"\\mfc120jpn.dll",&status);
	fver _mfc120kor_dll = getFileVer(System32+L"\\mfc120kor.dll",&status);
	fver _mfc120rus_dll = getFileVer(System32+L"\\mfc120rus.dll",&status);
	fver _mfc120u_dll   = getFileVer(System32+L"\\mfc120u.dll",&status);
	fver _mfcm120_dll   = getFileVer(System32+L"\\mfcm120.dll",&status);
	fver _mfcm120u_dll  = getFileVer(System32+L"\\mfcm120u.dll",&status);
	fver _msvcp120_dll  = getFileVer(System32+L"\\msvcp120.dll",&status);
	fver _msvcr120_dll  = getFileVer(System32+L"\\msvcr120.dll",&status);
	fver _vcomp120_dll  = getFileVer(System32+L"\\vcomp120.dll",&status);
/*
	printf("Microsoft Visual C++ 2013 DEBUG:\n");
	printf("Registry\t%s\n",_vs2013reg.format().c_str());
	printf("%s\t%s\n","mfc120.dll",_mfc120_dll.format().c_str());
	printf("%s\t%s\n","mfc120chs.dll",_mfc120chs_dll.format().c_str());
	printf("%s\t%s\n","mfc120cht.dll",_mfc120cht_dll.format().c_str());
	printf("%s\t%s\n","mfc120deu.dll",_mfc120deu_dll.format().c_str());
	printf("%s\t%s\n","mfc120enu.dll",_mfc120enu_dll.format().c_str());
	printf("%s\t%s\n","mfc120esn.dll",_mfc120esn_dll.format().c_str());
	printf("%s\t%s\n","mfc120fra.dll",_mfc120fra_dll.format().c_str());
	printf("%s\t%s\n","mfc120ita.dll",_mfc120ita_dll.format().c_str());
	printf("%s\t%s\n","mfc120jpn.dll",_mfc120jpn_dll.format().c_str());
	printf("%s\t%s\n","mfc120kor.dll",_mfc120kor_dll.format().c_str());
	printf("%s\t%s\n","mfc120rus.dll",_mfc120rus_dll.format().c_str());
	printf("%s\t%s\n","mfc120u.dll",_mfc120u_dll.format().c_str());
	printf("%s\t%s\n","mfcm120.dll",_mfcm120_dll.format().c_str());
	printf("%s\t%s\n","mfcm120u.dll",_mfcm120u_dll.format().c_str());
	printf("%s\t%s\n","msvcp120.dll",_msvcp120_dll.format().c_str());
	printf("%s\t%s\n","msvcr120.dll",_msvcr120_dll.format().c_str());
	printf("%s\t%s\n","vcomp120.dll",_vcomp120_dll.format().c_str());
	printf("\n");
*/
	fver _concrt140_dll    = getFileVer(System32+L"\\concrt140.dll",&status);
	fver _mfc140_dll = getFileVer(System32+L"\\mfc140.dll",&status);
	fver _mfc140chs_dll = getFileVer(System32+L"\\mfc140chs.dll",&status);
	fver _mfc140cht_dll = getFileVer(System32+L"\\mfc140cht.dll",&status);
	fver _mfc140deu_dll = getFileVer(System32+L"\\mfc140deu.dll",&status);
	fver _mfc140enu_dll = getFileVer(System32+L"\\mfc140enu.dll",&status);
	fver _mfc140esn_dll = getFileVer(System32+L"\\mfc140esn.dll",&status);
	fver _mfc140fra_dll = getFileVer(System32+L"\\mfc140fra.dll",&status);
	fver _mfc140ita_dll = getFileVer(System32+L"\\mfc140ita.dll",&status);
	fver _mfc140jpn_dll = getFileVer(System32+L"\\mfc140jpn.dll",&status);
	fver _mfc140kor_dll = getFileVer(System32+L"\\mfc140kor.dll",&status);
	fver _mfc140rus_dll = getFileVer(System32+L"\\mfc140rus.dll",&status);
	fver _mfc140u_dll = getFileVer(System32+L"\\mfc140u.dll",&status);
	fver _mfcm140_dll = getFileVer(System32+L"\\mfcm140.dll",&status);
	fver _mfcm140u_dll = getFileVer(System32+L"\\mfcm140u.dll",&status);
	fver _msvcp140_dll = getFileVer(System32+L"\\msvcp140.dll",&status);
	fver _ucrtbase_dll = getFileVer(System32+L"\\ucrtbase.dll",&status);
	fver _vcamp140_dll = getFileVer(System32+L"\\vcamp140.dll",&status);
	fver _vccorlib140_dll = getFileVer(System32+L"\\vccorlib140.dll",&status);
	fver _vcomp140_dll = getFileVer(System32+L"\\vcomp140.dll",&status);
	fver _vcruntime140_dll = getFileVer(System32+L"\\vcruntime140.dll",&status);
/*
	printf("Microsoft Visual C++ 2015 DEBUG:\n");
	printf("%s\t%s\n","concrt140.dll",_concrt140_dll.format().c_str());
	printf("%s\t%s\n","mfc140.dll",_mfc140_dll.format().c_str());
	printf("%s\t%s\n","mfc140chs.dll",_mfc140chs_dll.format().c_str());
	printf("%s\t%s\n","mfc140cht.dll",_mfc140cht_dll.format().c_str());
	printf("%s\t%s\n","mfc140deu.dll",_mfc140deu_dll.format().c_str());
	printf("%s\t%s\n","mfc140enu.dll",_mfc140enu_dll.format().c_str());
	printf("%s\t%s\n","mfc140esn.dll",_mfc140esn_dll.format().c_str());
	printf("%s\t%s\n","mfc140fra.dll",_mfc140fra_dll.format().c_str());
	printf("%s\t%s\n","mfc140ita.dll",_mfc140ita_dll.format().c_str());
	printf("%s\t%s\n","mfc140jpn.dll",_mfc140jpn_dll.format().c_str());
	printf("%s\t%s\n","mfc140kor.dll",_mfc140kor_dll.format().c_str());
	printf("%s\t%s\n","mfc140rus.dll",_mfc140rus_dll.format().c_str());
	printf("%s\t%s\n","mfc140u.dll",_mfc140u_dll.format().c_str());
	printf("%s\t%s\n","mfcm140.dll",_mfcm140_dll.format().c_str());
	printf("%s\t%s\n","mfcm140u.dll",_mfcm140u_dll.format().c_str());
	printf("%s\t%s\n","msvcp140.dll",_msvcp140_dll.format().c_str());
	printf("%s\t%s\n","ucrtbase.dll",_ucrtbase_dll.format().c_str());
	printf("%s\t%s\n","vcamp140.dll",_vcamp140_dll.format().c_str());
	printf("%s\t%s\n","vccorlib140.dll",_vccorlib140_dll.format().c_str());
	printf("%s\t%s\n","vcomp140.dll",_vcomp140_dll.format().c_str());
	printf("%s\t%s\n","vcruntime140.dll",_vcruntime140_dll.format().c_str());
	printf("\n");
*/
	// Windows XP SP2 updates;
	if( sp>=2 && (sku & XP_ALL) && (  _spcustom_dll_ref <fver(6,1,22,4)
		              ||    _spmsg_dll_ref    <fver(6,1,22,4)
					  ||    _spuninst_exe_ref <fver(6,1,22,4)
					  ||    _spupdsvc_exe_ref <fver(6,1,22,4)
					  ||    _update_exe_ref   <fver(6,1,22,4)
					  ||    _updspapi_dll_ref <fver(6,1,22,4) )) {
		NN("Update for Windows XP (KB898461)");
		XX(p+"WindowsXP-KB898461-x86-ENU.exe"+a1);
	}
	if((sku & XP_ALL) && (  _cdm_dll      <fver(7,0,6000,374)
		              ||    _wuapi_dll    <fver(7,0,6000,374)
					  ||    _wuauclt_exe  <fver(7,0,6000,374)
					  ||    _wuaucpl_cpl  <fver(7,0,6000,374)
					  ||    _wuaueng_dll  <fver(7,0,6000,374)
					  //||    _wuauserv_dll <fver(7,0,6000,374)
					  ||    _wucltui_dll  <fver(7,0,6000,374)
					  ||    _wups2_dll    <fver(7,0,6000,374)
					  ||    _wups_dll     <fver(7,0,6000,374)
					  ||    _wuweb_dll    <fver(7,0,6000,374) )) {
		NN("Windows Update Agent 3.0");
		XX(p+"WindowsUpdateAgent30-x86.exe"+a5);
	}
	if( sp==2 && (sku & XP_ALL) && _srvsvc_dll>zero && _srvsvc_dll<fver(5,1,2600,2577)) {
		NN("Security Update for Windows XP (KB888302)");
		XX(p+"windowsxp-kb888302-x86-enu_5c4ef905021d66aa78d9f5f112e5d05c40b1a909.exe"+a6);
	}
	/*if( sp==2 && (sku & XP_ALL) && _hhctrl_ocx>zero && _hhctrl_ocx<fver(5,2,3790,2847)) {
		// KB928843 is replaced with revised patch KB935448
		NN("Security Update for Windows XP (KB928843)");
		XX(p+"windowsxp-kb928843-x86-enu_80eb8779856aefa25bceab8926940fbdebabdc23.exe"+a1);
	}*/
	if( sp==2 && (sku & XP_ALL) && _hhctrl_ocx>zero && (
				_hhctrl_ocx<fver(5,2,3790,2847)
				|| !regTestKey(L"SOFTWARE\\Microsoft\\Updates\\Windows XP\\SP3\\KB935448")) ) {
		NN("Update for Windows XP (KB935448)");
		XX(p+"windowsxp-kb935448-x86-enu_f7a4f05c7979ea17ea0300ba131e0655c66f8eab.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _snmp_exe>zero && _snmp_exe<fver(5,1,2600,3038)) {
		NN("Security Update for Windows XP (KB926247)");
		XX(p+"windowsxp-kb926247-x86-enu_f465277dda6008156bb6f81bc77c72253a91d303.exe"+a1);
	}
	if( sp>=2 && (sku & XP_ALL) && (
		           (sp==2 && _wordpad_exe>zero && _wordpad_exe<fver(5,1,2600,3355))
				|| (sp==2 && _mswrd8_wpc >zero && _mswrd8_wpc <fver(10,0,803,10))
				|| (sp==3 && _wordpad_exe>zero && _wordpad_exe<fver(5,1,2600,5584))
				|| (sp==3 && _mswrd8_wpc >zero && _mswrd8_wpc <fver(2007,10,31,10)) )) {
		NN("Security Update for Windows XP (KB923561)");
		XX(p+"WindowsXP-KB923561-x86-ENU.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ( (_mrxsmb_sys>zero && _mrxsmb_sys<fver(5,1,2600,2902))
					  ||  (_rdbss_sys>zero && _rdbss_sys<fver(5,1,2600,2902)) )) {
		NN("Security Update for Windows XP (KB914389)");
		XX(p+"windowsxp-kb914389-x86-enu_8c44336e9e4f287891ac384bee0219e9c2224523.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _mrxsmb_sys>zero && _mrxsmb_sys<fver(5,1,2600,3675)) {
	//if( sp==3 && (sku & XP_ALL) && _mrxsmb_sys>zero && _mrxsmb_sys<fver(5,1,2600,5944)) {
		NN("Security Update for Windows XP (KB980232)");
		XX(p+"windowsxp-kb980232-x86-enu_d137985d21958a9f3f277189287b9a71eee02421.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ( (_dnsapi_dll>zero && _dnsapi_dll<fver(5,1,2600,2938))
					  ||  (_rasadhlp_dll>zero && _rasadhlp_dll<fver(5,1,2600,2938)) )) {
		NN("Security Update for Windows XP (KB920683)");
		XX(p+"windowsxp-kb920683-x86-enu_ef1482c5b88557e56563dace9b7549ebf6d7f9c7.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _oleaut32_dll>zero && _oleaut32_dll<fver(5,1,2600,3266)) {
		NN("Security Update for Windows XP (KB943055)");
		XX(p+"windowsxp-kb943055-x86-enu_e160e1b140872892d2d0050449b1606cc85b5654.exe"+a1);
	}
	/*if( sp==2 && (sku & XP_ALL) && _gdi32_dll>zero && _gdi32_dll<fver(5,1,2600,3316)) {
		// KB948590 is replaced by KB956802 in SP2
		NN("Security Update for Windows XP (KB948590)");
		XX(p+"windowsxp-kb948590-x86-enu_f94e1dbe8e74d3da8f3979e1cae71360c0c11a8e.exe"+a1);
	}*/
	if( sp==2 && (sku & XP_ALL) && _gdi32_dll>zero && _gdi32_dll<fver(5,1,2600,3466)) {
	//if( sp==3 && (sku & XP_ALL) && _gdi32_dll>zero && _gdi32_dll<fver(5,1,2600,5698)) {
		NN("Security Update for Windows XP (KB956802)");
		XX(p+"windowsxp-kb956802-x86-enu_bf0a8bfe0d01487a999fb8f66b3719957acf1f3a.exe"+a1);
	}
	/*if( sp==2 && (sku & XP_ALL) && _msxml3_dll>zero && _msxml3_dll<fver(8,90,1101,0)) {
		// KB936021 is replaced by KB955069 on SP2
		NN("Security Update for Windows XP (KB936021)");
		XX(p+"windowsxp-kb936021-x86-enu_9952bfa53c32d996780807ca1c1ab1c9ca398237.exe"+a1);
	}*/
	if( sp==2 && (sku & XP_ALL) && _msxml3_dll>zero && _msxml3_dll<fver(8,100,1048,0)) { // also installable on SP3, same payload
		NN("Security Update for Windows XP (KB955069)");
		XX(p+"windowsxp-kb955069-x86-enu_fa864585a7d761ba0f940eff151672871d0e69f3.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ( (_msdtcprx_dll>zero && _msdtcprx_dll<fver(2001,12,4414,311))
					  ||  (_msdtctm_dll>zero && _msdtctm_dll<fver(2001,12,4414,311))
					  ||  (_msdtcuiu_dll>zero && _msdtcuiu_dll<fver(2001,12,4414,311))
					  ||  (_mtxclu_dll>zero && _mtxclu_dll<fver(2001,12,4414,311))
					  ||  (_mtxoci_dll>zero && _mtxoci_dll<fver(2001,12,4414,311))
					  ||  (_xolehlp_dll>zero && _xolehlp_dll<fver(2001,12,4414,311)) )) {
		NN("Security Update for Windows XP (KB913580)");
		XX(p+"windowsxp-kb913580-x86-enu_f57aa2fdaf623d8b0231fc928c00ad8498d37c76.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _netapi32_dll>zero && _netapi32_dll<fver(5,1,2600,3462)) {
	//if( sp==3 && (sku & XP_ALL) && _netapi32_dll>zero && _netapi32_dll<fver(5,1,2600,5694)) {
		NN("Security Update for Windows XP (KB958644)");
		XX(p+"windowsxp-kb958644-x86-enu_5c135a8dae5721849430afe27af255f83e64f62b.exe"+a1);
	}
	/*if( sp==2 && (sku & XP_ALL) && ( (_mqac_sys>zero && _mqac_sys<fver(5,1,0,1109))
					  ||  (_mqad_dll    >zero && _mqad_dll    <fver(5,1,0,1109))
					  ||  (_mqdscli_dll >zero && _mqdscli_dll <fver(5,1,0,1109))
					  ||  (_mqise_dll   >zero && _mqise_dll   <fver(5,1,0,1109))
					  ||  (_mqqm_dll    >zero && _mqqm_dll    <fver(5,1,0,1109))
					  ||  (_mqrt_dll    >zero && _mqrt_dll    <fver(5,1,0,1109))
					  ||  (_mqsec_dll   >zero && _mqsec_dll   <fver(5,1,0,1109))
					  ||  (_mqupgrd_dll >zero && _mqupgrd_dll <fver(5,1,0,1109))
					  ||  (_mqutil_dll  >zero && _mqutil_dll  <fver(5,1,0,1109)) )) {
		// KB937894 is replaced by KB971032 on SP2
		NN("Security Update for Windows XP (KB937894)");
		XX(p+"windowsxp-kb937894-x86-enu_4546c1f99c7d0d557b5c454c961798eab01e3444.exe"+a1);
	}*/
	if( sp==2 && (sku & XP_ALL) && ( (_mqac_sys>zero && _mqac_sys<fver(5,1,0,1111))
					  ||  (_mqad_dll    >zero && _mqad_dll    <fver(5,1,0,1111))
					  ||  (_mqbkup_exe  >zero && _mqbkup_exe  <fver(5,1,0,1111))
					  ||  (_mqdscli_dll >zero && _mqdscli_dll <fver(5,1,0,1111))
					  ||  (_mqise_dll   >zero && _mqise_dll   <fver(5,1,0,1111))
					  ||  (_mqoa_dll    >zero && _mqoa_dll    <fver(5,1,0,1111))
					  ||  (_mqqm_dll    >zero && _mqqm_dll    <fver(5,1,0,1111))
					  ||  (_mqrt_dll    >zero && _mqrt_dll    <fver(5,1,0,1111))
					  ||  (_mqrtdep_dll >zero && _mqrtdep_dll <fver(5,1,0,1111))
					  ||  (_mqsec_dll   >zero && _mqsec_dll   <fver(5,1,0,1111))
					  ||  (_mqsnap_dll  >zero && _mqsnap_dll  <fver(5,1,0,1111))
					  ||  (_mqsvc_exe   >zero && _mqsvc_exe   <fver(5,1,0,1111))
					  ||  (_mqtgsvc_exe >zero && _mqtgsvc_exe <fver(5,1,0,1111))
					  ||  (_mqtrig_dll  >zero && _mqtrig_dll  <fver(5,1,0,1111))
					  ||  (_mqupgrd_dll >zero && _mqupgrd_dll <fver(5,1,0,1111))
					  ||  (_mqutil_dll  >zero && _mqutil_dll  <fver(5,1,0,1111))
					  ||  (_msmqocm_dll >zero && _msmqocm_dll <fver(5,1,0,1111)) )) {
		NN("Security Update for Windows XP (KB971032)");
		XX(p+"windowsxp-kb971032-x86-enu_23f7ac835ed26257c74a7b1ef5caa6198182cf6c.exe"+a1);
	}
	/*if( sp==2 && (sku & XP_ALL) && _quartz_dll>zero && _quartz_dll<fver(6,5,2600,3243)) {
		// KB941568 is replaced by KB975562 on SP2
		NN("Security Update for Windows XP (KB941568)");
		XX(p+"windowsxp-kb941568-x86-enu_e62eb6119afa388291cf71da243fb265afe31e60.exe"+a1);
	}*/
	if( sp==2 && (sku & XP_ALL) && _quartz_dll>zero && _quartz_dll<fver(6,5,2600,3665)) {
	//if( sp==3 && (sku & XP_ALL) && _quartz_dll>zero && _quartz_dll<fver(6,5,2600,5933)) {
		NN("Security Update for Windows XP (KB975562)");
		XX(p+"windowsxp-kb975562-x86-enu_6826ab2c0f8ce1e717e693036ae4662c7fcc7277.exe"+a1);
	}
	if(    (sp==2 && (sku & XP_ALL) && _mswebdvd_dll>zero && _mswebdvd_dll<fver(6,5,2600,3610))
		|| (sp==3 && (sku & XP_ALL) && _mswebdvd_dll>zero && _mswebdvd_dll<fver(6,5,2600,5857))) {
		NN("Security Update for Windows XP (KB973815)");
		XX(p+"WindowsXP-KB973815-x86-ENU.exe"+a1);
	}
	if(    (sp==2 && (sku & XP_PLUS_FLP) && _winhttp_dll>zero && _winhttp_dll<fver(5,1,2600,3494))
		|| (sp==3 && (sku & XP_PLUS_FLP) && _winhttp_dll>zero && _winhttp_dll<fver(5,1,2600,5727))) {
		NN("Security Update for Windows XP (KB960803)");
		XX(p+"WindowsXP-KB960803-x86-ENU.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _winhttp_dll>zero && _winhttp_dll<fver(5,1,2600,3619)) {
	//if( sp==3 && (sku & XP_ALL) && _winhttp_dll>zero && _winhttp_dll<fver(5,1,2600,5868)) {
		NN("Update for Windows XP (KB971737)");
		XX(p+"windowsxp-kb971737-x86-enu_e2aa4340aaf68e97a5dbdd278edb9182c4383ed7.exe"+a1);
	}
	if(    (sp==2 && (sku & XP_ALL) && _asycfilt_dll>zero && _asycfilt_dll<fver(5,1,2600,3680))
		|| (sp==3 && (sku & XP_ALL) && _asycfilt_dll>zero && _asycfilt_dll<fver(5,1,2600,5949))) {
		NN("Security Update for Windows XP (KB979482)");
		XX(p+"WindowsXP-KB979482-x86-ENU.exe"+a1);
	}
	if(    (sp==2 && (sku & XP_ALL) && _cabview_dll>zero && _cabview_dll<fver(6,0,2900,3663))
		|| (sp==3 && (sku & XP_ALL) && _cabview_dll>zero && _cabview_dll<fver(6,0,2900,5927))) {
		NN("Security Update for Windows XP (KB979309)");
		XX(p+"WindowsXP-KB979309-x86-ENU.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ( (_dao360_dll>zero && _dao360_dll<fver(3,60,9512,0))
					  ||  (_msexch40_dll >zero && _msexch40_dll <fver(4,0,9502,0))
					  ||  (_msexcl40_dll >zero && _msexcl40_dll <fver(4,0,9502,0))
					  ||  (_msjet40_dll  >zero && _msjet40_dll  <fver(4,0,9511,0))
					  ||  (_msjetol1_dll >zero && _msjetol1_dll <fver(4,0,9502,0))
					  ||  (_msjint40_dll >zero && _msjint40_dll <fver(4,0,9502,0))
					  ||  (_msjter40_dll >zero && _msjter40_dll <fver(4,0,9502,0))
					  ||  (_msjtes40_dll >zero && _msjtes40_dll <fver(4,0,9502,0))
					  ||  (_msltus40_dll >zero && _msltus40_dll <fver(4,0,9502,0))
					  ||  (_mspbde40_dll >zero && _mspbde40_dll <fver(4,0,9502,0))
					  ||  (_msrd2x40_dll >zero && _msrd2x40_dll <fver(4,0,9502,0))
					  ||  (_msrd3x40_dll >zero && _msrd3x40_dll <fver(4,0,9502,0))
					  ||  (_msrepl40_dll >zero && _msrepl40_dll <fver(4,0,9502,0))
					  ||  (_mstext40_dll >zero && _mstext40_dll <fver(4,0,9502,0))
					  ||  (_mswdat10_dll >zero && _mswdat10_dll <fver(4,0,9502,0))
					  ||  (_mswstr10_dll >zero && _mswstr10_dll <fver(4,0,9502,0))
					  ||  (_msxbde40_dll >zero && _msxbde40_dll <fver(4,0,9502,0)) )) {
		NN("Security Update for Windows XP (KB950749)");
		XX(p+"windowsxp-kb950749-x86-enu_52c183782f97ebcc7c92e0d71e0525a540898507.exe"+a1);
	}
	/*if( sp==2 && (sku & XP_ALL) && _rpcrt4_dll>zero && _rpcrt4_dll<fver(5,1,2600,3173)) {
		// KB933729 is replaced by KB970238 on SP2
		NN("Security Update for Windows XP (KB933729)");
		XX(p+"windowsxp-kb933729-x86-enu_5b4ed1a7e5586b93a367156607ace191dc4bf53a.exe"+a1);
	}*/
	if( sp==2 && (sku & XP_ALL) && _rpcrt4_dll>zero && _rpcrt4_dll<fver(5,1,2600,3555)) {
	//if( sp==3 && (sku & XP_ALL) && _rpcrt4_dll>zero && _rpcrt4_dll<fver(5,1,2600,5795)) {
		NN("Security Update for Windows XP (KB970238)");
		XX(p+"windowsxp-kb970238-x86-enu_82bd58dd365d93afa746a4beaf2a1ad5b8d5181a.exe"+a1);
	}
	/*if( sp==2 && (sku & XP_ALL) && _tcpip_sys>zero && _tcpip_sys<fver(5,1,2600,3244)) {
		// KB941644 is replaced by KB951748 on SP2
		NN("Security Update for Windows XP (KB941644)");
		XX(p+"windowsxp-kb941644-x86-enu_69bd9e4e22c97592dca087165e4fef304230f7a5.exe"+a1);
	}*/
	if( sp==2 && (sku & XP_ALL) && ( (_6to4svc_dll>zero && _6to4svc_dll<fver(5,1,2600,2975))
					  ||  (_afd_sys     >zero && _afd_sys     <fver(5,1,2600,3394))
					  ||  (_dnsapi_dll  >zero && _dnsapi_dll  <fver(5,1,2600,3394))
					  ||  (_mswsock_dll >zero && _mswsock_dll <fver(5,1,2600,3394))
					  ||  (_tcpip6_sys  >zero && _tcpip6_sys  <fver(5,1,2600,3394))
					  ||  (_tcpip_sys   >zero && _tcpip_sys   <fver(5,1,2600,3394)) )) {
	//if( sp==3 && (sku & XP_ALL) && ( (_afd_sys     >zero && _afd_sys     <fver(5,1,2600,5625))
	//				  ||  (_dnsapi_dll  >zero && _dnsapi_dll  <fver(5,1,2600,5625))
	//				  ||  (_mswsock_dll >zero && _mswsock_dll <fver(5,1,2600,5625))
	//				  ||  (_tcpip6_sys  >zero && _tcpip6_sys  <fver(5,1,2600,5625))
	//				  ||  (_tcpip_sys   >zero && _tcpip_sys   <fver(5,1,2600,5625)) )) {
		NN("Security Update for Windows XP (KB951748)");
		XX(p+"windowsxp-kb951748-x86-enu_6b7598e550ef967b83e9c088f8cd3409632f5d23.exe"+a1);
	}
	if( (sp==2 && (sku & XP_ALL) && ( (_avifil32_dll>zero && _avifil32_dll<fver(5,1,2600,3649))
		              ||  (_iyuv_32_dll  >zero && _iyuv_32_dll  <fver(5,1,2600,3649))
					  ||  (_msrle32_dll  >zero && _msrle32_dll  <fver(5,1,2600,3649))
					  ||  (_msvidc32_dll >zero && _msvidc32_dll <fver(5,1,2600,3649))
					  ||  (_tsbyuv_dll   >zero && _tsbyuv_dll   <fver(5,1,2600,3649)) ))
	|| (sp==3 && (sku & XP_ALL) && ( (_avifil32_dll>zero && _avifil32_dll<fver(5,1,2600,5908))
		              ||  (_iyuv_32_dll  >zero && _iyuv_32_dll  <fver(5,1,2600,5908))
					  ||  (_msrle32_dll  >zero && _msrle32_dll  <fver(5,1,2600,5908))
					  ||  (_msvidc32_dll >zero && _msvidc32_dll <fver(5,1,2600,5908))
					  ||  (_tsbyuv_dll   >zero && _tsbyuv_dll   <fver(5,1,2600,5908)) ))) {
		NN("Security Update for Windows XP (KB977914)");
		XX(p+"WindowsXP-KB977914-x86-ENU.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ( (_dnsapi_dll>zero && _dnsapi_dll<fver(5,1,2600,3316))
					  ||  (_dnsrslvr_dll>zero && _dnsrslvr_dll<fver(5,1,2600,3316)) )) {
		NN("Security Update for Windows XP (KB945553)");
		XX(p+"windowsxp-kb945553-x86-enu_94dfff615b4f4fb15d3db5ef261a964d65f60492.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ( (_agentdp2_dll>zero && _agentdp2_dll<fver(2,0,0,3424))
					  ||  (_agentdpv_dll>zero && _agentdpv_dll<fver(2,0,0,3424))
					  ||  (_agentsvr_exe>zero && _agentsvr_exe<fver(2,0,0,3424)) )) {
		NN("Security Update for Windows XP (KB920213)");
		XX(p+"windowsxp-kb920213-x86-enu_02cb394147b09e8926b4f8334feeff4b8fa4b33b.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _agentdpv_dll>zero && _agentdpv_dll<fver(2,0,0,3425)) {
		NN("Security Update for Windows XP (KB932168)");
		XX(p+"windowsxp-kb932168-x86-enu_646e8c2d535461396b8b53ce8fe04410327925a2.exe"+a1);
	}
	if(   (sp==2 && (sku & XP_ALL) && _oakley_dll>zero && _oakley_dll<fver(5,1,2600,3632))
		||(sp==3 && (sku & XP_ALL) && _oakley_dll>zero && _oakley_dll<fver(5,1,2600,5886))) {
		NN("Security Update for Windows XP (KB974392)");
		XX(p+"WindowsXP-KB974392-x86-ENU.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _sxs_dll>zero && _sxs_dll<fver(5,1,2600,3019)) {
		NN("Security Update for Windows XP (KB926255)");
		XX(p+"windowsxp-kb926255-x86-enu_1737b8dde544dbbc79dddd6f123291b781313c04.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ( (_msftedit_dll>zero && _msftedit_dll<fver(5,41,15,1514))
					  ||  (_riched20_dll>zero && _riched20_dll<fver(5,30,23,1228)) )) {
		NN("Security Update for Windows XP (KB918118)");
		XX(p+"windowsxp-kb918118-x86-enu_8c38e06d795c410b8a072506af40ad81d64f1d6d.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ((_mswrd6_wpc>zero && _mswrd6_wpc<fver(10,0,803,2))
	       || !regTestKey(L"SOFTWARE\\Microsoft\\Updates\\Windows XP\\SP3\\KB885836") )) {
		NN("Security Update for Windows XP (KB885836)");
		XX(p+"windowsxp-kb885836-x86-enu_f87074f42947ee275445bdd34dda472871ed3b41.exe"+a6);
	}
	if( sp>=2 && (sku & XP_ALL) && ( (_html32_cnv>zero && _html32_cnv   <fver(2003,1100,8165,0))
		              ||  (_msconv97_dll         >zero && _msconv97_dll <fver(2003,1100,8165,0))
		              ||  (_mswrd6_wpc           >zero && _mswrd6_wpc   <fver(2009,10,31,10))
					  ||  (_mswrd8_wpc           >zero && _mswrd8_wpc   <fver(2009,10,31,10))  
					  ||  (_mswrd632_wpc         >zero && _mswrd632_wpc <fver(2009,10,31,10))
					  ||  (_mswrd832_cnv         >zero && _mswrd832_cnv <fver(2003,1100,8313,0))
					  ||  (_write32_wpc          >zero && _write32_wpc  <fver(2009,10,31,10))
					  ||  (_write_wpc            >zero && _write_wpc    <fver(2009,10,31,10)) )) {
		NN("Security Update for Windows XP (KB973904)");
		XX(p+"WindowsXP-KB973904-x86-ENU.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ( (_comctl32_dll>zero && _comctl32_dll<fver(5,82,2900,2982))
					  ||  ( _comctl32_dll_2982<fver(6,0,2900,2982)) )) {
		NN("Security Update for Windows XP (KB923191)");
		XX(p+"windowsxp-kb923191-x86-enu_9d2cfed124f1f50804c20a6e8a881f84c266745f.exe"+a1);
	}
	/*if( sp==2 && (sku & XP_ALL) && _win32k_sys>zero && _win32k_sys<fver(5,1,2600,3335)) {
		// KB941693 is replaced by KB979559 on SP2
		NN("Security Update for Windows XP (KB941693)");
		XX(p+"windowsxp-kb941693-x86-enu_c2a97173c92d11672036e8a5e0a062f79460c6f1.exe"+a1);
	}*/
	if( sp==2 && (sku & XP_ALL) && _win32k_sys>zero && _win32k_sys<fver(5,1,2600,3706)) {
	//if( sp==3 && (sku & XP_ALL) && _win32k_sys>zero && _win32k_sys<fver(5,1,2600,5976)) {
		NN("Security Update for Windows XP (KB979559)");
		XX(p+"windowsxp-kb979559-x86-enu_4d87c29e02a9f212a012f3e035f91e68c8fc60be.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _wintrust_dll>zero && _wintrust_dll<fver(5,131,2600,3661)) {
	//if( sp==3 && (sku & XP_ALL) && _wintrust_dll>zero && _wintrust_dll<fver(5,131,2600,5922)) {
		NN("Security Update for Windows XP (KB978601)");
		XX(p+"windowsxp-kb978601-x86-enu_49ae6ac4f6e0a609124f2ce6f80fcef249273c02.exe"+a1);
	}
	/*if( sp==2 && (sku & XP_ALL) && _rmcast_sys>zero && _rmcast_sys<fver(5,1,2600,2951)) {
		// KB919007 is replaced by KB950762 on SP2
		NN("Security Update for Windows XP (KB919007)");
		XX(p+"windowsxp-kb919007-x86-enu_dc2307d635a64c87bbf1f216442104ef4b4ada0b.exe"+a1);
	}*/
	if(   (sp==2 && (sku & XP_ALL) && _rmcast_sys>zero && _rmcast_sys<fver(5,1,2600,3369))
		||(sp==3 && (sku & XP_ALL) && _rmcast_sys>zero && _rmcast_sys<fver(5,1,2600,5598))) {
		NN("Security Update for Windows XP (KB950762)");
		XX(p+"windowsxp-kb950762-x86-enu_bfa04c9d2e62b4695d1bb8953486788c8a8c11e4.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ( (_dhcpcsvc_dll>zero && _dhcpcsvc_dll<fver(5,1,2600,2912))
					  ||  (_dnsapi_dll>zero && _dnsapi_dll<fver(5,1,2600,2912))
					  ||  (_iphlpapi_dll>zero && _iphlpapi_dll<fver(5,1,2600,2912)) )) {
		NN("Security Update for Windows XP (KB914388)");
		XX(p+"windowsxp-kb914388-x86-enu_21992f2ad7f7fd8ab28e854ce364ebc4f8baf810.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _winsrv_dll>zero && _winsrv_dll<fver(5,1,2600,3103)) {
		NN("Security Update for Windows XP (KB930178)");
		XX(p+"windowsxp-kb930178-x86-enu_5e02462613354e7679cdd79b0ee52083efd9aa30.exe"+a1);
	}
	if( sp>=2 && (sku & XP_ALL) && _atl_dll>zero && _atl_dll<fver(3,5,2284,2)) {
		NN("Security Update for Windows XP (KB973507)");
		XX(p+"WindowsXP-KB973507-x86-ENU.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _oledlg_dll>zero && _oledlg_dll<fver(5,1,2600,3016)) {
		NN("Security Update for Windows XP (KB926436)");
		XX(p+"windowsxp-kb926436-x86-enu_98f46d49f189f01c14a7d5360d794da20edae885.exe"+a1);
	}
	if(    (sp==2 && (sku & XP_ALL) && ( (_msyuv_dll >zero && _msyuv_dll <fver(5,3,2600,3649))
					  ||                 (_quartz_dll>zero && _quartz_dll<fver(6,5,2600,3649))))
		|| (sp==3 && (sku & XP_ALL) && ( (_msyuv_dll >zero && _msyuv_dll <fver(5,3,2600,5908))
					  ||                 (_quartz_dll>zero && _quartz_dll<fver(6,5,2600,5908))))) {
		NN("Security Update for Windows XP (KB975560)");
		XX(p+"WindowsXP-KB975560-x86-ENU.exe"+a1);
	}
	if(    (sp==2 && (sku & XP_ALL) && _msasn1_dll>zero && _msasn1_dll<fver(5,1,2600,3624))
		|| (sp==3 && (sku & XP_ALL) && _msasn1_dll>zero && _msasn1_dll<fver(5,1,2600,5875))) {
		NN("Security Update for Windows XP (KB974571)");
		XX(p+"WindowsXP-KB974571-x86-ENU.exe"+a1);
	}
	if((sp==2 && (sku & XP_ALL) && ( (_msdtclog_dll >zero && _msdtclog_dll <fver(2001,12,4414,320))
		              ||             (_msdtcprx_dll >zero && _msdtcprx_dll <fver(2001,12,4414,320))
					  ||             (_msdtctm_dll  >zero && _msdtctm_dll  <fver(2001,12,4414,320))
					  ||             (_msdtcuiu_dll >zero && _msdtcuiu_dll <fver(2001,12,4414,320))
					  ||             (_mtxclu_dll   >zero && _mtxclu_dll   <fver(2001,12,4414,320))
					  ||             (_mtxoci_dll   >zero && _mtxoci_dll   <fver(2001,12,4414,320))))
	|| (sp==3 && (sku & XP_ALL) && ( (_msdtclog_dll >zero && _msdtclog_dll <fver(2001,12,4414,706))
		              ||             (_msdtcprx_dll >zero && _msdtcprx_dll <fver(2001,12,4414,706))
					  ||             (_msdtctm_dll  >zero && _msdtctm_dll  <fver(2001,12,4414,706))
					  ||             (_msdtcuiu_dll >zero && _msdtcuiu_dll <fver(2001,12,4414,706))
					  ||             (_mtxclu_dll   >zero && _mtxclu_dll   <fver(2001,12,4414,706))
					  ||             (_mtxoci_dll   >zero && _mtxoci_dll   <fver(2001,12,4414,706))))) {
		NN("Security Update for Windows XP (KB952004)");
		XX(p+"WindowsXP-KB952004-x86-ENU.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _hlink_dll>zero && _hlink_dll<fver(5,2,3790,2748)) {
		NN("Security Update for Windows XP (KB920670)");
		XX(p+"windowsxp-kb920670-x86-enu_a71ac163b276057101fe92739c06c6e6d143ccf8.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ( (_gdi32_dll>zero && _gdi32_dll<fver(5,1,2600,3099))
					  ||  (_mf3216_dll>zero && _mf3216_dll<fver(5,1,2600,3099))
					  ||  (_user32_dll>zero && _user32_dll<fver(5,1,2600,3099))
					  ||  (_win32k_sys>zero && _win32k_sys<fver(5,1,2600,3099)) )) {
		NN("Security Update for Windows XP (KB925902)");
		XX(p+"windowsxp-kb925902-x86-enu_a7b984afc7cec6e351d4f1d176db202492ac0e0f.exe"+a1);
	}
	/*if( sp>=2 && (sku & XP_ALL) && _dhtmled_ocx>zero && _dhtmled_ocx<fver(6,1,0,9232)) {
		// KB891781 is replaced by KB973869 on SP2
		NN("Security Update for Windows XP (KB891781)");
		XX(p+"windowsxp-kb891781-x86-enu_32b11076df0189adeb0f36ce3bf7baa01cff1c29.exe"+a6);
	}*/
	if( sp>=2 && (sku & XP_ALL) && _dhtmled_ocx>zero && _dhtmled_ocx<fver(6,1,0,9247)) {
		NN("Security Update for Windows XP (KB973869)");
		XX(p+"windowsxp-kb973869-x86-enu_c0679cb2d4c07f35f39ee5d8d7b973c5e4582d34.exe"+a1);
	}
	if( sp>=2 && (sku & XP_ALL) && _triedit_dll>zero && _triedit_dll<fver(6,1,0,9246)) {
		NN("Security Update for Windows XP (KB956844)");
		XX(p+"windowsxp-kb956844-x86-enu_926002701569eabda17630d8f9cb45d8ced0ab71.exe"+a1);
	}
	if((sp==2 && (sku & XP_ALL) && _localspl_dll>zero && _localspl_dll<fver(5,1,2600,3569))
	 ||(sp==3 && (sku & XP_ALL) && _localspl_dll>zero && _localspl_dll<fver(5,1,2600,5809))) {
		NN("Security Update for Windows XP (KB961501)");
		XX(p+"windowsxp-kb961501-x86-enu_a5360324a1fe219a9aa9f0e513edffe7305176e4.exe"+a1);
	}
	/*if( sp==2 && (sku & XP_ALL) && _lsasrv_dll>zero && _lsasrv_dll<fver(5,1,2600,3249)) {
		// KB943485 is replaced by KB956572 in SP2
		NN("Security Update for Windows XP (KB943485)");
		XX(p+"windowsxp-kb943485-x86-enu_cb8bf0aa18299dda9578a70411ade45cbd3ae619.exe"+a1);
	}*/
	if((sp==2 && (sku & XP_ALL) && (
		                  (_advapi32_dll >zero && _advapi32_dll <fver(5,1,2600,3520))
					  ||  (_colbact_dll  >zero && _colbact_dll  <fver(2001,12,4414,308))
		              ||  (_fastprox_dll >zero && _fastprox_dll <fver(5,1,2600,3520))
					  ||  (_lsasrv_dll   >zero && _lsasrv_dll   <fver(5,1,2600,3520))
					  ||  (_ntdll_dll    >zero && _ntdll_dll    <fver(5,1,2600,3520))
					  ||  (_ntkrnlmp_exe >zero && _ntkrnlmp_exe <fver(5,1,2600,3520))
					  ||  (_ntkrnlpa_exe >zero && _ntkrnlpa_exe <fver(5,1,2600,3520))
					  ||  (_ntkrpamp_exe >zero && _ntkrpamp_exe <fver(5,1,2600,3520))
					  ||  (_ntoskrnl_exe >zero && _ntoskrnl_exe <fver(5,1,2600,3520))
					  ||  (_pdh_dll      >zero && _pdh_dll      <fver(5,1,2600,3536))
					  ||  (_rpcss_dll    >zero && _rpcss_dll    <fver(5,1,2600,3520))
					  ||  (_sc_exe       >zero && _sc_exe       <fver(5,1,2600,3520))
					  ||  (_services_exe >zero && _services_exe <fver(5,1,2600,3520))
					  ||  (_wmiprvsd_dll >zero && _wmiprvsd_dll <fver(5,1,2600,3520))
					  ||  (_wmiprvse_exe >zero && _wmiprvse_exe <fver(5,1,2600,3520))))
	||(sp==3 && (sku & XP_ALL) && (
		                  (_advapi32_dll >zero && _advapi32_dll <fver(5,1,2600,5755))
		              ||  (_fastprox_dll >zero && _fastprox_dll <fver(5,1,2600,5755))
					  ||  (_lsasrv_dll   >zero && _lsasrv_dll   <fver(5,1,2600,5755))
					  ||  (_ntdll_dll    >zero && _ntdll_dll    <fver(5,1,2600,5755))
					  ||  (_ntkrnlmp_exe >zero && _ntkrnlmp_exe <fver(5,1,2600,5755))
					  ||  (_ntkrnlpa_exe >zero && _ntkrnlpa_exe <fver(5,1,2600,5755))
					  ||  (_ntkrpamp_exe >zero && _ntkrpamp_exe <fver(5,1,2600,5755))
					  ||  (_ntoskrnl_exe >zero && _ntoskrnl_exe <fver(5,1,2600,5755))
					  ||  (_pdh_dll      >zero && _pdh_dll      <fver(5,1,2600,5773))
					  ||  (_rpcss_dll    >zero && _rpcss_dll    <fver(5,1,2600,5755))
					  ||  (_sc_exe       >zero && _sc_exe       <fver(5,1,2600,5755))
					  ||  (_services_exe >zero && _services_exe <fver(5,1,2600,5755))
					  ||  (_wmiprvsd_dll >zero && _wmiprvsd_dll <fver(5,1,2600,5755))
					  ||  (_wmiprvse_exe >zero && _wmiprvse_exe <fver(5,1,2600,5755))))) {
		NN("Security Update for Windows XP (KB956572)");
		XX(p+"WindowsXP-KB956572-x86-ENU.exe"+a1);
	}
	if( sp>=2 && (sku & XP_ALL) && _strmdll_dll>zero && _strmdll_dll<fver(4,1,0,3938)) {
		NN("Security Update for Windows XP (KB974112)");
		XX(p+"WindowsXP-KB974112-x86-ENU.exe"+a1);
	}
	if( sp>=2 && (sku & XP_ALL) && ( (_dxmasf_dll>=fver(6,4,9,0) && _dxmasf_dll<fver(6,4,9,1133))
					  ||  (_strmdll_dll>=fver(4,1,0,0) && _strmdll_dll<fver(4,1,0,3936)) )) {
		NN("Security Update for Windows Media Player 6.4 (KB925398)");
		XX(p+"windowsmedia6-kb925398-v2-x86-enu_c8b7d9a38ecbd2bd8925345d69c53697a285b917.exe"+a1);
	}
	/*if( sp==2 && (sku & XP_ALL) && ( (_fontsub_dll>zero && _fontsub_dll<fver(5,1,2600,2777))
					  ||  (_t2embed_dll>zero && _t2embed_dll<fver(5,1,2600,2777)) )) {
		// KB908519 is replaced by KB972270 on SP2
		NN("Security Update for Windows XP (KB908519)");
		XX(p+"windowsxp-kb908519-x86-enu_ea7ea742f9a3632f1090eab8c66b3fe7735c084f.exe"+a1);
	}*/
	if((sp==2 && (sku & XP_ALL) && ( (_fontsub_dll>zero && _fontsub_dll<fver(5,1,2600,3634))
					  ||  (_t2embed_dll>zero && _t2embed_dll<fver(5,1,2600,3634))))
	 ||(sp==3 && (sku & XP_ALL) && ( (_fontsub_dll>zero && _fontsub_dll<fver(5,1,2600,5888))
					  ||  (_t2embed_dll>zero && _t2embed_dll<fver(5,1,2600,5888))))) {
		NN("Security Update for Windows XP (KB972270)");
		XX(p+"WindowsXP-KB972270-x86-ENU.exe"+a1);
	}
	/*if( sp==2 && (sku & XP_ALL) && _schannel_dll>zero && _schannel_dll<fver(5,1,2600,3126)) {
		// KB935840 is replaced by KB960225 on SP2
		NN("Security Update for Windows XP (KB935840)");
		XX(p+"windowsxp-kb935840-x86-enu_24d246dfa0195732d275b8f5206f9338f85a2113.exe"+a1);
	}*/
	if( sp==2 && (sku & XP_ALL) && _schannel_dll>zero && _schannel_dll<fver(5,1,2600,3487)) {
	//if( sp==3 && (sku & XP_ALL) && _schannel_dll>zero && _schannel_dll<fver(5,1,2600,5721)) {
		NN("Security Update for Windows XP (KB960225)");
		XX(p+"windowsxp-kb960225-x86-enu_bae2bc04b963c312a47f36bdea4a8236f7003d71.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _mrxdav_sys>zero && _mrxdav_sys<fver(5,1,2600,3276)) {
		NN("Security Update for Windows XP (KB946026)");
		XX(p+"windowsxp-kb946026-x86-enu_09eb2e3ee66c5e1be768a4fee49f9f47a0f0b978.exe"+a1);
	}
	if(sp==2 && (sku & XP_ALL) && ( (_6to4svc_dll>zero && _6to4svc_dll<fver(5,1,2600,2975))
					  || (_tcpip6_sys>zero && _tcpip6_sys<fver(5,1,2600,2975))
					  || !regTestKey(L"SOFTWARE\\Microsoft\\Updates\\Windows XP\\SP3\\KB922819"))) {
		// KB922819 is replaced by KB978338 on SP2, but is a prerequisite to KB920342
		NN("Security Update for Windows XP (KB922819)");
		XX(p+"windowsxp-kb922819-x86-enu_e4dceecdd4a72e5ad91cc78fe5f4572f91ee5db0.exe"+a1);
	}
	if((sp==2 && (sku & XP_ALL) && ( (_6to4svc_dll>zero && _6to4svc_dll<fver(5,1,2600,3667))
					  ||  (_tcpip6_sys>zero && _tcpip6_sys<fver(5,1,2600,3667))))
	 ||(sp==3 && (sku & XP_ALL) && ( (_6to4svc_dll>zero && _6to4svc_dll<fver(5,1,2600,5935))
					  ||  (_tcpip6_sys>zero && _tcpip6_sys<fver(5,1,2600,5935))))) {
		NN("Security Update for Windows XP (KB978338)");
		XX(p+"WindowsXP-KB978338-x86-ENU.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ( 
		                  (_p2p_dll      >zero && _p2p_dll      <fver(5,1,2600,3013))
					  ||  (_p2pgasvc_dll >zero && _p2pgasvc_dll <fver(5,1,2600,3013))
					  ||  (_p2pgraph_dll >zero && _p2pgraph_dll <fver(5,1,2600,3013))
					  ||  (_p2pnetsh_dll >zero && _p2pnetsh_dll <fver(5,1,2600,3013))
					  ||  (_p2psvc_dll   >zero && _p2psvc_dll   <fver(5,1,2600,3013))
					  ||  (_pnrpnsp_dll  >zero && _pnrpnsp_dll  <fver(5,1,2600,3013)) )) {
		// Update KB920342 requires KB922819 as a prerequisite
		NN("Update for Windows XP (KB920342)");
		XX(p+"windowsxp-kb920342-x86-enu_6ffee4344d80833300220f461eff4d9c2d9d7d7f.exe"+a1);
	}
	if((sp==2 && (sku & XP_ALL) && _wkssvc_dll>zero && _wkssvc_dll<fver(5,1,2600,3584))
	 ||(sp==3 && (sku & XP_ALL) && _wkssvc_dll>zero && _wkssvc_dll<fver(5,1,2600,5826))) {
		NN("Security Update for Windows XP (KB971657)");
		XX(p+"WindowsXP-KB971657-x86-ENU.exe"+a1);
	}
	if((sp==2 && (sku & XP_ALL) && _shlwapi_dll>zero && _shlwapi_dll<fver(6,0,2900,3653))
	 ||(sp==3 && (sku & XP_ALL) && _shlwapi_dll>zero && _shlwapi_dll<fver(6,0,2900,5912))) {
		NN("Security Update for Windows XP (KB975713)");
		XX(p+"WindowsXP-KB975713-x86-ENU.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _csrsrv_dll>zero && _csrsrv_dll<fver(5,1,2600,3657)) {
	//if( sp==3 && (sku & XP_ALL) && _csrsrv_dll>zero && _csrsrv_dll<fver(5,1,2600,5915)) {
		NN("Security Update for Windows XP (KB978037)");
		XX(p+"windowsxp-kb978037-x86-enu_0165498351f085f2cdcf407a23e2a85d08f7212e.exe"+a1);
	}
	if((sp==2 && (sku & XP_ALL) && _es_dll>zero && _es_dll<fver(2001,12,4414,320))
	 ||(sp==3 && (sku & XP_ALL) && _es_dll>zero && _es_dll<fver(2001,12,4414,706))) {
		NN("Security Update for Windows XP (KB950974)");
		XX(p+"WindowsXP-KB950974-x86-ENU.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _shdocvw_dll>zero && _shdocvw_dll<fver(6,0,2900,2987)) {
		NN("Security Update for Windows XP (KB924496)");
		XX(p+"windowsxp-kb924496-x86-enu_f1e2421551a739eae947590735fb3f4abec82c22.exe"+a1);
	}
	if((sp==2 && (sku & XP_ALL) && _helpsvc_exe>zero && _helpsvc_exe<fver(5,1,2600,3720))
	 ||(sp==3 && (sku & XP_ALL) && _helpsvc_exe>zero && _helpsvc_exe<fver(5,1,2600,5997))) {
		NN("Security Update for Windows XP (KB2229593)");
		XX(p+"WindowsXP-KB2229593-x86-ENU.exe"+a1);
	}
	if(sp==2 && (sku & XP_ALL) && ( (_ciodm_dll>zero && _ciodm_dll<fver(5,1,2600,2935))
					  ||  (_query_dll>zero && _query_dll<fver(5,1,2600,2935)) )) {
		NN("Security Update for Windows XP (KB920685)");
		XX(p+"windowsxp-kb920685-x86-enu_be0e9cea96e2ad48394aebe90d48edcc36ac38d5.exe"+a1);
	}
	if((sp==2 && (sku & XP_ALL) && _query_dll>zero && _query_dll<fver(5,1,2600,3602))
	 ||(sp==3 && (sku & XP_ALL) && _query_dll>zero && _query_dll<fver(5,1,2600,5847))) {
		NN("Security Update for Windows XP (KB969059)");
		XX(p+"WindowsXP-KB969059-x86-ENU.exe"+a1);
	}
	if((sp==2 && (sku & XP_ALL) && ( (_raschap_dll>zero && _raschap_dll<fver(5,1,2600,3632))
					  ||  (_rastls_dll>zero && _rastls_dll<fver(5,1,2600,3632))))
	 ||(sp==3 && (sku & XP_ALL) && ( (_raschap_dll>zero && _raschap_dll<fver(5,1,2600,5886))
					  ||  (_rastls_dll>zero && _rastls_dll<fver(5,1,2600,5886))))) {
		NN("Security Update for Windows XP (KB974318)");
		XX(p+"WindowsXP-KB974318-x86-ENU.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _upnphost_dll>zero && _upnphost_dll<fver(5,1,2600,3077)) {
		NN("Security Update for Windows XP (KB931261)");
		XX(p+"windowsxp-kb931261-x86-enu_76c76c0fbe6dd82bae54c22e353ea16f7bd4f95e.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ( (_lsasrv_dll>zero && _lsasrv_dll<fver(5,1,2600,2976))
					  ||  (_netapi32_dll>zero && _netapi32_dll<fver(5,1,2600,2976))
					  ||  (_wkssvc_dll>zero && _wkssvc_dll<fver(5,1,2600,2976)) )) {
		NN("Security Update for Windows XP (KB924270)");
		XX(p+"windowsxp-kb924270-x86-enu_3b1af30dc7a2f51f60a415eaf2cc01f9bf779dab.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ( (_mfc40u_dll>zero && _mfc40u_dll<fver(4,1,0,6141))
					  ||  (_mfc42u_dll>zero && _mfc42u_dll<fver(6,2,8071,0))
					  ||  (_atl_dll_9792     <fver(3,0,9238,0))
					  ||  (_mfc42_dll_9792   <fver(6,0,8665,0))
					  ||  (_mfc42u_dll_9792  <fver(6,0,9792,0))
					  ||  (_msvcp60_dll_9792 <fver(6,0,8972,0)) )) {
		NN("Security Update for Windows XP (KB924667)");
		XX(p+"windowsxp-kb924667-x86-enu_9016dbabca407c3278219baba256111e131330a3.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _msadco_dll>zero && _msadco_dll<fver(2,81,1124,0)) {
		NN("Security Update for Windows XP (KB911562)");
		XX(p+"windowsxp-kb911562-x86-enu_7d16ad9701607a354e0ca2602a3fef485c8d9929.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _rasmans_dll>zero && _rasmans_dll<fver(5,1,2600,2936)) {
		NN("Security Update for Windows XP (KB911280)");
		XX(p+"windowsxp-kb911280-v2-x86-enu_3a49ae105416eb7b37dbbaccbedc9c20069ef1d9.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && (!regTestKey(L"SOFTWARE\\Microsoft\\Updates\\Windows XP\\SP4\\KB980195")
		                        || !kb980195_installed())) {
		NN("Cumulative Security Update for ActiveX Killbits for Windows XP (KB980195)");
		XX(p+"windowsxp-kb980195-x86-enu_5e9692a99ed5f78af0ce82f7aa5e87b1cd5bf958.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && (!regTestKey(L"SOFTWARE\\Microsoft\\Updates\\Windows XP\\SP4\\KB2900986")
		                        || !kb2900986_installed())) {
		NN("Cumulative Security Update for ActiveX Killbits for Windows XP (KB2900986)");
		XX(p+"WindowsXP-KB2900986-x86-ENU.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _gdiplus_dll_22319<fver(5,2,6001,22319)) {
		NN("Security Update for Windows XP (KB958869)");
		XX(p+"windowsxp-kb958869-x86-enu_ec59d655f78bbc751bb5d664530615ba53929548.exe"+a1);
	}
	/*if( sp==2 && (sku & XP_ALL) && _vgx_dll>zero && _vgx_dll<fver(6,0,2900,3051)) {
		// KB929969 is replaced by KB938127 on SP2
		NN("Security Update for Windows XP (KB929969)");
		XX(p+"windowsxp-kb929969-x86-enu_1237547f1cb90f54269bdb73c9985f263e85d48c.exe"+a1);
	}*/
	if( sp==2 && (sku & XP_ALL) && _vgx_dll>zero && _vgx_dll<fver(6,0,2900,3164)) {
		NN("Security Update for Windows XP Service Pack 2 (KB938127)");
		XX(p+"WindowsXP-KB938127-x86-ENU.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ( (_shell32_dll>zero && _shell32_dll<fver(6,0,2900,3051))
					  ||  (_shsvcs_dll>zero && _shsvcs_dll<fver(6,0,2900,3051)) )) {
		NN("Security Update for Windows XP (KB928255)");
		XX(p+"windowsxp-kb928255-x86-enu_d29dfbbe228e49f746a947eeb4880e980b76d53d.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _shell32_dll>zero && _shell32_dll<fver(6,0,2900,3241)) {
		NN("Security Update for Windows XP (KB943460)");
		XX(p+"windowsxp-kb943460-x86-enu_ab4feaa4871f4c2512cad616479668f19e779c53.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _shell32_dll>zero && _shell32_dll<fver(6,0,2900,3402)) {
	//if( sp==3 && (sku & XP_ALL) && _shell32_dll>zero && _shell32_dll<fver(6,0,2900,5622)) {
		NN("Update for Windows XP (KB967715)");
		XX(p+"windowsxp-kb967715-x86-enu_7d158e1f0dc6a7b5de75ca59bc89816017150ab4.exe"+a1);
	}
	/*if( sp==2 && (sku & XP_ALL) && ( 
		                  (_ntkrnlmp_exe>zero && _ntkrnlmp_exe<fver(5,1,2600,3093))
					  ||  (_ntkrnlpa_exe>zero && _ntkrnlpa_exe<fver(5,1,2600,3093))
					  ||  (_ntkrpamp_exe>zero && _ntkrpamp_exe<fver(5,1,2600,3093))
					  ||  (_ntoskrnl_exe>zero && _ntoskrnl_exe<fver(5,1,2600,3093)) )) {
		// KB931784 is replaced by KB979683 on SP2
		NN("Security Update for Windows XP (KB931784)");
		XX(p+"windowsxp-kb931784-x86-enu_70c95793c76ac8746a1ca9abd1815a8f0b73944f.exe"+a1);
	}*/
	if( sp==2 && (sku & XP_ALL) && ( 
		                  (_ntkrnlmp_exe>zero && _ntkrnlmp_exe<fver(5,1,2600,3670))
					  ||  (_ntkrnlpa_exe>zero && _ntkrnlpa_exe<fver(5,1,2600,3670))
					  ||  (_ntkrpamp_exe>zero && _ntkrpamp_exe<fver(5,1,2600,3670))
					  ||  (_ntoskrnl_exe>zero && _ntoskrnl_exe<fver(5,1,2600,3670)) )) {
//	if( sp==3 && (sku & XP_ALL) && (
//		                  (_ntkrnlmp_exe>zero && _ntkrnlmp_exe<fver(5,1,2600,5938))
//					  ||  (_ntkrnlpa_exe>zero && _ntkrnlpa_exe<fver(5,1,2600,5938))
//					  ||  (_ntkrpamp_exe>zero && _ntkrpamp_exe<fver(5,1,2600,5938))
//					  ||  (_ntoskrnl_exe>zero && _ntoskrnl_exe<fver(5,1,2600,5938)) )) {
		NN("Security Update for Windows XP (KB979683)");
		XX(p+"windowsxp-kb979683-x86-enu_8f9e90bf9b6ad58fd264cc1cf0f187f9a83223a5.exe"+a1);
	}
	/*if( sp==2 && (sku & XP_ALL) && _srv_sys>zero && _srv_sys<fver(5,1,2600,2974)) {
		// KB923414 is replaced by KB971468 on SP2
		NN("Security Update for Windows XP (KB923414)");
		XX(p+"windowsxp-kb923414-x86-enu_ed2b1047badbd832a971a76ca7ef4519d1a444f4.exe"+a1);
	}*/
	if( sp==2 && (sku & XP_ALL) && _srv_sys>zero && _srv_sys<fver(5,1,2600,3662)) {
	//if( sp==3 && (sku & XP_ALL) && _srv_sys>zero && _srv_sys<fver(5,1,2600,5923)) {
		NN("Security Update for Windows XP (KB971468)");
		XX(p+"windowsxp-kb971468-x86-enu_68d7899c8b8462219daf40f02c6fb9f362b1ee6b.exe"+a1);
	}
	/*if( sp==2 && (sku & XP_ALL) && _telnet_exe>zero && _telnet_exe<fver(5,1,2600,2674)) {
		// KB896428 is replaced by KB960859 on SP2
		NN("Security Update for Windows XP (KB896428)");
		XX(p+"windowsxp-kb896428-x86-enu_24f66bc1e3b8107ec580ba2c53148a69dbc606a0.exe"+a1);
	}*/
	if((sp==2 && (sku & XP_ALL) && ( (_telnet_exe>zero && _telnet_exe<fver(5,1,2600,3587))
					  ||  (_tlntsess_exe>zero && _tlntsess_exe<fver(5,1,2600,3587))))
	 ||(sp==3 && (sku & XP_ALL) && ( (_telnet_exe>zero && _telnet_exe<fver(5,1,2600,5829))
					  ||  (_tlntsess_exe>zero && _tlntsess_exe<fver(5,1,2600,5829))))) {
		NN("Security Update for Windows XP (KB960859)");
		XX(p+"WindowsXP-KB960859-x86-ENU.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _afd_sys>zero && _afd_sys<fver(5,1,2600,3427)) {
	//if( sp==3 && (sku & XP_ALL) && _afd_sys>zero && _afd_sys<fver(5,1,2600,5657)) {
		NN("Security Update for Windows XP (KB956803)");
		XX(p+"windowsxp-kb956803-x86-enu_d075d359a28ab8b058a35a2e7b466bd0bca8e9ef.exe"+a1);
	}
	if( sp>=2 && (sku & XP_ALL) && _msgsc_dll>=fver(4,7,0,0) && _msgsc_dll<fver(4,7,0,3002)) {
		NN("Security Update for Windows XP (KB946648)");
		XX(p+"WindowsXP-KB946648-x86-ENU.exe"+a1);
	}
	/*if( sp==2 && (sku & XP_ALL) && _kernel32_dll>zero && _kernel32_dll<fver(5,1,2600,3119)) {
		// KB935839 is replaced by KB959426 on SP2
		NN("Security Update for Windows XP (KB935839)");
		XX(p+"windowsxp-kb935839-x86-enu_891b3c6e312494ccf2dd0dab00dfcc4c49f2ca85.exe"+a1);
	}*/
	if((sp==2 && (sku & XP_ALL) && ( (_kernel32_dll>zero && _kernel32_dll<fver(5,1,2600,3541))
					  ||  (_secur32_dll>zero && _secur32_dll<fver(5,1,2600,3518))))
	 ||(sp==3 && (sku & XP_ALL) && ( (_kernel32_dll>zero && _kernel32_dll<fver(5,1,2600,5781))
					  ||  (_secur32_dll>zero && _secur32_dll<fver(5,1,2600,5753))))) {
		NN("Security Update for Windows XP (KB959426)");
		XX(p+"WindowsXP-KB959426-x86-ENU.exe"+a1);
	}
	if((sp==2 && (sku & XP_ALL) && _mscms_dll>zero && _mscms_dll<fver(5,1,2600,3396))
	 ||(sp==3 && (sku & XP_ALL) && _mscms_dll>zero && _mscms_dll<fver(5,1,2600,5627))) {
		NN("Security Update for Windows XP (KB952954)");
		XX(p+"WindowsXP-KB952954-x86-ENU.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _wiaservc_dll>zero && _wiaservc_dll<fver(5,1,2600,3051)) {
		NN("Security Update for Windows XP (KB927802)");
		XX(p+"windowsxp-kb927802-x86-enu_94703f4083a9d9d6633d9134d0d0a85bfc405f3a.exe"+a1);
	}
	if((sp==2 && (sku & XP_ALL) && _bthport_sys<fver(5,1,2600,3389))
	 ||(sp==3 && (sku & XP_ALL) && _bthport_sys<fver(5,1,2600,5620))) {
		NN("Security Update for Windows XP (KB951376)");
		XX(p+"WindowsXP-KB951376-v2-x86-ENU.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ( 
		                  (_msado15_dll>zero && _msado15_dll<fver(2,81,1128,0))
					  ||  (_msadomd_dll>zero && _msadomd_dll<fver(2,81,1128,0))
					  ||  (_msadox_dll >zero && _msadox_dll <fver(2,81,1128,0))
					  ||  (_msjro_dll  >zero && _msjro_dll  <fver(2,81,1128,0)) )) {
		NN("Security Update for Windows XP (KB927779)");
		XX(p+"windowsxp-kb927779-x86-enu_ec7986f2b0afe9cc7f53a48b6582169a77d9515e.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _atmfd_dll>zero && _atmfd_dll<fver(5,1,2,228)) {
		NN("Security Update for Windows XP (KB980218)");
		XX(p+"windowsxp-kb980218-x86-enu_8f11793773e54a3ff1f1b93e5ebfdd2592caba2f.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) /*&& _msctf_dll>zero*/ && _msctf_dll<fver(5,1,2600,3319)) {
		NN("Update for Windows XP (KB932823)");
		XX(p+"windowsxp-kb932823-v3-x86-enu_d0806094569c5bbce9f6e0313cd67558316d048a.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _tzchange_exe<fver(5,1,2600,5971)) {
		NN("Update for Windows XP (KB981793)");
		XX(p+"windowsxp-kb981793-x86-enu_e1363fb1d582b860c3e25b2bbfbc0b7f38a8b44b.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) /*&& _aclayers_dll>zero*/ && _aclayers_dll<fver(5,1,2600,3647)) {
	//if( sp==3 && (sku & XP_ALL) /*&& _aclayers_dll>zero*/ && _aclayers_dll<fver(5,1,2600,5906)) {
		NN("Update for Windows XP (KB955759)");
		XX(p+"windowsxp-kb955759-x86-enu_32f91e0ecc35b55f6c9e51ad360b2adee1f74049.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _msxml3_dll>zero && _msxml3_dll<fver(8,100,1050,0)) {
//	if( sp==3 && (sku & XP_ALL) && ( 
//		                  (_msxml3_dll >zero && _msxml3_dll <fver(8,100,1051,0))
//					  ||  (_msxml6_dll >zero && _msxml6_dll <fver(6,20,1103,0)) )) {
		NN("Update for Windows XP (KB973687)");
		XX(p+"windowsxp-kb973687-x86-enu_936b46b34291e12c6fe6b7cb19e93d52dda990d8.exe"+a1);
	}
	if( (sp==2 && (sku & XP_ALL) && _msadce_dll>zero && _msadce_dll<fver(2,81,1135,0))
	 || (sp==3 && (sku & XP_ALL) && _msadce_dll>zero && _msadce_dll<fver(2,81,3002,0)) ){
		NN("Update for Windows XP (KB952287)");
		XX(p+"windowsxp-kb952287-x86-enu_5bee5928c09f4ee44bc41cab109022cec820e5ff.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _update_sys>zero && _update_sys<fver(5,1,2600,3124)) {
		NN("Update for Windows XP (KB936357)");
		XX(p+"windowsxp-kb936357-x86-enu_50f76a151743f032f20d6a9f6e71ed0b73fe7781.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _explorer_exe>zero && _explorer_exe<fver(6,0,2900,3156)) {
		NN("Update for Windows XP (KB938828)");
		XX(p+"windowsxp-kb938828-x86-enu_ba3f0cbe4ba5736d4254732e41fe058697b76ebc.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _ntfs_sys>zero && _ntfs_sys<fver(5,1,2600,3081)) {
		NN("Update for Windows XP (KB930916)");
		XX(p+"windowsxp-kb930916-x86-enu_f077461cd094716eafd9cfe353d257a0f71a4af2.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ( 
		                  (_fltlib_dll >zero && _fltlib_dll <fver(5,1,2600,2978))
					  ||  (_fltMc_exe  >zero && _fltMc_exe  <fver(5,1,2600,2978))
					  ||  (_fltMgr_sys >zero && _fltMgr_sys <fver(5,1,2600,2978)) )) {
		NN("Update for Windows XP (KB922582)");
		XX(p+"windowsxp-kb922582-x86-enu_4d2c05da13ad8b38b98d290938bbb7de3fd6d985.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _http_sys>zero && _http_sys<fver(5,1,2600,2869)) {
		NN("Update for Windows XP (KB916595)");
		XX(p+"windowsxp-kb916595-x86-enu_d2e2d4f421b6578b739717a98f84aa8fface741f.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ( 
		                  (_kmixer_sys   >zero && _kmixer_sys   <fver(5,1,2600,2929))
					  ||  (_splitter_sys >zero && _splitter_sys <fver(5,1,2600,2929))
					  ||  (_wdmaud_sys   >zero && _wdmaud_sys   <fver(5,1,2600,2929)) )) {
		NN("Update for Windows XP (KB920872)");
		XX(p+"windowsxp-kb920872-x86-enu_6f7ac7666023971f687e372581b6d40cfa8677f0.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _aec_sys>zero && _aec_sys<fver(5,1,2601,2180)) {
		NN("Update for Windows XP (KB900485)");
		XX(p+"windowsxp-kb900485-v2-x86-enu_e30ec198dd9fbfa393940a630dfc641f49db727c.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ( 
		                  ( /*_custsat_dll   >zero &&*/ _custsat_dll   <fver(9,0,2600,2921))
					  ||  ( /*_xpnetdg_exe   >zero &&*/ _xpnetdiag_exe <fver(5,1,2600,3012)) )) {
		NN("Network Diagnostic Tool (KB914440)");
		XX(p+"windowsxp-kb914440-v12-x86-enu_74aec9f94a132216c271bf0aa3c5324979d12740.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ( 
		                  ( _migload_exe  <fver(5,1,2600,2665))
					  ||  ( _migwiz_exe   <fver(5,1,2600,2665))
					  ||  ( _migwiza_exe  <fver(5,1,2600,2665))
					  ||  ( _cobramsg_dll <fver(5,1,2600,2665))
					  ||  ( _guitrn_dll   <fver(5,1,2600,2665))
					  ||  ( _guitrna_dll  <fver(5,1,2600,2665))
					  ||  ( _iconlib_dll  <fver(5,1,2600,2665))
					  ||  ( _log_dll      <fver(5,1,2600,2665))
					  ||  ( _migism_dll   <fver(5,1,2600,2665))
					  ||  ( _migisma_dll  <fver(5,1,2600,2665))
					  ||  ( _script_dll   <fver(5,1,2600,2665))
					  ||  ( _scripta_dll  <fver(5,1,2600,2665))
					  ||  ( _sysmod_dll   <fver(5,1,2600,2665))
					  ||  ( _sysmoda_dll  <fver(5,1,2600,2665)) )) {
		NN("Update for Windows XP (KB896344)");
		XX(p+"windowsxp-kb896344-v2-x86-enu_3ab0b06345f79bbcd426f4f57834753210716441.exe"+a1);
	}
	if( sp>=2 && (sku & XP_ALL) && ( 
		                  ( _photometadatahandler_dll <fver(6,0,5840,16388))
					  ||  ( _windowscodecs_dll        <fver(6,0,5840,16388))
					  ||  ( _windowscodecsext_dll     <fver(6,0,5840,16388))
					  ||  ( _wmphoto_dll              <fver(6,0,5840,16388)) )) {
		if(    _photometadatahandler_dll <=fver(6,0,5840,16388)
			&& _windowscodecs_dll        <=fver(6,0,5840,16388)
			&& _windowscodecsext_dll     <=fver(6,0,5840,16388)
			&& _wmphoto_dll              <=fver(6,0,5840,16388)) {
			NN("Windows Imaging Component for Windows XP and Server 2003");
			XX(p+"wic_x86_enu.exe"+a1);
		} else {
		//                                    ....V....1....V....2....V....3....V....4....V....5
		notifications->push_back(std::string("Windows Imaging Component is missing, but")
			                               +"|cannot be installed because newer parts of"
							               +"|it are already present on the system.");
		}
	}
	if( sp==2 && (sku & XP_ALL) && (
		                  ( _authz_dll    >zero && _authz_dll    <fver(5,1,2600,2622))
					  ||  ( _ntkrnlmp_exe >zero && _ntkrnlmp_exe <fver(5,1,2600,2622))
					  ||  ( _ntkrnlpa_exe >zero && _ntkrnlpa_exe <fver(5,1,2600,2622))
					  ||  ( _ntkrpamp_exe >zero && _ntkrpamp_exe <fver(5,1,2600,2622))
					  ||  ( _ntoskrnl_exe >zero && _ntoskrnl_exe <fver(5,1,2600,2622))
					  ||  ( _user32_dll   >zero && _user32_dll   <fver(5,1,2600,2622))
					  ||  ( _win32k_sys   >zero && _win32k_sys   <fver(5,1,2600,2622))
					  ||  ( _winsrv_dll   >zero && _winsrv_dll   <fver(5,1,2600,2622)) )) {
		NN("Security Update for Windows XP (KB890859)");
		XX(p+"windowsxp-kb890859-x86-enu_813f47d987b772bacae20e7dec9b5f6f16079303.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && (
		                  ( /*_secdrv_sys    >zero*/ _secdrv_sys_exist
						   && _secdrv_sys    <fver(4,3,86,0)) )) {
		NN("Security Update for Windows XP (KB944653)");
		XX(p+"windowsxp-kb944653-x86-enu_e08db12cc3442f904f2a1d9a51c7c5662e15ef3b.exe"+a1);
	}
	if( sp>=2 && (sku & XP_ALL) && _flash_ocx>zero && _flash6_ocx<fver(6,0,88,0)) {
		NN("Security Update for Flash Player (KB923789)");
		XX(p+"WindowsXP-KB923789-x86-ENU.exe"+a2);
	}
	if( sp==2 && (sku & XP_ALL) && (
		                  ( _umpnpmgr_dll    >zero && _umpnpmgr_dll    <fver(5,1,2600,2744)) )) {
		NN("Security Update for Windows XP (KB905749)");
		XX(p+"windowsxp-kb905749-x86-enu_3f44b68f7e0a0e6332dd18997e134beab1027c73.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && (
		                  ( _shell32_dll    >zero && _shell32_dll    <fver(6,0,2900,2869))
					  ||  ( _verclsid_exe   >zero && _verclsid_exe   <fver(5,1,2600,2869)) )) {
		NN("Security Update for Windows XP (KB908531)");
		XX(p+"windowsxp-kb908531-v2-x86-enu_0f04352bbc21b3c173cc8dd8c9e63c082b34b676.exe"+a1);
	}
	if((sp==2 && (sku & XP_ALL) && _mspaint_exe>zero && _mspaint_exe<fver(5,1,2600,3660))
	 ||(sp==3 && (sku & XP_ALL) && _mspaint_exe>zero && _mspaint_exe<fver(5,1,2600,5918))) {
		NN("Security Update for Windows XP (KB978706)");
		XX(p+"WindowsXP-KB978706-x86-ENU.exe"+a1);
	}
	/*if(sp==2 && (sku & XP_ALL) && _inetcomm_dll>zero && _inetcomm_dll<fver(6,0,2900,3198)) {
		// KB941202 is replaced by KB978542 on SP2
		NN("Security Update for Outlook Express for Windows XP (KB941202)");
		XX(p+"windowsxp-kb941202-x86-enu_a82783a1dd4d11145d52b0fa0aa2f8dc172eefc3.exe"+a1);
	}*/
	if((sp==2 && (sku & XP_ALL) && ( (_inetcomm_dll>zero && _inetcomm_dll<fver(6,0,2900,3664))
					  ||  (_msoe_dll>zero && _msoe_dll<fver(6,0,2900,3664))))
	|| (sp==3 && (sku & XP_ALL) && ( (_inetcomm_dll>zero && _inetcomm_dll<fver(6,0,2900,5931))
					  ||  (_msoe_dll>zero && _msoe_dll<fver(6,0,2900,5931))))) {
		NN("Security Update for Windows XP (KB978542)");
		XX(p+"WindowsXP-KB978542-x86-ENU.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && (
		                  ( _linkinfo_dll >zero && _linkinfo_dll <fver(5,1,2600,2751))
					  ||  ( _shell32_dll  >zero && _shell32_dll  <fver(6,0,2900,2763))
					  ||  ( _shlwapi_dll  >zero && _shlwapi_dll  <fver(6,0,2900,2753))
					  ||  ( _winsrv_dll   >zero && _winsrv_dll   <fver(5,1,2600,2751)) )) {
		NN("Security Update for Windows XP (KB900725)");
		XX(p+"windowsxp-kb900725-x86-enu_21b409882b7f51a9d09c32bd698504fadb9fc433.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && (
		                  ( _icm32_dll >zero && _icm32_dll <fver(5,1,2600,2709))
					  ||  ( _mscms_dll >zero && _mscms_dll <fver(5,1,2600,2709)) )) {
		NN("Security Update for Windows XP (KB901214)");
		XX(p+"windowsxp-kb901214-x86-enu_2838831de819dad80ea0edaf5fb1e0bfb3c026c0.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && (
		                  ( _netman_dll    >zero && _netman_dll    <fver(5,1,2600,2743)) )) {
		NN("Security Update for Windows XP (KB905414)");
		XX(p+"windowsxp-kb905414-x86-enu_9e8fa8909332653de951edcfdb691f2aa148eb1b.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && (
		                  ( _agentdpv_dll    >zero && _agentdpv_dll    <fver(2,0,0,3423)) )) {
		NN("Security Update for Windows XP (KB890046)");
		XX(p+"windowsxp-kb890046-x86-enu_432bf46f62aeaecc0e519af31d74723096f9b201.exe"+a1);
	}
	/*if( sp==2 && (sku & XP_ALL) && (
		                  ( _catsrv_dll   >zero && _catsrv_dll   <fver(2001,12,4414,308))
					  ||  ( _catsrvut_dll >zero && _catsrvut_dll <fver(2001,12,4414,308))
					  ||  ( _clbcatex_dll >zero && _clbcatex_dll <fver(2001,12,4414,308))
					  ||  ( _clbcatq_dll  >zero && _clbcatq_dll  <fver(2001,12,4414,308))
					  ||  ( _colbact_dll  >zero && _colbact_dll  <fver(2001,12,4414,308))
					  ||  ( _comadmin_dll >zero && _comadmin_dll <fver(2001,12,4414,308))
					  ||  ( _comrepl_dll  >zero && _comrepl_dll  <fver(2001,12,4414,308))
					  ||  ( _comsvcs_dll  >zero && _comsvcs_dll  <fver(2001,12,4414,308))
					  ||  ( _comuid_dll   >zero && _comuid_dll   <fver(2001,12,4414,308))
					  ||  ( _es_dll       >zero && _es_dll       <fver(2001,12,4414,308))
					  ||  ( _migregdb_exe >zero && _migregdb_exe <fver(2001,12,4414,308))
					  ||  ( _msdtcprx_dll >zero && _msdtcprx_dll <fver(2001,12,4414,308))
					  ||  ( _msdtctm_dll  >zero && _msdtctm_dll  <fver(2001,12,4414,308))
					  ||  ( _msdtcuiu_dll >zero && _msdtcuiu_dll <fver(2001,12,4414,308))
					  ||  ( _mtxclu_dll   >zero && _mtxclu_dll   <fver(2001,12,4414,308))
					  ||  ( _mtxoci_dll   >zero && _mtxoci_dll   <fver(2001,12,4414,308))
					  ||  ( _ole32_dll    >zero && _ole32_dll    <fver(5,1,2600,2726))
					  ||  ( _olecli32_dll >zero && _olecli32_dll <fver(5,1,2600,2726))
					  ||  ( _olecnv32_dll >zero && _olecnv32_dll <fver(5,1,2600,2726))
					  ||  ( _rpcss_dll    >zero && _rpcss_dll    <fver(5,1,2600,2726))
					  ||  ( _txflog_dll   >zero && _txflog_dll   <fver(2001,12,4414,308))
					  ||  ( _xolehlp_dll  >zero && _xolehlp_dll  <fver(2001,12,4414,308)) )) {
		// KB902400 is replaced by KB912817 on SP2
		NN("Security Update for Windows XP (KB902400)");
		XX(p+"windowsxp-kb902400-x86-enu_a51d743a1925dd0216160daaf9fc4c7a42a27e53.exe"+a1);
	}*/
	if( sp==2 && (sku & XP_ALL) && (
		                  ( _catsrv_dll   >zero && _catsrv_dll   <fver(2001,12,4414,310))
					  ||  ( _catsrvut_dll >zero && _catsrvut_dll <fver(2001,12,4414,310))
					  ||  ( _clbcatex_dll >zero && _clbcatex_dll <fver(2001,12,4414,310))
					  ||  ( _clbcatq_dll  >zero && _clbcatq_dll  <fver(2001,12,4414,310))
					  ||  ( _colbact_dll  >zero && _colbact_dll  <fver(2001,12,4414,310))
					  ||  ( _comadmin_dll >zero && _comadmin_dll <fver(2001,12,4414,310))
					  ||  ( _comrepl_dll  >zero && _comrepl_dll  <fver(2001,12,4414,310))
					  ||  ( _comsvcs_dll  >zero && _comsvcs_dll  <fver(2001,12,4414,310))
					  ||  ( _comuid_dll   >zero && _comuid_dll   <fver(2001,12,4414,310))
					  ||  ( _es_dll       >zero && _es_dll       <fver(2001,12,4414,310))
					  ||  ( _migregdb_exe >zero && _migregdb_exe <fver(2001,12,4414,310))
					  ||  ( _msdtcprx_dll >zero && _msdtcprx_dll <fver(2001,12,4414,310))
					  ||  ( _msdtctm_dll  >zero && _msdtctm_dll  <fver(2001,12,4414,310))
					  ||  ( _msdtcuiu_dll >zero && _msdtcuiu_dll <fver(2001,12,4414,310))
					  ||  ( _mtxclu_dll   >zero && _mtxclu_dll   <fver(2001,12,4414,310))
					  ||  ( _mtxoci_dll   >zero && _mtxoci_dll   <fver(2001,12,4414,310))
					  ||  ( _ole32_dll    >zero && _ole32_dll    <fver(5,1,2600,2846))
					  ||  ( _olecli32_dll >zero && _olecli32_dll <fver(5,1,2600,2846))
					  ||  ( _olecnv32_dll >zero && _olecnv32_dll <fver(5,1,2600,2846))
					  ||  ( _rpcss_dll    >zero && _rpcss_dll    <fver(5,1,2600,2846))
					  ||  ( _stclient_dll >zero && _stclient_dll <fver(2001,12,4414,310))
					  ||  ( _txflog_dll   >zero && _txflog_dll   <fver(2001,12,4414,310))
					  ||  ( _xolehlp_dll  >zero && _xolehlp_dll  <fver(2001,12,4414,310)) )) {
		NN("Windows Communication Foundation (KB912817)");
		XX(p+"WindowsXP-KB912817-v2-x86-ENU.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && (
		                  ( _jgdw400_dll >zero && _jgdw400_dll <fver(106,0,0,0))
					  ||  ( _jgpl400_dll >zero && _jgpl400_dll <fver( 54,0,0,0)) )) {
		NN("Security Update for Windows XP (KB918439)");
		XX(p+"windowsxp-kb918439-x86-enu_056bf5d0c049e0e8e799593b3508627ee8557dc1.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _moviemk_exe>zero && _moviemk_exe<fver(2,1,4027,0)) {
	//if( sp==3 && (sku & XP_ALL) && _moviemk_exe>zero && _moviemk_exe<fver(2,1,4027,0)) {
		NN("Security Update for Windows XP (KB975561)");
		XX(p+"windowsxp-kb975561-x86-enu_9d1f72aba7bfd6670433e4b997a88860f217e520.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _moviemk_exe>zero && _moviemk_exe<fver(2,1,4028,0)) {
		NN("Security Update for Windows XP (KB981997)");
		XX(p+"WindowsXP-KB981997-x86-ENU.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && (
		                  ( _hh_exe      >zero && _hh_exe      <fver(5,2,3790,2453))
					  ||  ( _hhctrl_ocx  >zero && _hhctrl_ocx  <fver(5,2,3790,2453))
					  ||  ( _hhsetup_dll >zero && _hhsetup_dll <fver(5,2,3790,2453))
					  ||  ( _itircl_dll  >zero && _itircl_dll  <fver(5,2,3790,2453))
					  ||  ( _itss_dll    >zero && _itss_dll    <fver(5,2,3790,2453)) )) {
		NN("Security Update for Windows XP (KB896358)");
		XX(p+"windowsxp-kb896358-x86-enu_42b05278a6f2ee006072af8830c103eab2ce045f.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _msmsgs_exe>zero && _msmsgs_exe<fver(4,7,0,3001)) {
		NN("Security Update for Windows Messenger (KB887472)");
		XX(p+"windowsxp-kb887472-x86-enu_5edc4ccc759d65f4afba8542435172ed54515135.exe"+a6);
	}
	if( sp==2 && (sku & XP_ALL) && _hypertrm_dll>zero && _hypertrm_dll<fver(5,1,2600,2563)) {
		NN("Security Update for Windows XP (KB873339)");
		XX(p+"windowsxp-kb873339-x86-enu_fd28098f5f0e8e629e5b7f64e5cd6b6b722a35a7.exe"+a6);
	}
	if( sp==2 && (sku & XP_ALL) && _spoolsv_exe>zero && _spoolsv_exe<fver(5,1,2600,2696)) {
		NN("Security Update for Windows XP (KB896423)");
		XX(p+"windowsxp-kb896423-x86-enu_baba29a9d96e44e3f55045f749cc82cfa4038f0b.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && (
		                  ( _nwapi32_dll  >zero && _nwapi32_dll  <fver(5,1,2600,3015))
					  ||  ( _nwprovau_dll >zero && _nwprovau_dll <fver(5,1,2600,3015))
					  ||  ( _nwrdr_sys    >zero && _nwrdr_sys    <fver(5,1,2600,3015))
					  ||  ( _nwwks_dll    >zero && _nwwks_dll    <fver(5,1,2600,3015)) )) {
		NN("Security Update for Windows XP (KB923980)");
		XX(p+"windowsxp-kb923980-x86-enu_1f04bf1859d5ba3761e482dbbd48f3795001e391.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && (
		                  ( _remotesp_tsp  >zero && _remotesp_tsp  <fver(5,1,2600,2716))
					  ||  ( _tapisrv_dll   >zero && _tapisrv_dll   <fver(5,1,2600,2716)) )) {
		NN("Security Update for Windows XP (KB893756)");
		XX(p+"windowsxp-kb893756-x86-enu_0fff59c5188cc15ec8f138fda97cb8be1e22da66.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _rdpwd_sys>zero && _rdpwd_sys<fver(5,1,2600,2695)) {
		NN("Security Update for Windows XP (KB899591)");
		XX(p+"windowsxp-kb899591-x86-enu_3022d995581878f4017098b693b4ba35f99dee5c.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _cdosys_dll>zero && _cdosys_dll<fver(6,2,4,0)) {
		NN("Security Update for Windows XP (KB901017)");
		XX(p+"windowsxp-kb901017-x86-enu_04c459695f9018fd31c762bb0a8250cb0506061b.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _webclnt_dll>zero && _webclnt_dll<fver(5,1,2600,2821)) {
		NN("Security Update for Windows XP (KB911927)");
		XX(p+"windowsxp-kb911927-x86-enu_db8cbad537f3f0453deac488f8eb629b3c3a832b.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && (
		                  ( _lsasrv_dll >zero && _lsasrv_dll <fver(5,1,2600,2525))
					  ||  ( _mrxsmb_sys >zero && _mrxsmb_sys <fver(5,1,2600,2541))
					  ||  ( _rdbss_sys  >zero && _rdbss_sys  <fver(5,1,2600,2541)) )) {
		NN("Security Update for Windows XP (KB885835)");
		XX(p+"windowsxp-kb885835-x86-enu_920ecebf9cd90610ec67a305820f98e4186ca748.exe"+a6);
	}
	if( sp==2 && (sku & XP_ALL) && _kerberos_dll>zero && _kerberos_dll<fver(5,1,2600,2698)) {
		NN("Security Update for Windows XP (KB899587)");
		XX(p+"windowsxp-kb899587-x86-enu_95ef03f0da9761b044b9a98d445af90266777ea8.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && (
		                  ( _directdb_dll >zero && _directdb_dll <fver(6,0,2900,3138))
					  ||  ( _inetcomm_dll >zero && _inetcomm_dll <fver(6,0,2900,3138))
					  ||  ( _msoe_dll     >zero && _msoe_dll     <fver(6,0,2900,3138))
					  ||  ( _wab32_dll    >zero && _wab32_dll    <fver(6,0,2900,3138))
					  ||  ( _wabimp_dll   >zero && _wabimp_dll   <fver(6,0,2900,3138)) )) {
		NN("Cumulative Security Update for Outlook Express for Windows XP (KB929123)");
		XX(p+"windowsxp-kb929123-x86-enu_e0a3cac53fbefd5629a4cc6ec79803875ff473b3.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _ipnat_sys>zero && _ipnat_sys<fver(5,1,2600,2524)) {
		NN("Critical Update for Windows XP (KB886185)");
		XX(p+"windowsxp-kb886185-x86-enu_88123f8c8a9a8147529eac8ae4656b86c1018a8f.exe"+a6);
	}
	if( sp==2 && (sku & XP_ALL) && _imekr61_ime>zero && _imekr61_ime<fver(6,1,2600,3)) {
		NN("Security Update for Windows XP (KB901190)");
		XX(p+"windowsxp-kb901190-x86-enu_2497a4e9957ddf13e2343858608f89ef6132efb2.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _esent_dll>zero && _esent_dll<fver(5,1,2600,2780)) {
		NN("Update for Windows XP (KB910437)");
		XX(p+"windowsxp-kb910437-x86-enu_6045df96253a6130f973f5f6ac9d10988ba1b01c.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) 
		&& regQueryDWORD(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Setup\\Oc Manager\\Subcomponents",L"iis_www",&status)==1
		&& _w3svc_dll > zero && _w3svc_dll <fver(5,1,2600,3163)) {
		NN("Security Update for Windows XP (KB939373)");
		XX(p+"windowsxp-kb939373-x86-enu_1060604efc56284ff8208d265f72be4d5394ba32.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && (
		   (_asp51_dll          > zero && _asp51_dll          <fver(5,1,2600,2889))
		|| (_asp51_dll_dllcache > zero && _asp51_dll_dllcache <fver(5,1,2600,2889)) )) {
		NN("Security Update for Windows XP (KB917537)");
		XX(p+"windowsxp-kb917537-x86-enu_a4dbb2338b97e63f46d45f1d69aa6a7908269b13.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL)
		&& regQueryDWORD(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Setup\\Oc Manager\\Subcomponents",L"iis_www",&status)==1
		&& ((_asp51_dll > zero && _asp51_dll <fver(5,1,2600,3291))
		||   _asp51_dll_dllcache < fver(5,1,2600,3291)) ) {
		NN("Security Update for Windows XP (KB942830)");
		XX(p+"windowsxp-kb942830-x86-enu_3b34ded0cc8d3c0de48c3c31f4fae006a2eae923.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) 
		&& regTestKey(L"SYSTEM\\CurrentControlSet\\Services\\IISADMIN")
		&& _infocomm_dll > zero && _infocomm_dll <fver(6,0,2600,3290)) {
		NN("Security Update for Windows XP (KB942831)");
		XX(p+"windowsxp-kb942831-x86-enu_0a8500f0300505cecc06c90840c75796c5e3c9a5.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _xmllite_dll <fver(1,0,1018,0)) {
		NN("XMLLite for Windows XP (KB915865)");
		XX(p+"WindowsXP-KB915865-v11-x86-ENU.exe"+a1);
	}
	if( sp>=2 && (sku & XP_ALL) && _gpprefcl_dll <fver(5,2,3790,4568)) {
		// KB943729 requires KB915865
		NN("Group Policy Preference Client Side Extensions for Windows XP (KB943729)");
		XX(p+"windows-kb943729-x86-enu_e174c41ce3dcbd5c8922d6d1c39df1be425a70e0.exe"+a1);
	}


	// Windows XP Media Center
	if( sp==2 && (sku & XP_ALL) && _wmp_dll>=fver(9,0,0,0) && (
		                  (_blackbox_dll >zero && _blackbox_dll <fver(10,0,0,3802))
		              ||  (_cewmdm_dll   >zero && _cewmdm_dll   <fver(10,0,3790,3802))
					  ||  (_drmclien_dll >zero && _drmclien_dll <fver(10,0,0,3802))
					  ||  (_drmstor_dll  >zero && _drmstor_dll  <fver(10,0,0,3802))
					  ||  (_drmupgds_exe >zero && _drmupgds_exe <fver(10,0,0,3802))
					  ||  (_drmv2clt_dll >zero && _drmv2clt_dll <fver(10,0,0,3802))
					  ||  (_LAPRXY_dll   >zero && _LAPRXY_dll   <fver(10,0,0,3802))
					  ||  (_logagent_exe >zero && _logagent_exe <fver(10,0,0,3802))
					  ||  (_msnetobj_dll >zero && _msnetobj_dll <fver(10,0,0,3802))
					  ||  (_mspmsnsv_dll >zero && _mspmsnsv_dll <fver(10,0,3790,3802))
					  ||  (_mspmsp_dll   >zero && _mspmsp_dll   <fver(10,0,3790,3802))
					  ||  (_msscp_dll    >zero && _msscp_dll    <fver(10,0,0,3802))
					  ||  (_mswmdm_dll   >zero && _mswmdm_dll   <fver(10,0,3790,3802))
					  ||  (_qasf_dll     >zero && _qasf_dll     <fver(10,0,0,3802))
					  ||  (_uwdf_exe     >zero && _uwdf_exe     <fver(5,2,3790,1230))
					  ||  (_wdfapi_dll   >zero && _wdfapi_dll   <fver(5,2,3790,1230))
					  ||  (_wdfmgr_exe   >zero && _wdfmgr_exe   <fver(5,2,3790,1230))
					  ||  (_WMADMOD_dll  >zero && _WMADMOD_dll  <fver(10,0,0,3802))
					  ||  (_WMADMOE_dll  >zero && _WMADMOE_dll  <fver(10,0,0,3802))
					  ||  (_wmasf_dll    >zero && _wmasf_dll    <fver(10,0,0,3802))
					  ||  (_wmdmlog_dll  >zero && _wmdmlog_dll  <fver(10,0,3790,3802))
					  ||  (_wmdmps_dll   >zero && _wmdmps_dll   <fver(10,0,3790,3802))
					  ||  (_wmdrmdev_dll >zero && _wmdrmdev_dll <fver(10,0,0,3802))
					  ||  (_wmdrmnet_dll >zero && _wmdrmnet_dll <fver(10,0,0,3802))
					  ||  (_wmidx_dll    >zero && _wmidx_dll    <fver(10,0,0,3802))
					  ||  (_wmnetmgr_dll >zero && _wmnetmgr_dll <fver(10,0,0,3802))
					  ||  (_wmsdmod_dll  >zero && _wmsdmod_dll  <fver(10,0,0,3802))
					  ||  (_wmsdmoe2_dll >zero && _wmsdmoe2_dll <fver(10,0,0,3802))
					  ||  (_wmsetsdk_exe >zero && _wmsetsdk_exe <fver(10,0,0,3802))
					  ||  (_wmspdmod_dll >zero && _wmspdmod_dll <fver(10,0,0,3802))
					  ||  (_WMSPDMOE_dll >zero && _WMSPDMOE_dll <fver(10,0,0,3802))
					  ||  (_WMVADVD_dll  >zero && _WMVADVD_dll  <fver(10,0,0,3802))
					  ||  (_WMVADVE_DLL  >zero && _WMVADVE_DLL  <fver(10,0,0,3802))
					  ||  (_wmvcore_dll  >zero && _wmvcore_dll  <fver(10,0,0,3802))
					  ||  (_wmvdmod_dll  >zero && _wmvdmod_dll  <fver(10,0,0,3802))
					  ||  (_wmvdmoe2_dll >zero && _wmvdmoe2_dll <fver(10,0,0,3802))
//					  ||  (_wpdconns_dll >zero && _wpdconns_dll <fver(5,2,3802,3802))
//					  ||  (_wpdmtp_dll   >zero && _wpdmtp_dll   <fver(5,2,3802,3802))
//					  ||  (_wpdmtpdr_dll >zero && _wpdmtpdr_dll <fver(5,2,3802,3802))
//					  ||  (_wpdmtpus_dll >zero && _wpdmtpus_dll <fver(5,2,3802,3802))
//					  ||  (_wpdsp_dll    >zero && _wpdsp_dll    <fver(5,2,3802,3802))
//					  ||  (_wpdtrace_dll >zero && _wpdtrace_dll <fver(5,2,3802,3802))
//					  ||  (_wpdusb_sys   >zero && _wpdusb_sys   <fver(5,2,3802,3802))
//					  ||  (_wpd_ci_dll   >zero && _wpd_ci_dll   <fver(5,2,3802,3802))
					      )) {
		kb891122=true;
		NN("Update for WMDRM-enabled Media Players (KB891122)");
		XX(p+"windowsmedia-kb891122-x86-enu_c1b14007407ee75d546dfd820af4f52eaf5e18f9.exe"+a1);
	}
//	if( sp>=2 && (sku & XP_ALL) && _wmp_dll>=fver(10,0,0,3646) && _wmp_dll<fver(10,0,0,3909)) {
//		NN("Update for Windows XP (KB895572)");
//		XX(p+"windowsmedia10-kb895572-x86-enu.exe"+a1);
//	}
	if( sp==2 && (sku & XP_ALL) && (
		                  ( _qdvd_dll   >zero && _qdvd_dll   <fver(6,5,2600,2709))
					  ||  ( _quartz_dll >zero && _quartz_dll <fver(6,5,2600,2709)) )) {
		NN("Update for Windows XP (KB891593)");
		XX(p+"WindowsXP-KB891593-v2-x86-ENU.exe"+a1);
	}
	if((sp==2 && (sku & XP_ALL) && _termsrv_dll>zero && _termsrv_dll<fver(5,1,2600,3352))
	 ||(sp==3 && (sku & XP_ALL) && _termsrv_dll>zero && _termsrv_dll<fver(5,1,2600,5581))) {
		NN("Update for Windows XP (KB895961)");
		XX(p+"WindowsXP-KB895961-v4-x86-ENU.exe"+a1);
	}
//	if( sp==2 && (sku & XP_ALL) && (
//		                  ( _ndisuio_sys  >zero && _ndisuio_sys  <fver(5,1,2600,2626))
//					  ||  ( _netshell_dll >zero && _netshell_dll <fver(5,1,2600,2626))
//					  ||  ( _wzcdlg_dll   >zero && _wzcdlg_dll   <fver(5,1,2600,2676))
//					  ||  ( _wzcsapi_dll  >zero && _wzcsapi_dll  <fver(5,1,2600,2626))
//					  ||  ( _wzcsvc_dll   >zero && _wzcsvc_dll   <fver(5,1,2600,2626)) )) {
//		NN("Update for Windows XP (KB899337)");
//		XX(p+"WindowsXP-KB899337-v2-x86-ENU.exe"+a1);
//	}
//	if( sp==2 && (sku & XP_MCE2005) && (
//			(    _sbp2port_sys       <fver(5,1,2600,2688)
//		      && _sbp2port_sys_cache <fver(5,1,2600,2688))
//			|| (_sbp2port_sys>zero && _sbp2port_sys <fver(5,1,2600,2688))
//			|| !regTestKey(L"SOFTWARE\\Microsoft\\Updates\\Windows XP\\SP3\\KB899510"))) {
//		NN("Update for Windows XP (KB899510)");
//		XX(p+"WindowsXP-KB899510-x86.exe"+a1);
//	}
//	if( sp==2 && (sku & XP_MCE2005) && (
//		                  ( _hidir_sys >zero && _hidir_sys <fver(5,1,2600,2709))
//					  ||  ( _irbus_sys >zero && _irbus_sys <fver(5,1,2600,2709)) )) {
//		NN("Update Rollup 1 for eHome Infrared Receiver for Windows XP Media Center Edition (KB888795)");
//		XX(p+"windowsxp-kb888795-v3-x86-enu_35a20ee3889859f9e0db3d9899c0a679664726be.exe"+a1);
//	}
//	if( sp==2 && (sku & XP_MCE2005) && (
//			(    _powerfil_sys       <fver(5,1,2600,2707)
//		      && _powerfil_sys_cache <fver(5,1,2600,2707))
//			|| (_powerfil_sys>zero && _powerfil_sys <fver(5,1,2600,2707))
//			|| !regTestKey(L"SOFTWARE\\Microsoft\\Updates\\Windows XP\\SP3\\KB902841"))) {
//		NN("Update for Windows XP (KB902841)");
//		XX(p+"WindowsXP-KB902841-x86.exe"+a1);
//	}
	if(sp>=2 && (sku & XP_MCE2005) && _ehkeyctl_dll>zero && _ehkeyctl_dll<fver(5,1,2715,5512)) {
		NN("Security Update for Windows XP (KB973768)");
		XX(p+"windowsxpmediacenter2005-kb973768-enu_05fb491b522e4265834136d7288711e5fdb827e4.exe"+a1);
	}
	if(sp>=2 && (sku & XP_MCE2005) && *nfx11<0) {
		// Update Rollup 2 (KB900325) requires .NET Framework 1.1 SP1
		NN("Microsoft .NET Framework 1.1");
		XX(np+"dotnetf11.exe /q:a /c:\"install.exe /qb /l\"");
	}
	if(sp>=2 && (sku & XP_MCE2005) && *nfx11<1){
		// Update Rollup 2 (KB900325) requires .NET Framework 1.1 SP1
		NN("Microsoft .NET Framework 1.1 Service Pack 1 (KB867460)");
		XX(p+"NDP1.1sp1-KB867460-X86.exe"+a2);
	}
	std::string temp;
	if( sp>=2 && (sku & XP_MCE2005) && (
		    ( _audiodepthconverter_ax    <fver(6,0,0,1))
		||  ( _authorscript_dll          <fver(6,0,0,1))
		||  ( _bdatunepia_dll            <fver(5,1,2710,2732))
		||  ( _createdisc_dll            <fver(2,0,0,0))
		||  ( _directshowtap_ax          <fver(6,0,0,1))
		||  ( _ehcett_dll                <fver(5,1,2710,2732))
		||  ( _ehchhime_dll              <fver(5,1,2710,2732))
		||  ( _ehchtime_dll              <fver(5,1,2710,2732))
		||  ( _ehcir_dll                 <fver(5,1,2710,2732))
		||  ( _ehcm_dll                  <fver(5,1,2710,2732))
		||  ( _ehcommon_dll              <fver(5,1,2710,2732))
		||  ( _ehcyrtt_dll               <fver(5,1,2710,2732))
		||  ( _ehdebug_dll               <fver(5,1,2710,2732))
		||  ( _ehdrop_dll                <fver(5,1,2710,2732))
		||  ( _ehentt_dll                <fver(5,1,2710,2732))
		||  ( _ehepg_dll                 <fver(5,1,2710,2732))
		||  ( _ehepgdat_dll              <fver(5,1,2710,2732))
		||  ( _ehextcom_dll              <fver(5,1,2710,2732))
		||  ( _ehexthost_exe             <fver(5,1,2710,2732))
		||  ( _ehglid_dll                <fver(5,1,2710,2732))
		||  ( _ehiextcom_dll             <fver(5,1,2710,2732))
		||  ( _ehiextens_dll             <fver(5,1,2710,2732))
		||  ( _ehiplay_dll               <fver(5,1,2710,2732))
		||  ( _ehiproxy_dll              <fver(5,1,2710,2732))
		||  ( _ehiuserxp_dll             <fver(5,1,2710,2732))
		||  ( _ehividctl_dll             <fver(5,1,2710,2732))
		||  ( _ehiwuapi_dll              <fver(5,1,2710,2732))
		||  ( _ehjpnime_dll              <fver(5,1,2710,2732))
		||  ( _ehkorime_dll              <fver(5,1,2710,2732))
		||  ( _ehmcxins_exe              <fver(6,1,2710,2732))
		||  ( _ehmsas_exe                <fver(5,1,2710,2732))
		||  ( _ehplayer_dll              <fver(5,1,2710,2732))
		||  ( _ehproxy_dll               <fver(5,1,2710,2732))
		||  ( _ehrec_exe                 <fver(5,1,2710,2732))
		||  ( _ehrecobj_dll              <fver(5,1,2710,2732))
		||  ( _ehrecvr_exe               <fver(5,1,2710,2732))
		||  ( _ehres_dll                 <fver(5,1,2710,2732))
		||  ( _ehreschs_dll              <fver(5,1,2710,2732))
		||  ( _ehrescht_dll              <fver(5,1,2710,2732))
		||  ( _ehrescs_dll               <fver(5,1,2710,2732))
		||  ( _ehresda_dll               <fver(5,1,2710,2732))
		||  ( _ehresde_dll               <fver(5,1,2710,2732))
		||  ( _ehreses_dll               <fver(5,1,2710,2732))
		||  ( _ehresfi_dll               <fver(5,1,2710,2732))
		||  ( _ehresfr_dll               <fver(5,1,2710,2732))
		||  ( _ehresit_dll               <fver(5,1,2710,2732))
		||  ( _ehresja_dll               <fver(5,1,2710,2732))
		||  ( _ehresko_dll               <fver(5,1,2710,2732))
		||  ( _ehresnl_dll               <fver(5,1,2710,2732))
		||  ( _ehresno_dll               <fver(5,1,2710,2732))
		||  ( _ehrespl_dll               <fver(5,1,2710,2732))
		||  ( _ehrespt_dll               <fver(5,1,2710,2732))
		||  ( _ehresru_dll               <fver(5,1,2710,2732))
		||  ( _ehressv_dll               <fver(5,1,2710,2732))
		||  ( _ehrestr_dll               <fver(5,1,2710,2732))
		||  ( _ehsched_exe               <fver(5,1,2710,2732))
		||  ( _ehshell_exe               <fver(5,1,2710,2732))
		||  ( _sqldb20_dll               <fver(2,2,6712,0))
		||  ( _sqlqp20_dll               <fver(2,2,6712,0))
		||  ( _sqlse20_dll               <fver(2,2,6712,0))
		||  ( _ehtktt_dll                <fver(5,1,2710,2732))
		||  ( _ehtrace_dll               <fver(5,1,2710,2732))
		||  ( _ehtray_exe                <fver(5,1,2710,2732))
		||  ( _ehui_dll                  <fver(5,1,2710,2732))
		||  ( _ehuihlp_dll               <fver(5,1,2710,2732))
		||  ( _encdec_dll                <fver(6,5,2710,2732))
		||  ( _mce_wmadrm_dll            <fver(2,0,0,0))
		||  ( _mcrdsvc_exe               <fver(4,1,2710,2732))
		||  ( _mcrmgr_dll                <fver(6,1,2710,2732))
		||  ( _mcrmgr_exe                <fver(6,1,2710,2732))
		||  ( _medctrro_exe              <fver(5,1,2710,2732))
		||  ( _microsoft_mediacenter_dll <fver(5,1,2710,2732))
		||  ( _mpeg2data_ax              <fver(6,5,2710,2732))
		||  ( _mpg2splt_ax               <fver(6,5,2710,2732))
		||  ( _msdvbnp_ax                <fver(6,5,2710,2732))
		||  ( _msnp_ax                   <fver(6,5,2710,2732))
		||  ( _mstvcapn_dll              <fver(5,1,2710,2732))
		||  ( _msvidctl_dll              <fver(6,5,2710,2732))
		||  ( _primosdk_dll              <fver(2,2,45,500))
		||  ( _psisdecd_dll              <fver(6,5,2710,2732))
		||  ( _psisrndr_ax               <fver(6,5,2710,2732))
		||  ( _px_dll                    <fver(2,2,45,500))
		||  ( _pxdrv_dll                 <fver(1,1,33,0))
		||  ( _pxhelp20_sys              <fver(2,3,26,0))
		||  ( _pxhpinst_exe              <fver(2,3,0,0))
		||  ( _pxmas_dll                 <fver(2,2,45,500))
		||  ( _pxwavedec_dll             <fver(2,2,45,0))
		||  ( _pxwma_dll                 <fver(1,0,0,3))
		||  ( _rtstreamsink_ax           <fver(6,0,0,1))
		||  ( _sbe_dll                   <fver(6,5,2710,2732))
		||  ( _sbeserver_exe             <fver(2,0,0,0))
		||  ( _sbeserverps_dll           <fver(2,0,0,0))
		||  ( _simpleburnuires_dll       <fver(0,9,0,0))
		||  ( _sonicmceburnengine_dll    <fver(0,9,0,0))
		||  ( _sonicmmburnengine_exe     <fver(2,0,0,0))
		||  ( _sonicmpegaudio_dll        <fver(3,0,0,14))
		||  ( _sonicmpegsplitter_dll     <fver(3,0,0,19))
		||  ( _sonicmpegvideo_dll        <fver(3,0,0,19))
//		||  ( _spuninst_exe              <fver(5,6,1002,0))
//		||  ( _spupdsvc_exe              <fver(5,6,1002,0))
		||  ( _standardfx_plugin_dll     <fver(2,0,0,0))
		||  ( _tenfoothelp_exe           <fver(5,6,1000,0))
		||  ( _vbicodec_ax               <fver(6,5,2710,2732))
		||  ( _vxblock_dll               <fver(1,0,62,0))
		||  ( _wstpager_ax               <fver(6,5,2710,2732))
		||  ( _wstrenderer_ax            <fver(6,5,2710,2732)) )) {
	kb900325=true;
	NN("Update Rollup 2 for Windows XP Media Center Edition 2005 (KB900325)");
	//XX(p+"WindowsXPMediaCenter2005-KB900325-usa.exe /quiet /norestart");
	temp=p+"WindowsXPMediaCenter2005-KB900325-usa.exe /Q /x:%TEMP%\\KB900325\n"
		  +"pushd %TEMP%\\KB900325\n"
		  +"md install\n"
		  +"copy bin\\WindowsXP-KB891593-x86.exe install\\\n"
		  +"copy bin\\WindowsXP-KB899337-v2-x86.exe install\\\n"
		  +"copy bin\\WindowsXP-KB899510-x86.exe install\\\n"
		  +"copy bin\\WindowsXP-KB888795-x86.exe install\\\n"
		  +"copy bin\\WindowsXP-KB902841-x86.exe install\\\n";
		if( sp>=2 && (sku & XP_ALL) && _wmp_dll>=fver(10,0,0,3646) && _wmp_dll<fver(10,0,0,3937)) {
			//NN("Update for Windows XP (KB895572)"); // Actually it is KB903157
			temp+="bin\\WindowsMedia10-KB895572-x86.exe"+a1+"\n";
		}
//		if( sp==2 && (sku & XP_ALL) && (
//							  ( _qdvd_dll   >zero && _qdvd_dll   <fver(6,5,2600,2709))
//						  ||  ( _quartz_dll >zero && _quartz_dll <fver(6,5,2600,2709)) )) {
//			//NN("Update for Windows XP (KB891593)");
//			temp+="install\\WindowsXP-KB891593-x86.exe"+a1+"\n";
//		}
		if( sp==2 && (sku & XP_ALL) && (
							  ( _ndisuio_sys  >zero && _ndisuio_sys  <fver(5,1,2600,2626))
						  ||  ( _netshell_dll >zero && _netshell_dll <fver(5,1,2600,2626))
						  ||  ( _wzcdlg_dll   >zero && _wzcdlg_dll   <fver(5,1,2600,2676))
						  ||  ( _wzcsapi_dll  >zero && _wzcsapi_dll  <fver(5,1,2600,2626))
						  ||  ( _wzcsvc_dll   >zero && _wzcsvc_dll   <fver(5,1,2600,2626)) )) {
			//NN("Update for Windows XP (KB899337)");
			temp+="install\\WindowsXP-KB899337-v2-x86.exe"+a1+"\n";
		}
		if( sp==2 && (sku & XP_MCE2005) && (
				(    _sbp2port_sys       <fver(5,1,2600,2688)
				  && _sbp2port_sys_cache <fver(5,1,2600,2688))
				|| (_sbp2port_sys>zero && _sbp2port_sys <fver(5,1,2600,2688))
				|| !regTestKey(L"SOFTWARE\\Microsoft\\Updates\\Windows XP\\SP3\\KB899510"))) {
			//NN("Update for Windows XP (KB899510)");
			temp+="install\\WindowsXP-KB899510-x86.exe"+a1+"\n";
		}
		if( sp==2 && (sku & XP_MCE2005) && (
							  ( _hidir_sys >zero && _hidir_sys <fver(5,1,2600,2709))
						  ||  ( _irbus_sys >zero && _irbus_sys <fver(5,1,2600,2709))
						  ||    _hidir_sys_cache < fver(5,1,2600,2709)
						  ||    _irbus_sys_cache < fver(5,1,2600,2709) )) {
			//NN("Update Rollup 1 for eHome Infrared Receiver for Windows XP Media Center Edition (KB888795)");
			temp+="install\\WindowsXP-KB888795-x86.exe"+a1+"\n";
		}
		if( sp==2 && (sku & XP_MCE2005) && (
				(    _powerfil_sys       <fver(5,1,2600,2707)
				  && _powerfil_sys_cache <fver(5,1,2600,2707))
				|| (_powerfil_sys>zero && _powerfil_sys <fver(5,1,2600,2707))
				|| !regTestKey(L"SOFTWARE\\Microsoft\\Updates\\Windows XP\\SP3\\KB902841"))) {
			//NN("Update for Windows XP (KB902841)");
			temp+="install\\WindowsXP-KB902841-x86.exe"+a1+"\n";
		}
		temp+="bin\\KB900325.exe /passive /norestart\n";
		temp+="bin\\wmfdist95.exe /Q:A /R:N /c:\"wmsetsdk.exe /WMFDIST /Q /R:N /DisallowSystemRestore\"\n";
		temp+="popd\nrd /S /Q %TEMP%\\KB900325";
//	XX(p+"WindowsXPMediaCenter2005-KB900325-usa.exe /Q /x:%TEMP%\\KB900325\n"
//		+"pushd %TEMP%\\KB900325\\bin\n"
//		+"WindowsMedia10-KB895572-x86.exe /passive /norestart\n"
//		+"WindowsXP-KB891593-x86.exe /passive /norestart\n"
//		+"WindowsXP-KB895961-x86.exe /passive /norestart\n"
//		+"WindowsXP-KB899337-v2-x86.exe /passive /norestart\n"
//		+"WindowsXP-KB899510-x86.exe /passive /norestart\n"
//		+"WindowsXP-KB888795-x86.exe /passive /norestart\n"
//		+"WindowsXP-KB902841-x86.exe /passive /norestart\n"
//		+"KB900325.exe /passive /norestart\n"
//		+"wmfdist95.exe /Q:A /R:N /c:\"wmsetsdk.exe /WMFDIST /Q /R:N /DisallowSystemRestore\"\n"
//		+"popd\nrd /S /Q %TEMP%\\KB900325");
		XX(temp);
	}
	if(sp>=2 && (sku & XP_MCE2005) && ( kb900325
		 || regQueryValue(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Media Center",L"Ident",&status)==L"4.0"
		 || regQueryValue(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Media Center",L"Ident",&status)==L"4.1" )
		 && (/*_mstvcapn_dll>zero &&*/ _mstvcapn_dll<fver(5,1,2715,5512))) {
		NN("Security Update for Windows XP Media Center Edition 2005 Update Rollup 2 X86 Edition (KB2628259)");
		XX(p+"windowsxpmediacenter2005-kb2628259-enu_78c8b91dfe474d076c66d74e8454960e5bc23fa3.exe"+a1);
	}
	if(sp>=2 && (sku & XP_MCE2005) && ( kb900325
		 || regQueryValue(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Media Center",L"Ident",&status)==L"4.0"
		 || regQueryValue(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Media Center",L"Ident",&status)==L"4.1" )
		 && ((/*_encdec_dll>zero &&*/ _encdec_dll<fver(6,5,2715,5512))
		  || !regTestKey(L"SOFTWARE\\Microsoft\\Updates\\Windows XP\\SP3\\KB2619340") )) {
		NN("Security Update for Windows XP Media Center Edition 2005 Update Rollup 2 X86 Edition (KB2619340)");
		XX(p+"windowsxpmediacenter2005-kb2619340-enu_074452832167002c2b8388847294a6a5950d78f1.exe"+a1);
	}
	if(sp>=2 && (sku & XP_MCE2005) && ( kb900325
		 || regQueryValue(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Media Center",L"Ident",&status)==L"4.0"
		 || regQueryValue(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Media Center",L"Ident",&status)==L"4.1" )
		 && ((/*_encdec_dll>zero &&*/ _encdec_dll<fver(6,5,2715,5512))
		  || (_sbe_dll   >zero && _sbe_dll   <fver(6,5,2715,5512)) )) {
		NN("Security Update for Windows XP Media Center Edition 2005 Update Rollup 2 X86 Edition (KB2502898)");
		XX(p+"windowsxpmediacenter2005-kb2502898-enu_6449e0bc751c138c0a5a31fe0394677038616056.exe"+a1);
	}
	if(sp>=2 && (sku & XP_MCE2005)
		&& ( kb900325 || regQueryValue(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Media Center",L"Ident",&status)==L"4.0" )
		&& (    (_blackbox_dll >zero && _blackbox_dll <fver(11,0,0,4346))
		     || (_cewmdm_dll   >zero && _cewmdm_dll   <fver(10,0,3790,4332))
		     || (_drmupgds_exe >zero && _drmupgds_exe <fver(11,0,0,4332))
		     || (_drmv2clt_dll >zero && _drmv2clt_dll <fver(11,0,0,4346))
		     || (_kb913800_exe >zero && _kb913800_exe <fver(11,0,0,4350))
		     || (_LAPRXY_dll   >zero && _LAPRXY_dll   <fver(10,0,0,4332))
		     || (_logagent_exe >zero && _logagent_exe <fver(10,0,0,4332))
		     || (_mfplat_dll   >zero && _mfplat_dll   <fver(11,0,0,4332))
		     || (_msnetobj_dll >zero && _msnetobj_dll <fver(11,0,0,4332))
		     || (_mspmsnsv_dll >zero && _mspmsnsv_dll <fver(10,0,3790,4332))
		     || (_mspmsp_dll   >zero && _mspmsp_dll   <fver(10,0,3790,4332))
		     || (_msscp_dll    >zero && _msscp_dll    <fver(11,0,0,4332))
		     || (_mswmdm_dll   >zero && _mswmdm_dll   <fver(10,0,3790,4332))
		     || (_qasf_dll     >zero && _qasf_dll     <fver(10,0,0,4332))
//		     || (_spmsg_dll    >zero && _spmsg_dll    <fver(6,2,29,0))
//		     || (_spuninst_exe >zero && _spuninst_exe <fver(6,2,29,0))
//		     || (_spupdsvc_exe >zero && _spupdsvc_exe <fver(6,2,29,0))
		     || (_uwdf_exe     >zero && _uwdf_exe     <fver(5,2,3790,1230))
		     || (_wdfapi_dll   >zero && _wdfapi_dll   <fver(5,2,3790,1230))
		     || (_wdfmgr_exe   >zero && _wdfmgr_exe   <fver(5,2,3790,1230))
		     || (_WMADMOD_dll  >zero && _WMADMOD_dll  <fver(10,0,0,4332))
		     || (_WMADMOE_dll  >zero && _WMADMOE_dll  <fver(10,0,0,4332))
		     || (_wmasf_dll    >zero && _wmasf_dll    <fver(10,0,0,4332))
		     || (_wmdmlog_dll  >zero && _wmdmlog_dll  <fver(10,0,3790,4332))
		     || (_wmdmps_dll   >zero && _wmdmps_dll   <fver(10,0,3790,4332))
		     || (_wmdrmdev_dll >zero && _wmdrmdev_dll <fver(10,0,0,4332))
		     || (_wmdrmnet_dll >zero && _wmdrmnet_dll <fver(10,0,0,4332))
		     || (_wmdrmsdk_dll >zero && _wmdrmsdk_dll <fver(11,0,0,4332))
		     || (_wmidx_dll    >zero && _wmidx_dll    <fver(10,0,0,4332))
		     || (_wmnetmgr_dll >zero && _wmnetmgr_dll <fver(10,0,0,4332))
		     || (_wmp_dll      >zero && _wmp_dll      <fver(10,0,0,3937))
		     || (_wmplayer_exe >zero && _wmplayer_exe <fver(10,0,0,3931))
		     || (_wmploc_dll   >zero && _wmploc_dll   <fver(10,0,0,3931))
		     || (_wmsdmod_dll  >zero && _wmsdmod_dll  <fver(10,0,0,4332))
		     || (_wmsdmoe2_dll >zero && _wmsdmoe2_dll <fver(10,0,0,4332))
		     || (_wmsetsdk_exe >zero && _wmsetsdk_exe <fver(10,0,0,4332))
		     || (_wmspdmod_dll >zero && _wmspdmod_dll <fver(10,0,0,4332))
		     || (_WMSPDMOE_dll >zero && _WMSPDMOE_dll <fver(10,0,0,4332))
		     || (_WMVADVD_dll  >zero && _WMVADVD_dll  <fver(10,0,0,4332))
		     || (_WMVADVE_DLL  >zero && _WMVADVE_DLL  <fver(10,0,0,4332))
		     || (_wmvcore_dll  >zero && _wmvcore_dll  <fver(10,0,0,4332))
		     || (_wmvdmod_dll  >zero && _wmvdmod_dll  <fver(10,0,0,4332))
		     || (_wmvdmoe2_dll >zero && _wmvdmoe2_dll <fver(10,0,0,4332))
		     || (_wpdconns_dll >zero && _wpdconns_dll <fver(5,2,3811,4346))
		     || (_wpdmtp_dll   >zero && _wpdmtp_dll   <fver(5,2,3811,4346))
		     || (_wpdmtpdr_dll >zero && _wpdmtpdr_dll <fver(5,2,3811,4346))
		     || (_wpdmtpus_dll >zero && _wpdmtpus_dll <fver(5,2,3811,4346))
		     || (_wpdsp_dll    >zero && _wpdsp_dll    <fver(5,2,3811,4346))
		     || (_wpdtrace_dll >zero && _wpdtrace_dll <fver(5,2,3811,4346))
		     || (_wpdusb_sys   >zero && _wpdusb_sys   <fver(5,2,3811,4346))
		     || (_wpd_ci_dll   >zero && _wpd_ci_dll   <fver(5,2,3811,4346)) )) {
		NN("Update for Windows XP Media Center Edition 2005 (KB913800)");
		XX(p+"windowsmedia10-kb913800-x86-enu_d92cb6ac9b52c7926f8f94d65217ed297c45fecc.exe"+a1);
		kb913800=true;
	}
	if(sp==2 && (sku & XP_MCE2005) &&  (_wmploc_dll==fver(10,0,0,3931) ||
				( _wmploc_dll<=fver(10,0,0,3931) && kb913800 ))
			&& ((_setup_wm_exe >zero && _setup_wm_exe <fver(10,0,0,3931)) )) {
		NN("Update for Windows XP Media Center Edition 2005 (KB926251)");
		XX(p+"windowsmedia10-kb926251-x86-enu_1e6ce415f8de5a8a3b43e9917daf0f501493dde5.exe"+a1);
	}
	if(sp>=2 && (sku & XP_MCE2005)
		&& ( kb900325 || regQueryValue(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Media Center",L"Ident",&status)==L"4.0" )
		&& (    (_ehcm_dll     >zero && _ehcm_dll     <fver(5,1,2715,3011))
		     || (_ehepg_dll    >zero && _ehepg_dll    <fver(5,1,2715,3011))
		     || (_ehglid_dll   >zero && _ehglid_dll   <fver(5,1,2715,3011))
		     || (_ehkeyctl_dll >zero && _ehkeyctl_dll <fver(5,1,2715,3011))
		     || (_ehrecvr_exe  >zero && _ehrecvr_exe  <fver(5,1,2715,3011))
		     || (_ehshell_exe  >zero && _ehshell_exe  <fver(5,1,2715,3011))
		     || (_ehui_dll     >zero && _ehui_dll     <fver(5,1,2715,3011))
		     || (_encdec_dll   >zero && _encdec_dll   <fver(6,5,2715,3011))
		     || (_mstvcapn_dll >zero && _mstvcapn_dll <fver(5,1,2715,3011))
		     || (_msvidctl_dll >zero && _msvidctl_dll <fver(6,5,2715,3011))
		     || (_psisdecd_dll >zero && _psisdecd_dll <fver(6,5,2715,3011))
		     || (_psisrndr_ax  >zero && _psisrndr_ax  <fver(6,5,2715,3011))
		     || (_pxdrv_dll    >zero && _pxdrv_dll    <fver(1,1,63,0))
		     || (_sbe_dll      >zero && _sbe_dll      <fver(6,5,2715,3011)) )) {
		NN("Update Rollup for Windows XP Media Center Edition 2005 (KB925766)");
		XX(p+"windowsxpmediacenter2005-kb925766-enu_30e90395a057a5e974768536ee7eaf014670ed2c.exe"+a1);
	}
	if( sp==2 && (sku & XP_MCE2005) && ( kb900325
		              || ( _hidir_sys >zero && _hidir_sys <fver(5,1,2600,2825))
					  || ( _irbus_sys >zero && _irbus_sys <fver(5,1,2600,2825)) )) {
		NN("Update for Windows XP Media Center Edition 2005 (KB912024)");
		XX(p+"windowsxp-kb912024-v2-x86-enu_0e305ca77d97d5ee54ea79e3f43bedaba844705b.exe"+a1);
	}

	// Windows XP SP3 updates;
	if( sp==3 && (sku & XP_ALL) && _packager_exe>zero && _packager_exe<fver(5,1,2600,6176)) {
		NN("Security Update for Windows XP (KB2584146)");
		XX(p+"WindowsXP-KB2584146-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _ndproxy_sys>zero && _ndproxy_sys<fver(5,1,2600,6484)) {
		NN("Security Update for Windows XP (KB2914368)");
		XX(p+"WindowsXP-KB2914368-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _iacenc_dll<fver(1,0,0,0)) {
		NN("Security Update for Windows XP (KB2661637)");
		XX(p+"WindowsXP-KB2661637-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _ndistapi_sys>zero && _ndistapi_sys<fver(5,1,2600,6132)) {
		NN("Security Update for Windows XP (KB2566454)");
		XX(p+"WindowsXP-KB2566454-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _csrsrv_dll>zero && _csrsrv_dll<fver(5,1,2600,6166)) {
		NN("Security Update for Windows XP (KB2620712)");
		XX(p+"WindowsXP-KB2620712-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_ntdll_dll>zero && _ntdll_dll<fver(5,1,2600,6055))
		              ||  (_ntkrnlmp_exe>zero && _ntkrnlmp_exe<fver(5,1,2600,6055))
					  ||  (_ntkrnlpa_exe>zero && _ntkrnlpa_exe<fver(5,1,2600,6055))
					  ||  (_ntkrpamp_exe>zero && _ntkrpamp_exe<fver(5,1,2600,6055))
					  ||  (_ntoskrnl_exe>zero && _ntoskrnl_exe<fver(5,1,2600,6055)) )) {
		NN("Security Update for Windows XP (KB2393802)");
		XX(p+"WindowsXP-KB2393802-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _lsasrv_dll>zero && _lsasrv_dll<fver(5,1,2600,6058)) {
		NN("Security Update for Windows XP (KB2478960)");
		XX(p+"WindowsXP-KB2478960-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _iccvid_dll>zero && _iccvid_dll<fver(1,10,0,13)) {
		NN("Security Update for Windows XP (KB982665)");
		XX(p+"WindowsXP-KB982665-x86-ENU.exe"+a1);
	}
	if( sp>=2 && !checkCertificates("Root")) {
		NN("Update for Root Certificates for Windows XP [March 2014] (KB931125)");
		XX(sw+p+"rootsupd_01a26e5c75ff5b3e34fb6b763ace486fe6836aac.exe");
	}
	if( sp>=2 && !checkCertificates("Disallowed")) {
		NN("Update for Certificate Revocation List [March 2015] (KB3050995)");
		XX(sw+p+"rvkroots_3f2ce4676450c06f109b5b4e68bec252873ccc21.exe");
	}
	if( sp==3 && (sku & XP_ALL) && ( (_win32k_sys>zero && _win32k_sys<fver(5,1,2600,6206))
		              ||  (_ntkrnlmp_exe>zero && _ntkrnlmp_exe<fver(5,1,2600,6206))
					  ||  (_ntkrnlpa_exe>zero && _ntkrnlpa_exe<fver(5,1,2600,6206))
					  ||  (_ntkrpamp_exe>zero && _ntkrpamp_exe<fver(5,1,2600,6206))
					  ||  (_ntoskrnl_exe>zero && _ntoskrnl_exe<fver(5,1,2600,6206)) )) {
		NN("Security Update for Windows XP (KB2676562)");
		XX(p+"WindowsXP-KB2676562-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_afd_sys>zero && _afd_sys<fver(5,1,2600,5695))
		              ||  (_dnsapi_dll>zero && _dnsapi_dll<fver(5,1,2600,6089))
					  ||  (_dnsrslvr_dll>zero && _dnsrslvr_dll<fver(5,1,2600,5797))
					  ||  (_mswsock_dll>zero && _mswsock_dll<fver(5,1,2600,5625))
					  ||  (_tcpip_sys>zero && _tcpip_sys<fver(5,1,2600,5625))
					  ||  (_tcpip6_sys>zero && _tcpip6_sys<fver(5,1,2600,5625)) )) {
		NN("Security Update for Windows XP (KB2509553)");
		XX(p+"WindowsXP-KB2509553-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_usbccgp_sys>zero && _usbccgp_sys<fver(5,1,2600,6437))
		              ||  (_usbd_sys>zero && _usbd_sys<fver(5,1,2600,6437))
					  ||  (_usbehci_sys>zero && _usbehci_sys<fver(5,1,2600,5778))
					  ||  (_usbport_sys>zero && _usbport_sys<fver(5,1,2600,6437)) )) {
		NN("Security Update for Windows XP (KB2862330)");
		XX(p+"WindowsXP-KB2862330-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _rdpwd_sys>zero && _rdpwd_sys<fver(5,1,2600,6258)) {
		NN("Security Update for Windows XP (KB2723135)");
		XX(p+"WindowsXP-KB2723135-v2-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _synceng_dll>zero && _synceng_dll<fver(5,1,2600,6293)) {
		NN("Security Update for Windows XP (KB2727528)");
		XX(p+"WindowsXP-KB2727528-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_browser_dll>zero && _browser_dll<fver(5,1,2600,6260))
					  ||  (_netapi32_dll>zero && _netapi32_dll<fver(5,1,2600,6260)) )) {
		NN("Security Update for Windows XP (KB2705219)");
		XX(p+"WindowsXP-KB2705219-v2-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_msado15_dll>zero && !regTestKey(L"SOFTWARE\\Microsoft\\Updates\\Windows XP\\SP4\\KB2698365"))
		            //||  (_msadco_dll>zero && _msadco_dll<fver(2,81,3014,0))
		              ||  (_msado15_dll>zero && _msado15_dll<fver(2,81,3014,0))
		// NOTE: A lot of these files are QFE only (not GDR), so testing for them is misleading.
					/*||  (_msadomd_dll>zero && _msadomd_dll<fver(2,81,3014,0))
					  ||  (_msador15_dll>zero && _msador15_dll<fver(2,81,3014,0))
					  ||  (_msadox_dll>zero && _msadox_dll<fver(2,81,3014,0))
					  ||  (_msjro_dll>zero && _msjro_dll<fver(2,81,3014,0))
					  ||  (_msado20_tlb>zero && _msado20_tlb<fver(2,81,3014,0))
					  ||  (_msado21_tlb>zero && _msado21_tlb<fver(2,81,3014,0))
					  ||  (_msado25_tlb>zero && _msado25_tlb<fver(2,81,3014,0))
					  ||  (_msado26_tlb>zero && _msado26_tlb<fver(2,81,3014,0))
					  ||  (_msado27_tlb>zero && _msado27_tlb<fver(2,81,3014,0))
					  ||  (_msado28_tlb>zero && _msado28_tlb<fver(2,81,3014,0))*/ )) {
		NN("Security Update for Windows XP (KB2698365)");
		XX(p+"WindowsXP-KB2698365-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _scrrun_dll>zero && _scrrun_dll<fver(5,7,6002,18960)) {
		NN("Security Update for Windows XP (KB2892075)");
		XX(p+"WindowsXP-KB2892075-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_mfc42_dll>zero && _mfc42_dll<fver(6,2,8081,0))
					  ||  (_mfc42u_dll>zero && _mfc42u_dll<fver(6,2,8081,0)) )) {
		NN("Security Update for Windows XP (KB2506212)");
		XX(p+"WindowsXP-KB2506212-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_imagehlp_dll>zero && _imagehlp_dll<fver(5,1,2600,6198))
					  ||  (_wintrust_dll>zero && _wintrust_dll<fver(5,131,2600,6198)) )) {
		NN("Security Update for Windows XP (KB2653956)");
		XX(p+"WindowsXP-KB2653956-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_srv_sys>zero && _srv_sys<fver(5,1,2600,6082))
					  ||  ( _xpsp4res_dll<fver(5,1,2600,6082)) )) {
		NN("Security Update for Windows XP (KB2508429)");
		XX(p+"WindowsXP-KB2508429-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_msadco_dll>zero && _msadco_dll<fver(2,81,3012,0))
		              ||  (_msado15_dll>zero && _msado15_dll<fver(2,81,3012,0))
					  ||  (_msadomd_dll>zero && _msadomd_dll<fver(2,81,3012,0))
					  ||  (_msadox_dll>zero && _msadox_dll<fver(2,81,3012,0))
					  ||  (_msjro_dll>zero && _msjro_dll<fver(2,81,3012,0))
					  ||  (_odbc32_dll>zero && _odbc32_dll<fver(3,525,3012,0))
					  ||  (_msado20_tlb>zero && _msado20_tlb<fver(2,81,3012,0))
					  ||  (_msado21_tlb>zero && _msado21_tlb<fver(2,81,3012,0))
					  ||  (_msado25_tlb>zero && _msado25_tlb<fver(2,81,3012,0))
					  ||  (_msado26_tlb>zero && _msado26_tlb<fver(2,81,3012,0))
					  ||  (_msado27_tlb>zero && _msado27_tlb<fver(2,81,3012,0)) )) {
	// NOTE: GDR and QFE file lists and versions are identical for this patch.
		NN("Security Update for Windows XP (KB2419632)");
		XX(p+"WindowsXP-KB2419632-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _msxml6_dll>zero && _msxml6_dll<fver(6,20,2502,0)) {
		NN("Security Update for Windows XP (KB2757638)");
		XX(p+"WindowsXP-KB2757638-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _imagehlp_dll>zero && _imagehlp_dll<fver(5,1,2600,6479)) {
		NN("Security Update for Windows XP (KB2893294)");
		XX(p+"WindowsXP-KB2893294-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && !regTestKey(L"SOFTWARE\\Microsoft\\Updates\\Windows XP\\SP4\\KB2603381")) {
		NN("Security Update for Windows XP (KB2603381)");
		XX(p+"WindowsXP-KB2603381-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _winsrv_dll>zero && _winsrv_dll<fver(5,1,2600,6368)) {
		NN("Security Update for Windows XP (KB2820917)");
		XX(p+"WindowsXP-KB2820917-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && !regTestKey(L"SOFTWARE\\Microsoft\\Updates\\Windows XP\\SP4\\KB2570947")) {
		NN("Security Update for Windows XP (KB2570947)");
		XX(p+"WindowsXP-KB2570947-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_usb8023_sys>zero && _usb8023_sys<fver(5,1,2600,6352))
					  ||  (_usb8023x_sys>zero && _usb8023x_sys<fver(5,1,2600,6352)) )) {
		NN("Security Update for Windows XP (KB2807986)");
		XX(p+"WindowsXP-KB2807986-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_ntkrnlmp_exe>zero && _ntkrnlmp_exe<fver(5,1,2600,6419))
					  ||  (_ntkrnlpa_exe>zero && _ntkrnlpa_exe<fver(5,1,2600,6419))
					  ||  (_ntkrpamp_exe>zero && _ntkrpamp_exe<fver(5,1,2600,6419))
					  ||  (_ntoskrnl_exe>zero && _ntoskrnl_exe<fver(5,1,2600,6419)) )) {
		NN("Security Update for Windows XP (KB2859537)");
		XX(p+"WindowsXP-KB2859537-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _gdi32_dll>zero && _gdi32_dll<fver(5,1,2600,6460)) {
		NN("Security Update for Windows XP (KB2876331)");
		XX(p+"WindowsXP-KB2876331-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _usp10_dll>zero && _usp10_dll<fver(1,420,2600,6421)) {
		NN("Security Update for Windows XP (KB2850869)");
		XX(p+"WindowsXP-KB2850869-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _mup_sys>zero && _mup_sys<fver(5,1,2600,6103)) {
		NN("Security Update for Windows XP (KB2535512)");
		XX(p+"WindowsXP-KB2535512-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _dpnet_dll>zero && _dpnet_dll<fver(5,3,2600,6311)) {
		NN("Security Update for Windows XP (KB2770660)");
		XX(p+"WindowsXP-KB2770660-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _afd_sys>zero && _afd_sys<fver(5,1,2600,6142)) {
		NN("Security Update for Windows XP (KB2592799)");
		XX(p+"WindowsXP-KB2592799-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _oakley_dll>zero && _oakley_dll<fver(5,1,2600,6462)) {
		NN("Security Update for Windows XP (KB2862152)");
		XX(p+"WindowsXP-KB2862152-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_msxml3_dll>zero && _msxml3_dll<fver(8,100,1053,0))
					  ||  (_msxml6_dll>zero && _msxml6_dll<fver(6,20,2501,0)) )) {
		NN("Security Update for Windows XP (KB2719985)");
		XX(p+"WindowsXP-KB2719985-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _themeui_dll>zero && _themeui_dll<fver(6,0,2900,6437)) {
		NN("Security Update for Windows XP (KB2864063)");
		XX(p+"WindowsXP-KB2864063-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _win32k_sys>zero && _win32k_sys<fver(5,1,2600,6514)) {
		NN("Security Update for Windows XP (KB2930275)");
		XX(p+"WindowsXP-KB2930275-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_ole32_dll>zero && _ole32_dll<fver(5,1,2600,6010))
		              ||  (_wordpad_exe>zero && _wordpad_exe<fver(5,1,2600,6010)) )) {
		NN("Security Update for Windows XP (KB979687)");
		XX(p+"WindowsXP-KB979687-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_shell32_dll>zero && _shell32_dll<fver(6,0,2900,6072))
					  ||  (_shimgvw_dll>zero && _shimgvw_dll<fver(6,0,2900,6072)) )) {
		NN("Security Update for Windows XP (KB2483185)");
		XX(p+"WindowsXP-KB2483185-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _ole32_dll>zero && _ole32_dll<fver(5,1,2600,6435)) {
		NN("Security Update for Windows XP (KB2876217)");
		XX(p+"WindowsXP-KB2876217-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _spoolsv_exe>zero && _spoolsv_exe<fver(5,1,2600,6024)) {
		NN("Security Update for Windows XP (KB2347290)");
		XX(p+"WindowsXP-KB2347290-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_CLIENT) && !regTestKey(L"SOFTWARE\\Microsoft\\\Updates\\Windows XP\\SP4\\KB2686509")) {
		NN("Security Update for Windows XP (KB2686509)");
		XX(p+"WindowsXP-KB2686509-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_EMBEDDED) && !regTestKey(L"SOFTWARE\\Microsoft\\\Updates\\Windows XP\\SP4\\KB2686509")) {
		NN("Security Update for Windows XP Embedded (KB2686509)");
		XX(p+"WindowsXPEmbedded-KB2686509-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_mpg2splt_ax>zero && _mpg2splt_ax<fver(6,5,2600,6333))
					  ||  (_quartz_dll>zero && _quartz_dll<fver(6,5,2600,6333)) )) {
		NN("Security Update for Windows XP (KB2780091)");
		XX(p+"WindowsXP-KB2780091-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_csrsrv_dll>zero && _csrsrv_dll<fver(5,1,2600,6104))
					  ||  (_winsrv_dll>zero && _winsrv_dll<fver(5,1,2600,6104)) )) {
		NN("Security Update for Windows XP (KB2507938)");
		XX(p+"WindowsXP-KB2507938-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_hidparse_sys>zero && _hidparse_sys<fver(5,1,2600,6418))
					  ||  (_usbscan_sys>zero && _usbscan_sys<fver(5,1,2600,6418)) )) {
		NN("Security Update for Windows XP (KB2862335)");
		XX(p+"WindowsXP-KB2862335-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _t2embed_dll>zero && _t2embed_dll<fver(5,1,2600,6031)) {
		NN("Security Update for Windows XP (KB982132)");
		XX(p+"WindowsXP-KB982132-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _qedit_dll>zero && _qedit_dll<fver(6,5,2600,6512)) {
		NN("Security Update for Windows XP (KB2929961)");
		XX(p+"WindowsXP-KB2929961-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_mciseq_dll>zero && _mciseq_dll<fver(5,1,2600,6160))
					  ||  (_winmm_dll>zero && _winmm_dll<fver(5,1,2600,6160)) )) {
		NN("Security Update for Windows XP (KB2598479)");
		XX(p+"WindowsXP-KB2598479-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _mswrd8_wpc>zero && _mswrd8_wpc<fver(2011,1,31,10)) {
		NN("Security Update for Windows XP (KB2485663)");
		XX(p+"WindowsXP-KB2485663-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_rpcrt4_dll>zero && _rpcrt4_dll<fver(5,1,2600,6477))
					  ||  (_xpsp4res_dll<fver(5,1,2600,6477)) )) {
		NN("Security Update for Windows XP (KB2898715)");
		XX(p+"WindowsXP-KB2898715-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _oleaut32_dll>zero && _oleaut32_dll<fver(5,1,2600,6341)) {
		NN("Security Update for Windows XP (KB2802968)");
		XX(p+"WindowsXP-KB2802968-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _schannel_dll>zero && _schannel_dll<fver(5,1,2600,6239)) {
		NN("Security Update for Windows XP (KB2655992)");
		XX(p+"WindowsXP-KB2655992-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _isign32_dll>zero && _isign32_dll<fver(6,0,2900,6052)) {
		NN("Security Update for Windows XP (KB2443105)");
		XX(p+"WindowsXP-KB2443105-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _atmfd_dll>zero && _atmfd_dll<fver(5,1,2,236)) {
		NN("Security Update for Windows XP (KB2847311)");
		XX(p+"WindowsXP-KB2847311-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _shell32_dll>zero && _shell32_dll<fver(6,0,2900,6242)) {
		NN("Security Update for Windows XP (KB2691442)");
		XX(p+"WindowsXP-KB2691442-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _comctl32_dll>zero && _comctl32_dll<fver(5,82,2900,6028)) {
		NN("Security Update for Windows XP (KB2296011)");
		XX(p+"WindowsXP-KB2296011-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_qdvd_dll>zero && _qdvd_dll<fver(6,5,2600,6169))
					  ||  (_quartz_dll>zero && _quartz_dll<fver(6,5,2600,6169)) )) {
		NN("Security Update for Windows XP (KB2631813)");
		XX(p+"WindowsXP-KB2631813-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_schannel_dll>zero && _schannel_dll<fver(5,1,2600,6175))
					  ||  (_winhttp_dll>zero && _winhttp_dll<fver(5,1,2600,6175)) )) {
		NN("Security Update for Windows XP (KB2585542)");
		XX(p+"WindowsXP-KB2585542-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _mrxsmb_sys>zero && _mrxsmb_sys<fver(5,1,2600,6133)) {
		NN("Security Update for Windows XP (KB2536276)");
		XX(p+"WindowsXP-KB2536276-v2-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _gdiplus_dll_23084<fver(5,2,6002,23084)) {
		NN("Security Update for Windows XP (KB2834886)");
		XX(p+"WindowsXP-KB2834886-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _inetcomm_dll>zero && _inetcomm_dll<fver(6,0,2900,6157)) {
		NN("Security Update for Windows XP (KB2544893)");
		XX(p+"WindowsXP-KB2544893-v2-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _msxml3_dll>zero && _msxml3_dll<fver(8,100,1054,0)) {
		NN("Security Update for Windows XP (KB2916036)");
		XX(p+"WindowsXP-KB2916036-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_oleacc_dll>zero && _oleacc_dll<fver(7,0,2600,6153))
		              ||  (_oleaccrc_dll>zero && _oleaccrc_dll<fver(7,0,2600,6153))
					  ||  (_uiautomationcore_dll<fver(7,0,2600,6153)) )) {
		NN("Security Update for Windows XP (KB2564958)");
		XX(p+"Windows-en-US-KB2564958.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _gdiplus_dll_22791<fver(5,2,6002,22791)) {
		NN("Security Update for Windows XP (KB2659262)");
		XX(p+"WindowsXP-KB2659262-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( ( /*_encdec_dll>zero &&*/ _encdec_dll<fver(6,5,2600,6076))
					  ||  (_sbe_dll>zero && _sbe_dll<fver(6,5,2600,6076)) )) {
		NN("Security Update for Windows XP (KB2479943)");
		XX(p+"WindowsXP-KB2479943-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) /*&& _encdec_dll>zero*/ && _encdec_dll<fver(6,5,2600,6161)) {
		NN("Security Update for Windows XP (KB2619339)");
		XX(p+"WindowsXP-KB2619339-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_mfc40_dll>zero && _mfc40_dll<fver(4,1,0,6151))
		              ||  (_mfc40u_dll>zero && _mfc40u_dll<fver(4,1,0,6151))
					  ||  (_mfc42_dll>zero && _mfc42_dll<fver(6,2,8073,0))
					  ||  (_mfc42u_dll>zero && _mfc42u_dll<fver(6,2,8073,0)) )) {
		NN("Security Update for Windows XP (KB2387149)");
		XX(p+"WindowsXP-KB2387149-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _kernel32_dll>zero && _kernel32_dll<fver(5,1,2600,6532)) {
		NN("Security Update for Windows XP (KB2922229)");
		XX(p+"WindowsXP-KB2922229-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_crypt32_dll>zero && _crypt32_dll<fver(5,131,2600,6459))
					  ||  ( _xpsp4res_dll<fver(5,1,2600,6459)) )) {
		NN("Security Update for Windows XP (KB2868626)");
		XX(p+"WindowsXP-KB2868626-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _tzchange_exe<fver(5,1,2600,6479)) {
		NN("Update for Windows XP (KB2904266)");
		XX(p+"WindowsXP-KB2904266-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _wab_exe>zero && _wab_exe<fver(6,0,2900,6040)) {
		NN("Security Update for Windows XP (KB2423089)");
		XX(p+"WindowsXP-KB2423089-x86-ENU.exe"+a1);
	}
	if((sp==2 && (sku & XP_ALL) && ( ( _exfat_sys   < fver(5,1,2600,3453))
		||   ( _fmifs_dll   < fver(5,1,2600,3453))
		||   ( _format_com > zero && _format_com  < fver(5,1,2600,3453))
		||   ( _fs_rec_sys  < fver(5,1,2600,3453))
		||   ( _ifsutil_dll < fver(5,1,2600,3453))
		||   ( _shell32_dll < fver(6,0,2900,3453))
		||   ( _uexfat_dll  < fver(5,1,2600,3453))
		||   ( _ulib_dll    < fver(5,1,2600,3453))))
	 ||(sp==3 && (sku & XP_ALL) && ( ( _exfat_sys   < fver(5,1,2600,5686))
		||   ( _fmifs_dll   < fver(5,1,2600,5686))
		||   ( _format_com > zero && _format_com  < fver(5,1,2600,5686))
		||   ( _fs_rec_sys  < fver(5,1,2600,5686))
		||   ( _ifsutil_dll < fver(5,1,2600,5686))
		||   ( _shell32_dll < fver(6,0,2900,5686))
		||   ( _uexfat_dll  < fver(5,1,2600,5686))
		||   ( _ulib_dll    < fver(5,1,2600,5686))))) {
		NN("Update for Windows XP (KB955704)"); // exFAT File System Driver
		XX(p+"WindowsXP-KB955704-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_wintrust_dll>zero && _wintrust_dll<fver(5,131,2600,6285)) )) {
		NN("Update for Windows XP (KB2749655)");
		XX(p+"WindowsXP-KB2749655-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_shell32_dll>zero && _shell32_dll<fver(6,0,2900,5853))
					  ||  ( _shsvcs_dll >zero && _shsvcs_dll <fver(6,0,2900,5853)) )) {
		NN("Update for Windows XP (KB971029)");
		XX(p+"WindowsXP-KB971029-x86-ENU.exe"+a1);
	}
	if((sp==2 && (sku & XP_ALL) && (
		     ( _kerberos_dll > zero && _kerberos_dll < fver(5,1,2600,3592))
		||   ( _ksecdd_sys   < fver(5,1,2600,3592))
		||   ( _lsasrv_dll   < fver(5,1,2600,3592))
		||   ( _msv1_0_dll   > zero && _msv1_0_dll   < fver(5,1,2600,3592))
		||   ( _schannel_dll > zero && _schannel_dll < fver(5,1,2600,3592))
		||   ( _secur32_dll  > zero && _secur32_dll  < fver(5,1,2600,3592))
		||   ( _wdigest_dll  > zero && _wdigest_dll  < fver(5,1,2600,3592))))
	 ||(sp==3 && (sku & XP_ALL) && (
		     ( _kerberos_dll > zero && _kerberos_dll < fver(5,1,2600,5834))
		||   ( _ksecdd_sys   < fver(5,1,2600,5834))
		||   ( _lsasrv_dll   < fver(5,1,2600,5834))
		||   ( _msv1_0_dll   > zero && _msv1_0_dll   < fver(5,1,2600,5834))
		||   ( _schannel_dll > zero && _schannel_dll < fver(5,1,2600,5834))
		||   ( _secur32_dll  > zero && _secur32_dll  < fver(5,1,2600,5834))
		||   ( _wdigest_dll  > zero && _wdigest_dll  < fver(5,1,2600,5834))))) {
		NN("Update for Windows XP (KB968389)");
		XX(p+"WindowsXP-KB968389-x86-ENU.exe"+a1);
	}
	if((sp==2 && (sku & XP_ALL) && _msv1_0_dll>zero && _msv1_0_dll<fver(5,1,2600,3625))
	 ||(sp==3 && (sku & XP_ALL) && _msv1_0_dll>zero && _msv1_0_dll<fver(5,1,2600,5876))) {
		NN("Security Update for Windows XP (KB975467)");
		XX(p+"windowsxp-kb975467-x86-enu_703673b065074bb9dd7d849f420e52ea0d343f8d.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_crypt32_dll  >zero && _crypt32_dll  <fver(5,131,2600,6369))
					  ||  ( _cryptdlg_dll >zero && _cryptdlg_dll <fver(5,1,2600,6369)) )) {
		NN("Update for Windows XP (KB2808679)");
		XX(p+"WindowsXP-KB2808679-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (/*_aclayers_dll>zero &&*/ _aclayers_dll<fver(5,1,2600,6093)) )) {
		NN("Update for Windows XP (KB2492386)");
		XX(p+"WindowsXP-KB2492386-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & (XP_ALL)) && (
		     ( _irbus_sys    > zero && _irbus_sys    < fver(5,1,2600,6425))
		||   ( _usbaudio_sys > zero && _usbaudio_sys < fver(5,1,2600,6425))
		||   ( _usbvideo_sys > zero && _usbvideo_sys < fver(5,1,2600,6425)) )) {
		NN("Security Update for Windows XP (KB2868038)");
		XX(p+"WindowsXP-KB2868038-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_PLUS_FLP) && (
		     ( _kerberos_dll > zero && _kerberos_dll < fver(5,1,2600,6059)) )) {
		NN("Security Update for Windows XP (KB2478971)");
		XX(p+"WindowsXP-KB2478971-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_PLUS_FLP) && (
		     ( _localspl_dll > zero && _localspl_dll < fver(5,1,2600,6226)) )) {
		 NN("Security Update for Windows XP (KB2712808)");
		XX(p+"WindowsXP-KB2712808-x86-ENU.exe"+a1);
	}
	
	if( sp>=2 && (sku & XP_ALL) && (
		     ( _axaltocm_dll < fver(6,0,5066,164))
		||   ( _basecsp_dll  < fver(6,0,5066,160))
		||   ( _bcsprsrc_dll < fver(6,0,5066,164))
		||   ( _ifxcardm_dll < fver(6,0,5066,164))
		||   ( _pintool_exe  < fver(6,0,5066,164)) )) {
		NN("Microsoft Base Smart Card Cryptographic Service Provider Package: x86 (KB909520)");
		XX(p+"Windows-KB909520-v1.000-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( ( _srv_sys    >zero && _srv_sys    <fver(5,1,2600,6031))
					  ||   ( _srvsvc_dll >zero && _srvsvc_dll <fver(5,1,2600,6031)) )) {
		// To use the features that this update includes, you must have Windows 7, Windows 2008 R2, or
		// update 968389 installed on the computer.
		NN("Update for Windows XP (KB2345886)");
		XX(p+"WindowsXP-KB2345886-x86-ENU.exe"+a1);
	}
	if((sp==2 && (sku & XP_ALL) && ( (_http_sys    >zero && _http_sys     <fver(5,1,2600,3637))
		              ||  (_httpapi_dll  >zero && _httpapi_dll  <fver(5,1,2600,3637))
					  ||  (_strmfilt_dll >zero && _strmfilt_dll <fver(6,0,2600,3637))))
	 ||(sp==3 && (sku & XP_ALL) && ( (_http_sys    >zero && _http_sys     <fver(5,1,2600,5891))
		              ||  (_httpapi_dll  >zero && _httpapi_dll  <fver(5,1,2600,5891))
					  ||  (_strmfilt_dll >zero && _strmfilt_dll <fver(6,0,2600,5891))))) {
		// To use the features that this update includes, you must have the update 968389 installed
		// on the computer.
		NN("Update for Windows XP (KB970430)");
		XX(p+"WindowsXP-KB970430-x86-ENU.exe"+a1);
	}

	// Updates for Microsoft Visual C++ Runtime Versions
	if( sp>=2 && (sku & XP_ALL) && (/*(_ATL80_dll    >zero && _ATL80_dll    <fver(8,0,50727,6195))
		              ||*/  (_msdia80_dll  >zero && _msdia80_dll  <fver(8,0,50727,6195))
		              ||  (_mfc80_dll    >zero && _mfc80_dll    <fver(8,0,50727,6195))
		              ||  (_mfc80u_dll   >zero && _mfc80u_dll   <fver(8,0,50727,6195))
		              ||  (_mfcm80_dll   >zero && _mfcm80_dll   <fver(8,0,50727,6195))
		              ||  (_mfcm80u_dll  >zero && _mfcm80u_dll  <fver(8,0,50727,6195))
		              ||  (_mfc80CHS_dll >zero && _mfc80CHS_dll <fver(8,0,50727,6195))
		              ||  (_mfc80CHT_dll >zero && _mfc80CHT_dll <fver(8,0,50727,6195))
		              ||  (_mfc80DEU_dll >zero && _mfc80DEU_dll <fver(8,0,50727,6195))
		              ||  (_mfc80ENU_dll >zero && _mfc80ENU_dll <fver(8,0,50727,6195))
		              ||  (_mfc80ESP_dll >zero && _mfc80ESP_dll <fver(8,0,50727,6195))
		              ||  (_mfc80FRA_dll >zero && _mfc80FRA_dll <fver(8,0,50727,6195))
		              ||  (_mfc80ITA_dll >zero && _mfc80ITA_dll <fver(8,0,50727,6195))
		              ||  (_mfc80JPN_dll >zero && _mfc80JPN_dll <fver(8,0,50727,6195))
		              ||  (_mfc80KOR_dll >zero && _mfc80KOR_dll <fver(8,0,50727,6195))
		              ||  (_msvcm80_dll  >zero && _msvcm80_dll  <fver(8,0,50727,6195))
		              ||  (_msvcp80_dll  >zero && _msvcp80_dll  <fver(8,0,50727,6195))
		              ||  (_msvcr80_dll  >zero && _msvcr80_dll  <fver(8,0,50727,6195))
		              ||  (_vcomp_dll    >zero && _vcomp_dll    <fver(8,0,50727,6195)) )) {
		NN("Security Update for Microsoft Visual C++ 2005 Service Pack 1 Redistributable Package (KB2538242)");
		XX(p+"vcredist_x86_b8fab0bb7f62a24ddfe77b19cd9a1451abd7b847.exe"+a2);
	}
	if( sp>=2 && (sku & XP_ALL) && (/*(_atl90_dll    >zero && _atl90_dll    <fver(9,0,30729,6161))
		              ||*/  (_msvcm90_dll  >zero && _msvcm90_dll  <fver(9,0,30729,6161))
		              ||  (_msvcp90_dll  >zero && _msvcp90_dll  <fver(9,0,30729,6161))
		              ||  (_msvcr90_dll  >zero && _msvcr90_dll  <fver(9,0,30729,6161))
		              ||  (_mfc90_dll    >zero && _mfc90_dll    <fver(9,0,30729,6161))
		              ||  (_mfc90u_dll   >zero && _mfc90u_dll   <fver(9,0,30729,6161))
		              ||  (_mfcm90_dll   >zero && _mfcm90_dll   <fver(9,0,30729,6161))
		              ||  (_mfcm90u_dll  >zero && _mfcm90u_dll  <fver(9,0,30729,6161))
		              ||  (_mfc90chs_dll >zero && _mfc90chs_dll <fver(9,0,30729,6161))
		              ||  (_mfc90cht_dll >zero && _mfc90cht_dll <fver(9,0,30729,6161))
		              ||  (_mfc90deu_dll >zero && _mfc90deu_dll <fver(9,0,30729,6161))
		              ||  (_mfc90enu_dll >zero && _mfc90enu_dll <fver(9,0,30729,6161))
		              ||  (_mfc90esn_dll >zero && _mfc90esn_dll <fver(9,0,30729,6161))
		              ||  (_mfc90esp_dll >zero && _mfc90esp_dll <fver(9,0,30729,6161))
		              ||  (_mfc90fra_dll >zero && _mfc90fra_dll <fver(9,0,30729,6161))
		              ||  (_mfc90ita_dll >zero && _mfc90ita_dll <fver(9,0,30729,6161))
		              ||  (_mfc90jpn_dll >zero && _mfc90jpn_dll <fver(9,0,30729,6161))
		              ||  (_mfc90kor_dll >zero && _mfc90kor_dll <fver(9,0,30729,6161))
		              ||  (_vcomp90_dll  >zero && _vcomp90_dll  <fver(9,0,30729,6161)) )) {
		NN("Security Update for Microsoft Visual C++ 2008 Service Pack 1 Redistributable Package (KB2538243)");
		XX(p+"vcredist_x86_470640aa4bb7db8e69196b5edb0010933569e98d.exe /qb");
	}
	if( sp==3 && (sku & XP_ALL) && !vs2010sp1 && (/*(_atl100_dll    >zero && _atl100_dll    <fver(10,0,30319,415))
		              ||*/  (_msdia100_dll  >zero && _msdia100_dll  <fver(10,0,30319,1))
		            //||  (_msvcm100_dll  >zero && _msvcm100_dll  <fver(10,0,30319,415))
		              ||  (_msvcp100_dll  >zero && _msvcp100_dll  <fver(10,0,30319,415))
		              ||  (_msvcr100_dll  >zero && _msvcr100_dll  <fver(10,0,30319,415))
		              ||  (_mfc100_dll    >zero && _mfc100_dll    <fver(10,0,30319,415))
		              ||  (_mfc100u_dll   >zero && _mfc100u_dll   <fver(10,0,30319,415))
		              ||  (_mfcm100_dll   >zero && _mfcm100_dll   <fver(10,0,30319,415))
		              ||  (_mfcm100u_dll  >zero && _mfcm100u_dll  <fver(10,0,30319,415))
		              ||  (_mfc100chs_dll >zero && _mfc100chs_dll <fver(10,0,30319,415))
		              ||  (_mfc100cht_dll >zero && _mfc100cht_dll <fver(10,0,30319,415))
		              ||  (_mfc100deu_dll >zero && _mfc100deu_dll <fver(10,0,30319,415))
		              ||  (_mfc100enu_dll >zero && _mfc100enu_dll <fver(10,0,30319,415))
		              ||  (_mfc100esn_dll >zero && _mfc100esn_dll <fver(10,0,30319,415))
		            //||  (_mfc100esp_dll >zero && _mfc100esp_dll <fver(10,0,30319,415))
		              ||  (_mfc100fra_dll >zero && _mfc100fra_dll <fver(10,0,30319,415))
		              ||  (_mfc100ita_dll >zero && _mfc100ita_dll <fver(10,0,30319,415))
		              ||  (_mfc100jpn_dll >zero && _mfc100jpn_dll <fver(10,0,30319,415))
		              ||  (_mfc100kor_dll >zero && _mfc100kor_dll <fver(10,0,30319,415))
		              ||  (_mfc100rus_dll >zero && _mfc100rus_dll <fver(10,0,30319,415))
		              ||  (_vcomp100_dll  >zero && _vcomp100_dll  <fver(10,0,30319,415)) )) {
		NN("Security Update for Microsoft Visual C++ 2010 Redistributable Package (KB2467173)");
		XX(p+"vcredist_x86_f5420aa2ac335ce211623a1aa8e16fcf547cfe9a.exe"+a3);
	}
	if( sp==3 && (sku & XP_ALL) && vs2010sp1 && (/*(_atl100_dll    >zero && _atl100_dll    <fver(10,0,40219,325))
		              ||*/  (_msdia100_dll  >zero && _msdia100_dll  <fver(10,0,40219,1))
		            //||  (_msvcm100_dll  >zero && _msvcm100_dll  <fver(10,0,40219,325))
		              ||  (_msvcp100_dll  >zero && _msvcp100_dll  <fver(10,0,40219,325))
		              ||  (_msvcr100_dll  >zero && _msvcr100_dll  <fver(10,0,40219,325))
		              ||  (_mfc100_dll    >zero && _mfc100_dll    <fver(10,0,40219,325))
		              ||  (_mfc100u_dll   >zero && _mfc100u_dll   <fver(10,0,40219,325))
		              ||  (_mfcm100_dll   >zero && _mfcm100_dll   <fver(10,0,40219,325))
		              ||  (_mfcm100u_dll  >zero && _mfcm100u_dll  <fver(10,0,40219,325))
		              ||  (_mfc100chs_dll >zero && _mfc100chs_dll <fver(10,0,40219,325))
		              ||  (_mfc100cht_dll >zero && _mfc100cht_dll <fver(10,0,40219,325))
		              ||  (_mfc100deu_dll >zero && _mfc100deu_dll <fver(10,0,40219,325))
		              ||  (_mfc100enu_dll >zero && _mfc100enu_dll <fver(10,0,40219,325))
		              ||  (_mfc100esn_dll >zero && _mfc100esn_dll <fver(10,0,40219,325))
		            //||  (_mfc100esp_dll >zero && _mfc100esp_dll <fver(10,0,40219,325))
		              ||  (_mfc100fra_dll >zero && _mfc100fra_dll <fver(10,0,40219,325))
		              ||  (_mfc100ita_dll >zero && _mfc100ita_dll <fver(10,0,40219,325))
		              ||  (_mfc100jpn_dll >zero && _mfc100jpn_dll <fver(10,0,40219,325))
		              ||  (_mfc100kor_dll >zero && _mfc100kor_dll <fver(10,0,40219,325))
		              ||  (_mfc100rus_dll >zero && _mfc100rus_dll <fver(10,0,40219,325))
		              ||  (_vcomp100_dll  >zero && _vcomp100_dll  <fver(10,0,40219,325)) )) {
		NN("Security Update for Microsoft Visual C++ 2010 Service Pack 1 Redistributable Package (KB2565063)");
		XX(p+"vcredist_x86_28c54491be70c38c97849c3d8cfbfdd0d3c515cb.exe"+a3);
	}
	if( sp==3 && (sku & XP_ALL) && (((_mfc110_dll>zero || _msvcr110_dll>zero || _mfc110u_dll>zero
		                  ||_mfcm110_dll>zero|| _mfcm110u_dll>zero || _vs2012reg>zero) && _vs2012reg <fver(11,0,61030,0))
		            //||  (_atl110_dll    >zero && _atl110_dll    <fver(11,0,60610,1))
		              ||  (_msvcp110_dll  >zero && _msvcp110_dll  <fver(11,0,51106,1))
		              ||  (_msvcr110_dll  >zero && _msvcr110_dll  <fver(11,0,51106,1))
		              ||  (_mfc110_dll  >zero && _mfc110_dll  <fver(11,0,60610,1))
		              ||  (_mfc110u_dll  >zero && _mfc110u_dll  <fver(11,0,60610,1))
		              ||  (_mfcm110_dll  >zero && _mfcm110_dll  <fver(11,0,60610,1))
		              ||  (_mfcm110u_dll  >zero && _mfcm110u_dll  <fver(11,0,60610,1))
		              ||  (_mfc110chs_dll  >zero && _mfc110chs_dll  <fver(11,0,60610,1))
		              ||  (_mfc110cht_dll  >zero && _mfc110cht_dll  <fver(11,0,60610,1))
		              ||  (_mfc110deu_dll  >zero && _mfc110deu_dll  <fver(11,0,60610,1))
		              ||  (_mfc110enu_dll  >zero && _mfc110enu_dll  <fver(11,0,60610,1))
		              ||  (_mfc110esn_dll  >zero && _mfc110esn_dll  <fver(11,0,60610,1))
		              ||  (_mfc110fra_dll  >zero && _mfc110fra_dll  <fver(11,0,60610,1))
		              ||  (_mfc110ita_dll  >zero && _mfc110ita_dll  <fver(11,0,60610,1))
		              ||  (_mfc110jpn_dll  >zero && _mfc110jpn_dll  <fver(11,0,60610,1))
		              ||  (_mfc110kor_dll  >zero && _mfc110kor_dll  <fver(11,0,60610,1))
		              ||  (_mfc110rus_dll  >zero && _mfc110rus_dll  <fver(11,0,60610,1))
		              ||  (_vcomp110_dll  >zero && _vcomp110_dll  <fver(11,0,51106,1)) )) {
		NN("Update for Microsoft Visual C++ 2012 Update 4 Redistributable Package (KB3119142)");
		XX(p+"vcredist_x86_96b377a27ac5445328cbaae210fc4f0aaa750d3f.exe"+a3);
	}
	if( sp==3 && (sku & XP_ALL) && (/*((_mfc120_dll>zero || _msvcr120_dll>zero || _mfc120u_dll>zero
		                  ||_mfcm120_dll>zero|| _mfcm120u_dll>zero || _vs2013reg>zero) && _vs2013reg <fver(12,0,40664,0))
					  ||*/  (_mfc120_dll    >zero && _mfc120_dll    <fver(12,0,40664,0))
		              ||  (_mfc120chs_dll  >zero && _mfc120chs_dll  <fver(12,0,40664,0))
		              ||  (_mfc120cht_dll  >zero && _mfc120cht_dll  <fver(12,0,40664,0))
		              ||  (_mfc120deu_dll  >zero && _mfc120deu_dll  <fver(12,0,40664,0))
		              ||  (_mfc120enu_dll  >zero && _mfc120enu_dll  <fver(12,0,40664,0))
		              ||  (_mfc120esn_dll  >zero && _mfc120esn_dll  <fver(12,0,40664,0))
		              ||  (_mfc120fra_dll  >zero && _mfc120fra_dll  <fver(12,0,40664,0))
		              ||  (_mfc120ita_dll  >zero && _mfc120ita_dll  <fver(12,0,40664,0))
		              ||  (_mfc120jpn_dll  >zero && _mfc120jpn_dll  <fver(12,0,40664,0))
		              ||  (_mfc120kor_dll  >zero && _mfc120kor_dll  <fver(12,0,40664,0))
		              ||  (_mfc120rus_dll  >zero && _mfc120rus_dll  <fver(12,0,40664,0))
		              ||  (_mfc120u_dll  >zero && _mfc120u_dll  <fver(12,0,40664,0))
		              ||  (_mfcm120_dll  >zero && _mfcm120_dll  <fver(12,0,40664,0))
		              ||  (_mfcm120u_dll  >zero && _mfcm120u_dll  <fver(12,0,40664,0))
		              ||  (_msvcp120_dll  >zero && _msvcp120_dll  <fver(12,0,40664,0))
		              ||  (_msvcr120_dll  >zero && _msvcr120_dll  <fver(12,0,40664,0))
		              ||  (_vcomp120_dll  >zero && _vcomp120_dll  <fver(12,0,40664,0)) )) {
		NN("Update for Microsoft Visual C++ 2013 Update 5 Redistributable Package (KB4032938)");
		XX(p+"vcredist_x86_0f5d66bcaf120f2d3f340e448a268fe4bbf7709d.exe"+a3);
	}
	if( sp==3 && (sku & XP_ALL) && ((_concrt140_dll       >zero && _concrt140_dll   <fver(14,0,24215,1))
		              ||  (_mfc140_dll      >zero && _mfc140_dll      <fver(14,0,24215,1))
		              ||  (_mfc140chs_dll   >zero && _mfc140chs_dll   <fver(14,0,24215,1))
		              ||  (_mfc140cht_dll   >zero && _mfc140cht_dll   <fver(14,0,24215,1))
		              ||  (_mfc140deu_dll   >zero && _mfc140deu_dll   <fver(14,0,24215,1))
		              ||  (_mfc140enu_dll   >zero && _mfc140enu_dll   <fver(14,0,24215,1))
		              ||  (_mfc140esn_dll   >zero && _mfc140esn_dll   <fver(14,0,24215,1))
		              ||  (_mfc140fra_dll   >zero && _mfc140fra_dll   <fver(14,0,24215,1))
		              ||  (_mfc140ita_dll   >zero && _mfc140ita_dll   <fver(14,0,24215,1))
		              ||  (_mfc140jpn_dll   >zero && _mfc140jpn_dll   <fver(14,0,24215,1))
		              ||  (_mfc140kor_dll   >zero && _mfc140kor_dll   <fver(14,0,24215,1))
		              ||  (_mfc140rus_dll   >zero && _mfc140rus_dll   <fver(14,0,24215,1))
		              ||  (_mfc140u_dll     >zero && _mfc140u_dll     <fver(14,0,24215,1))
		              ||  (_mfcm140_dll     >zero && _mfcm140_dll     <fver(14,0,24215,1))
		              ||  (_mfcm140u_dll    >zero && _mfcm140u_dll    <fver(14,0,24215,1))
		              ||  (_msvcp140_dll    >zero && _msvcp140_dll    <fver(14,0,24215,1))
		              ||  (_ucrtbase_dll    >zero && _ucrtbase_dll    <fver(10,0,10586,15))
		              ||  (_vcamp140_dll    >zero && _vcamp140_dll    <fver(14,0,24215,1))
		              ||  (_vccorlib140_dll >zero && _vccorlib140_dll <fver(14,0,24215,1))
		              ||  (_vcomp140_dll    >zero && _vcomp140_dll    <fver(14,0,24215,1))
		              ||  (_vcruntime140_dll>zero && _vcruntime140_dll<fver(14,0,24215,1)) )) {
		NN("Update for Microsoft Visual C++ 2015 Update 3 Redistributable Package");
		XX(p+"vcredist_x86_72211bd2e7dfc91ea7c8fac549c49c0543ba791b.exe"+a3);
	}
	if( sp==3 && (sku & XPE_2009) && _shlwapi_dll>zero && _shlwapi_dll<fver(6,0,2900,6540)) {
		NN("Security Update for WEPOS and POSReady 2009 (KB2926765)");
		XX(p+"windowsxp-kb2926765-x86-embedded-enu_fd1b4b0791354fab04ae68907436cab0c8676665.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _gdiplus_dll_23386<fver(5,2,6002,23386)) {
		NN("Security Update for WEPOS and POSReady 2009 (KB2957503)");
		XX(p+"windowsxp-kb2957503-x86-embedded-enu_34e002ae983c5a61dced1e5cb41d81e714a6cfbd.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (_gdi32_dll>zero && _gdi32_dll<fver(5,1,2600,5781))
					  ||  (_win32k_sys>zero && _win32k_sys<fver(5,1,2600,5753)) )) {
		NN("Security Update for WEPOS and POSReady 2009 (KB2993651)");
		XX(p+"windowsxp-kb2993651-x86-embedded-enu_60107e4eed4850513f41778ad3a9caef35d893b8.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (_mqac_sys>zero && _mqac_sys<fver(5,1,0,1111))
					  ||  (_mqqm_dll>zero && _mqqm_dll<fver(5,1,0,1111)) )) {
		NN("Security Update for WEPOS and POSReady 2009 (KB2993254)");
		XX(p+"windowsxp-kb2993254-x86-embedded-enu_a56e76a2cc8404722e44a938849f114431788829.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _fastfat_sys>zero && _fastfat_sys<fver(5,1,2600,6630)) {
		NN("Security Update for WEPOS and POSReady 2009 (KB2998579)");
		XX(p+"windowsxp-kb2998579-x86-embedded-enu_166f21272c3e54fa2430ca1231b65d870ac1a1bc.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _gdiplus_dll_23535<fver(5,2,6002,23535)) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3013126)");
		XX(p+"windowsxp-kb3013126-x86-embedded-enu_a7485236ec7df12c907ac232c4d8778df1936f43.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _tlntsess_exe>zero && _tlntsess_exe<fver(5,1,2600,6705)) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3020393)");
		XX(p+"windowsxp-kb3020393-v2-x86-embedded-enu_8eed034b5b679808bfb76c522cede9ab48e681e5.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _userenv_dll>zero && _userenv_dll<fver(5,1,2600,6689)) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3021674)");
		XX(p+"windowsxp-kb3021674-x86-embedded-enu_3fc15ebe581b61c1a30ab790f97d2fb2d7d41926.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _scesrv_dll>zero && _scesrv_dll<fver(5,1,2600,6690)) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3004361)");
		XX(p+"windowsxp-kb3004361-x86-embedded-enu_47e4ec000eca37d76a17a45d1f17ac181849b45b.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _gdiplus_dll_23588<fver(5,2,6002,23588)) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3029944)");
		XX(p+"windowsxp-kb3029944-x86-embedded-enu_ecdc1784751745ae36f14c6a964f1d9701baefad.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _msctf_dll>zero && _msctf_dll<fver(5,1,2600,6727)) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3033889)");
		XX(p+"windowsxp-kb3033889-x86-embedded-enu_97084ab0bd0fe1c7097c6d176d61d85cc28a3609.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _gdiplus_dll_23609<fver(5,2,6002,23609)) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3035132)"); 
		XX(p+"windowsxp-kb3035132-x86-embedded-enu_8e7607ac53706be4b440013f5c82725c2a94a6d2.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (_atmfd_dll>zero && _atmfd_dll<fver(5,1,2,245))
					  ||  ( _gdiplus_dll_23746<fver(5,2,6002,23746)) )) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3078601)");
		XX(p+"windowsxp-kb3078601-x86-embedded-enu_9f428ed1e710bced0702b4097846d36429a93202.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (_atmfd_dll>zero && _atmfd_dll<fver(5,1,2,246))
					  ||  (_win32k_sys>zero && _win32k_sys<fver(5,1,2600,6908)) )) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3087039)");
		XX(p+"windowsxp-kb3087039-x86-embedded-enu_06ac33315eaa9c1d4728cb19651e72cad6186a97.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (_tcpip6_sys>zero && _tcpip6_sys<fver(5,1,2600,6935))
					  ||  (_tcpip_sys>zero && _tcpip_sys<fver(5,1,2600,6935)) )) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3092601)");
		XX(p+"windowsxp-kb3092601-x86-embedded-enu_8ddf39236405ff94407350f62daf3ab6e0fbca65.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ((_dssenh_dll>zero && _dssenh_dll<fver(5,1,2600,6924))
		              ||  (_rsaenh_dll>zero && _rsaenh_dll<fver(5,1,2600,6924))
					  ||  (_schannel_dll>zero && _schannel_dll<fver(5,1,2600,6926)) )) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3081320)");
		XX(p+"windowsxp-kb3081320-x86-embedded-enu_d8e991e08445605d85c48425684c7850d1d63a36.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (_devenum_dll>zero && _devenum_dll<fver(6,5,2600,6971))
					  ||  (_qedit_dll>zero && _qedit_dll<fver(6,5,2600,6971)) )) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3109560)");
		XX(p+"windowsxp-kb3109560-x86-embedded-enu_ec8a3e049d5b27bbdbb091cd3ea2ffc6c725ffc5.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _advapi32_dll>zero && _advapi32_dll<fver(5,1,2600,6987)) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3121918)");
		XX(p+"windowsxp-kb3121918-x86-embedded-enu_0415f43e728e56f42bdc3171ec7d0f59221cafc7.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ((_msdaora_dll>zero && _msdaora_dll<fver(2,81,3014,0))
		              ||  (_msorcl32_dll>zero && _msorcl32_dll<fver(2,575,3014,0))
					  ||  (_mtxoci_dll>zero && _mtxoci_dll<fver(2001,12,4414,707)) )) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3126587)");
		XX(p+"windowsxp-kb3126587-x86-embedded-enu_b638c3dc6cc42839e8f73e46306791ebc7fc6709.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ((_kernel32_dll>zero && _kernel32_dll<fver(5,1,2600,7013))
		              ||  (_ntdll_dll>zero && _ntdll_dll<fver(5,1,2600,7013))
					  ||  (_ole32_dll>zero && _ole32_dll<fver(5,1,2600,7013)) )) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3126593)");
		XX(p+"windowsxp-kb3126593-x86-embedded-enu_28707de25730ebbb4d0e5f02d156dff431bc6e6d.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _seclogon_dll>zero && _seclogon_dll<fver(5,1,2600,7020)) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3139914)");
		XX(p+"windowsxp-kb3139914-x86-embedded-enu_44ee887d5f8da7d99a521b41e3c5ba9e4954de62.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _usbstor_sys>zero && _usbstor_sys<fver(5,1,2600,7012)) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3139398)");
		XX(p+"windowsxp-kb3139398-x86-embedded-enu_1a601a891b38acd2b4e92ed4695646b74b66e1d9.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ((_wabfind_dll>zero && _wabfind_dll<fver(6,0,2900,7019))
		              ||  (_wabimp_dll>zero && _wabimp_dll<fver(6,0,2900,7019))
					  ||  (_wabmig_exe>zero && _wabmig_exe<fver(6,0,2900,7019)) )) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3140709)");
		XX(p+"windowsxp-kb3140709-x86-embedded-enu_26891d26d98b012dd5b42112b7e692a5cae56cdf.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (_win32k_sys>zero && _win32k_sys<fver(5,1,2600,7051))
					  ||  ( _gdiplus_dll_23930<fver(5,2,6002,23930)) )) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3145739)");
		XX(p+"windowsxp-kb3145739-x86-embedded-enu_e958811dbe4188202eda69cdc9e797a4ae1dc923.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (_lsasrv_dll>zero && _lsasrv_dll<fver(5,1,2600,7049))
					  ||  (_samsrv_dll>zero && _samsrv_dll<fver(5,1,2600,7049)) )) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3149090)");
		XX(p+"windowsxp-kb3149090-x86-embedded-enu_ee6360c61fb2bc49278a0fcbcfb9c299af436857.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ((_kerberos_dll>zero && _kerberos_dll<fver(5,1,2600,7075))
		              ||  (_ksecdd_sys>zero && _ksecdd_sys<fver(5,1,2600,7075))
					  ||  (_msv1_0_dll>zero && _msv1_0_dll<fver(5,1,2600,7075))
					  ||  (_srv_sys>zero && _srv_sys<fver(5,1,2600,7075)) )) {
		NN("Security Update for WES09 and POSReady 2009 (KB3161561)");
		XX(p+"windowsxp-kb3161561-x86-embedded-enu_cf0ce9294706376c9c16f927ec915f5892f31213.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ((_mswsock_dll>zero && _mswsock_dll<fver(5,1,2600,7074))
		              ||  (_netbt_sys>zero && _netbt_sys<fver(5,1,2600,7074))
					  ||  (_winhttp_dll>zero && _winhttp_dll<fver(5,1,2600,7074))
					  ||  (_ws2_32_dll>zero && _ws2_32_dll<fver(5,1,2600,7074)) )) {
		NN("Security Update for WES09 and POSReady 2009 (KB3161949)");
		XX(p+"windowsxp-kb3161949-x86-embedded-enu_0973436e841ac9a6697c9439f97c7d6ab66484f6.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ((_inetpp_dll>zero && _inetpp_dll<fver(5,1,2600,7094))
		              ||  (_localspl_dll>zero && _localspl_dll<fver(5,1,2600,7094))
					  ||  (_printui_dll>zero && _printui_dll<fver(5,1,2600,7094)) )) {
		NN("Security Update for WES09 and POSReady 2009 (KB3170455)");
		XX(p+"windowsxp-kb3170455-x86-embedded-enu_c49f5af4f42870c20dc1f5bc1c848405dc8ad197.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _gdiplus_dll_23984<fver(5,2,6002,23984)) {
		NN("Security Update for WES09 and POSReady 2009 (KB3178034)");
		XX(p+"windowsxp-kb3178034-x86-embedded-enu_cb20691577ade771359e8a680e75d8f0872ff5f4.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (_user32_dll>zero && _user32_dll<fver(5,1,2600,7121))
					  ||  (_win32k_sys>zero && _win32k_sys<fver(5,1,2600,7121)) )) {
		NN("Security Update for WES09 and POSReady 2009 (KB3185911)");
		XX(p+"windowsxp-kb3185911-x86-embedded-enu_2ca7c76b25bdb5024f89b4754048d0b5f884c12e.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (_mrxdav_sys>zero && _mrxdav_sys<fver(5,1,2600,7128))
					  ||  (_mup_sys>zero && _mup_sys<fver(5,1,2600,7128)) )) {
		NN("Security Update for WES09 and POSReady 2009 (KB3183431)");
		XX(p+"windowsxp-kb3183431-x86-embedded-enu_e1867d355002ce685fbffb06f364d2f003b9afcd.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ((_asycfilt_dll>zero && _asycfilt_dll<fver(5,1,2600,7149))
		              ||  (_oleaut32_dll>zero && _oleaut32_dll<fver(5,1,2600,7149))
					  ||  (_olepro32_dll>zero && _olepro32_dll<fver(5,1,2600,7149)) )) {
		NN("Security Update for WES09 and POSReady 2009 (KB3196718)");
		XX(p+"windowsxp-kb3196718-x86-embedded-enu_6b1d637604907623f8989b3d98c9e531b122c6cd.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (_lsasrv_dll>zero && _lsasrv_dll<fver(5,1,2600,7151))
					  ||  (_msv1_0_dll>zero && _msv1_0_dll<fver(5,1,2600,7151)) )) {
		NN("Security Update for WES09 and POSReady 2009 (KB3198510)");
		XX(p+"windowsxp-kb3198510-x86-embedded-enu_237a0e55e051d8e10cd5510b89e25bf2da64d508.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ((_gdi32_dll>zero && _gdi32_dll<fver(5,1,2600,7209))
		              ||  (_usp10_dll>zero && _usp10_dll<fver(1,420,2600,7209))
					  ||  (_gdiplus_dll_24064<fver(5,2,6002,24064)) )) {
		NN("Security Update for WES09 and POSReady 2009 (KB4012583)");
		XX(p+"windowsxp-kb4012583-x86-embedded-enu_cd150712980ded7ca5765a8fdd9a9dda860877b8.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _icm32_dll>zero && _icm32_dll<fver(5,1,2600,7197)) {
		NN("Security Update for WES09 and POSReady 2009 (KB4012584)");
		XX(p+"windowsxp-kb4012584-x86-embedded-enu_04053061cb8d03ee685554dd351594fc6f0a12ab.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (_qedit_dll>zero && _qedit_dll<fver(6,5,2600,7218))
					  ||  (_quartz_dll>zero && _quartz_dll<fver(6,5,2600,7218)) )) {
		NN("Security Update for WES09 and POSReady 2009 (KB4014652)");
		XX(p+"windowsxp-kb4014652-x86-embedded-enu_c2702e4f1e63d06a320c139a853fd3a578d5e228.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _asycfilt_dll>zero && _asycfilt_dll<fver(5,1,2600,7225)) {
		NN("Security Update for WES09 and POSReady 2009 (KB4014794)");
		XX(p+"windowsxp-kb4014794-x86-embedded-enu_ffefa30f0b989917a75cd5474576c6155d17d420.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _gdiplus_dll_24072<fver(5,2,6002,24072)) {
		NN("Security Update for WES09 and POSReady 2009 (KB4015383)");
		XX(p+"windowsxp-kb4015383-x86-embedded-enu_f2891bd693ccc72eac9aba102aed25687df5a85c.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _kerberos_dll>zero && _kerberos_dll<fver(5,1,2600,7292)) {
		NN("Security Update for WES09 and POSReady 2009 (KB4022746)");
		XX(p+"windowsxp-kb4022746-x86-embedded-enu_c8cee9e23429d15d9a21529b7eaedde935b36699.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _gpkcsp_dll>zero && _gpkcsp_dll<fver(5,1,2600,7263)) {
		NN("Security Update for WES09 and POSReady 2009 (KB4022747)");
		XX(p+"windowsxp-kb4022747-x86-embedded-enu_9ee3ca3c99988ec3a9cade2073f461b39706c82c.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ((_iprtrmgr_dll>zero && _iprtrmgr_dll<fver(5,1,2600,7271))
		              ||  (_ndiswan_sys>zero && _ndiswan_sys<fver(5,1,2600,7271))
					  ||  (_rasctrs_dll>zero && _rasctrs_dll<fver(5,1,2600,7271))
					  ||  (_rasdlg_dll>zero && _rasdlg_dll<fver(5,1,2600,7271))
					  ||  (_rasmxs_dll>zero && _rasmxs_dll<fver(5,1,2600,7271)) )) {
		NN("Security Update for WES09 and POSReady 2009 (KB4024323)");
		XX(p+"windowsxp-kb4024323-x86-embedded-enu_42d69f9c232fb86657c938a507388bcdf5043483.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _wldap32_dll>zero && _wldap32_dll<fver(5,1,2600,7313)) {
		NN("Security Update for WES09 and POSReady 2009 (KB4025409)");
		XX(p+"windowsxp-kb4025409-x86-embedded-enu_3afe33a68dfc323b223c6573670bbafddcdf82f4.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _browseui_dll>zero && _browseui_dll<fver(6,0,2900,7288)) {
		NN("Security Update for WES09 and POSReady 2009 (KB4025497)");
		XX(p+"windowsxp-kb4025497-x86-embedded-enu_4a37e35189bb8bbdc561962c3ea6e500638fccf5.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _wordpad_exe>zero && _wordpad_exe<fver(5,1,2600,7297)) {
		NN("Security Update for WES09 and POSReady 2009 (KB4026061)");
		XX(p+"windowsxp-kb4026061-x86-embedded-enu_c9e64b9686dae23b9e299fd13669019f174e26b8.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (_msado15_dll>zero && _msado15_dll<fver(2,81,3015,0))
					  ||  (_msado28_tlb>zero && _msado28_tlb<fver(2,81,3015,0)) )) {
		NN("2018-02 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4034044)");
		XX(p+"windowsxp-kb4034044-x86-embedded-enu_2eed2f8234cf7d6326ece957d4309979faf9d84a.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ((_msexcl40_dll>zero && _msexcl40_dll<fver(4,0,9801,0))
		              ||  (_msjet40_dll>zero && _msjet40_dll<fver(4,0,9801,0))
					  ||  (_msjetol1_dll>zero && _msjetol1_dll<fver(4,0,9801,0))
					  ||  (_msjint40_dll>zero && _msjint40_dll<fver(4,0,9801,0))
					  ||  (_msjter40_dll>zero && _msjter40_dll<fver(4,0,9801,0))
					  ||  (_msjtes40_dll>zero && _msjtes40_dll<fver(4,0,9801,0))
					  ||  (_msltus40_dll>zero && _msltus40_dll<fver(4,0,9801,0))
					  ||  (_mspbde40_dll>zero && _mspbde40_dll<fver(4,0,9801,0))
					  ||  (_msrd2x40_dll>zero && _msrd2x40_dll<fver(4,0,9801,0))
					  ||  (_msrd3x40_dll>zero && _msrd3x40_dll<fver(4,0,9801,0))
					  ||  (_msrepl40_dll>zero && _msrepl40_dll<fver(4,0,9801,0))
					  ||  (_mstext40_dll>zero && _mstext40_dll<fver(4,0,9801,0))
					  ||  (_mswdat10_dll>zero && _mswdat10_dll<fver(4,0,9801,0))
					  ||  (_mswstr10_dll>zero && _mswstr10_dll<fver(4,0,9801,0))
					  ||  (_msxbde40_dll>zero && _msxbde40_dll<fver(4,0,9801,0)) )) {
		NN("Security Update for WES09 and POSReady 2009 (KB4034775)");
		XX(p+"windowsxp-kb4034775-x86-embedded-enu_0c5c2f6822400aaf9fb2daf450e6373be2255e75.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ((_usp10_dll>zero && _usp10_dll<fver(1,420,2600,7334))
		              ||  (_win32k_sys>zero && _win32k_sys<fver(5,1,2600,7334))
					  ||  (_gdiplus_dll_24180<fver(5,2,6002,24180)) )) {
		NN("Security Update for WES09 and POSReady 2009 (KB4039384)");
		XX(p+"windowsxp-kb4039384-x86-embedded-enu_6aabfe977264b12c8331500d97053134de7d0738.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ((_mrxsmb_sys>zero && _mrxsmb_sys<fver(5,1,2600,7348))
					  ||    (_srv_sys   >zero && _srv_sys   <fver(5,1,2600,7348)) )) {
		NN("Security Update for WES09 and POSReady 2009 (KB4041995)");
		XX(p+"windowsxp-kb4041995-x86-embedded-enu_acee429ab4e7445545ac73a97f390628e647c1cf.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _query_dll>zero && _query_dll<fver(5,1,2600,7363)) {
		NN("Security Update for WES09 and POSReady 2009 (KB4047211)");
		XX(p+"windowsxp-kb4047211-x86-embedded-enu_217f80951ee111cf78ea67a854c2919ddc94dceb.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _iprtrmgr_dll>zero && _iprtrmgr_dll<fver(5,1,2600,7377)) {
		NN("2017-12 Security Update for Windows XP Embedded SP3 for x86-based Systems (KB4052303)");
		XX(p+"windowsxp-kb4052303-x86-embedded-enu_04ab266aa0752613a9b6c68ff33d40699a0045e5.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (/*_credssp_dll>zero &&*/ _credssp_dll<fver(5,1,2600,7471))
					  ||  (/*_tspkg_dll>zero &&*/ _tspkg_dll<fver(5,1,2600,7471)) )) {
		NN("2018-05 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4056564)");
		XX(p+"windowsxp-kb4056564-v2-x86-embedded-enu_964310efb26058c35b0233f008f8b5f6ffd102af.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _atmfd_dll>zero && _atmfd_dll<fver(5,1,2,253)) {
		NN("2018-01 Security Update for Windows XP Embedded SP3 for x86-based Systems (KB4056941)");
		XX(p+"windowsxp-kb4056941-x86-embedded-enu_24ebcc41e3f03048c25cf02d8d8ab6107479aabd.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _msvidctl_dll>zero && _msvidctl_dll<fver(6,5,2600,7431)) {
		NN("2018-03 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4089082)");
		XX(p+"windowsxp-kb4089082-x86-embedded-enu_56a4c587452f40f8e91f3192168f4cf1193e801c.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _zipfldr_dll>zero && _zipfldr_dll<fver(6,0,2900,7443)) {
		NN("2018-03 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4089175)");
		XX(p+"windowsxp-kb4089175-x86-embedded-enu_1caf7f6ec0b27d38070eb0385f1548776bc65e6d.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ((_helpctr_exe>zero && _helpctr_exe<fver(5,1,2600,7431))
		              ||  (_helphost_exe>zero && _helphost_exe<fver(5,1,2600,7431))
					  ||  (_helpsvc_exe>zero && _helpsvc_exe<fver(5,1,2600,7431))
					  ||  (_pchsvc_dll>zero && _pchsvc_dll<fver(5,1,2600,7431))
					  ||  (_safrslv_dll>zero && _safrslv_dll<fver(5,1,2600,7431))
					  ||  (_srdiag_exe>zero && _srdiag_exe<fver(5,1,2600,7431))
					  ||  (_uploadm_exe>zero && _uploadm_exe<fver(5,1,2600,7431)) )) {
		NN("2018-05 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4089453)");
		XX(p+"windowsxp-kb4089453-v2-x86-embedded-enu_23b1aec8d9e41242c20799e66045dc75c089df61.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _wzcsvc_dll>zero && _wzcsvc_dll<fver(5,1,2600,7432)) {
		NN("2018-03 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4089694)");
		XX(p+"windowsxp-kb4089694-x86-embedded-enu_f0698bdbb2c0dbaf56d7c4071061046865ec3526.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _wsnmp32_dll>zero && _wsnmp32_dll<fver(5,1,2600,7450)) {
		NN("2018-04 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4091756)");
		XX(p+"windowsxp-kb4091756-x86-embedded-enu_4084e5a3bf76e0a22d8d33b0c5e8ff3b57e3faf3.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _comsvcs_dll>zero && _comsvcs_dll<fver(2001,12,4414,7477)) {
		NN("2018-05 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4101477)");
		XX(p+"windowsxp-kb4101477-x86-embedded-enu_867ee71e20e05bcdcab83b86466ab20771f74ac5.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _itss_dll>zero && _itss_dll<fver(5,2,3790,6349)) {
		NN("2018-05 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4130957)");
		XX(p+"windowsxp-kb4130957-x86-embedded-enu_dab532721ce76725b35909e6d95c9d430edc1e13.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ((_kernel32_dll>zero && _kernel32_dll<fver(5,1,2600,7494))
		              ||  (_ntdll_dll>zero && _ntdll_dll<fver(5,1,2600,7494))
					  ||  (_ole32_dll>zero && _ole32_dll<fver(5,1,2600,7494))
					  ||  (_oleaut32_dll>zero && _oleaut32_dll<fver(5,1,2600,7494))
					  ||  (_rpcss_dll>zero && _rpcss_dll<fver(5,1,2600,7494)) )) {
		NN("2018-07 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4134651)");
		XX(p+"windowsxp-kb4134651-v2-x86-embedded-enu_df9272e617574b8645fdca35b58627930c394999.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ((_afd_sys>zero && _afd_sys<fver(5,1,2600,7510))
		              ||  (_dnsapi_dll>zero && _dnsapi_dll<fver(5,1,2600,7510))
					  ||  (_dnsrslvr_dll>zero && _dnsrslvr_dll<fver(5,1,2600,7510))
					  ||  (_mswsock_dll>zero && _mswsock_dll<fver(5,1,2600,7510))
					  ||  (_tcpip6_sys>zero && _tcpip6_sys<fver(5,1,2600,7510))
					  ||  (_tcpip_sys>zero && _tcpip_sys<fver(5,1,2600,7510)) )) {
		NN("2018-07 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4291391)");
		XX(p+"windowsxp-kb4291391-x86-embedded-enu_a48bc4ba244b5a49d0191900c5f65f60e1568579.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _hidparse_sys>zero && _hidparse_sys<fver(5,1,2600,7494)) {
		NN("2018-06 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4294413)");
		XX(p+"windowsxp-kb4294413-x86-embedded-enu_b211e66c425fcef8980fd16953483282cb4e6b14.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ((_kernel32_dll>zero && _kernel32_dll<fver(5,1,2600,7512))
		              ||  (_ntdll_dll>zero && _ntdll_dll<fver(5,1,2600,7512))
					  ||  (_ole32_dll>zero && _ole32_dll<fver(5,1,2600,7512))
					  ||  (_rpcss_dll>zero && _rpcss_dll<fver(5,1,2600,7512))
					  ||  (_wordpad_exe>zero && _wordpad_exe<fver(5,1,2600,7512)) )) {
		NN("2018-07 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4339291)");
		XX(p+"windowsxp-kb4339291-x86-embedded-enu_093848f3c816e2814c7cda1baab07f00e0ac5917.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( ( _mf3216_dll>zero && _mf3216_dll<fver(5,1,2600,7535))
					  ||  ( _gdiplus_dll_24439<fver(5,2,6002,24439)) )) {
		NN("2018-08 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4343674)");
		XX(p+"windowsxp-kb4343674-x86-embedded-enu_ca27aebe8d7a210ffbc67934c144627a9d6f5463.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _shell32_dll>zero && _shell32_dll<fver(6,0,2900,7551)) {
		NN("2018-09 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4457163)");
		XX(p+"windowsxp-kb4457163-x86-embedded-enu_6079ca40413cb788b625ebc6c4b8b802fa3bca57.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (_gdi32_dll>zero && _gdi32_dll<fver(5,1,2600,7553))
		              ||  ( _mf3216_dll>zero && _mf3216_dll<fver(5,1,2600,7553))
					  ||  ( _gdiplus_dll_24465<fver(5,2,6002,24465)) )) {
		NN("2018-09 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4458000)");
		XX(p+"windowsxp-kb4458000-x86-embedded-enu_cd9e39263c1b198b9d0a22529100434d3bd20e90.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (_fontsub_dll>zero && _fontsub_dll<fver(5,1,2600,7563))
					  ||  (_t2embed_dll>zero && _t2embed_dll<fver(5,1,2600,7563)) )) {
		NN("2018-09 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4458001)");
		XX(p+"windowsxp-kb4458001-x86-embedded-enu_b6ae71b9b58128d4c6a3221208c1e42d0d434f79.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (_msxml3_dll>zero && _msxml3_dll<fver(8,100,1063,0))
					  ||  (_msxml6_dll>zero && _msxml6_dll<fver(6,20,2506,0)) )) {
		NN("2018-09 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4458003)");
		XX(p+"windowsxp-kb4458003-x86-embedded-enu_a17d87d408201c9d690d726da04d09f717bc661e.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (_msexcl40_dll>zero && _msexcl40_dll<fver(4,0,9801,4))
					  ||  (_msjet40_dll>zero && _msjet40_dll<fver(4,0,9801,4)) )) {
		NN("2018-09 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4458006)");
		XX(p+"windowsxp-kb4458006-x86-embedded-enu_e96b950633bde3636cffe6629b30f424acc90daf.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _msxml6_dll>zero && _msxml6_dll<fver(6,20,2507,0)) {
		NN("2018-10 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4458008)");
		XX(p+"windowsxp-kb4458008-x86-embedded-enu_abc9e19593fcbcf30212a77493af05b144da8bb6.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _ks_sys>zero && _ks_sys<fver(5,3,2600,7562)) {
		NN("2018-09 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4458318)");
		XX(p+"windowsxp-kb4458318-x86-embedded-enu_d2751e7d4cd301c51c010f1d193d909e64ba8466.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ((_dssenh_dll>zero && _dssenh_dll<fver(5,1,2600,7345))
		              ||  (_rsaenh_dll>zero && _rsaenh_dll<fver(5,1,2600,7345))
					  ||  (_schannel_dll>zero && _schannel_dll<fver(5,1,2600,7567)) )) {
		NN("Update for WES09 and POSReady 2009 (KB4459091)");
		XX(p+"windowsxp-kb4459091-x86-embedded-enu_6f260244fafd69643591ded6624997eb2211b627.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _mrxsmb_sys>zero && _mrxsmb_sys<fver(5,1,2600,7570)) {
		NN("2018-10 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4462935)");
		XX(p+"windowsxp-kb4462935-x86-embedded-enu_b5de399aa37284c81ca1e26ffb93893db035ec17.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _gdiplus_dll_24489<fver(5,2,6002,24489)) {
		NN("2018-10 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4462987)");
		XX(p+"windowsxp-kb4462987-x86-embedded-enu_2ca3c3b7b4e2f9bee24af4e46eeb7e4624aabd3d.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (_ntfs_sys>zero && _ntfs_sys<fver(5,1,2600,7581))
		              ||  (_ntkrnlmp_exe>zero && _ntkrnlmp_exe<fver(5,1,2600,7581))
					  ||  (_ntkrnlpa_exe>zero && _ntkrnlpa_exe<fver(5,1,2600,7581))
					  ||  (_ntkrpamp_exe>zero && _ntkrpamp_exe<fver(5,1,2600,7581))
					  ||  (_ntoskrnl_exe>zero && _ntoskrnl_exe<fver(5,1,2600,7581)) )) {
		NN("2018-10 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4463103)");
		XX(p+"windowsxp-kb4463103-x86-embedded-enu_1d7cc48a7f1902bdd5953a1ab980db5832631339.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _msrd3x40_dll>zero && _msrd3x40_dll<fver(4,0,9801,5)) {
		NN("2018-10 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4463573)");
		XX(p+"windowsxp-kb4463573-x86-embedded-enu_b0e76ad95da29d0ce8ba122a6b703660b93ed49b.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ((_kernel32_dll>zero && _kernel32_dll<fver(5,1,2600,7594))
		              ||  (_ntdll_dll>zero && _ntdll_dll<fver(5,1,2600,7594))
					  ||  (_ole32_dll>zero && _ole32_dll<fver(5,1,2600,7594))
					  ||  (_oleaut32_dll>zero && _oleaut32_dll<fver(5,1,2600,7594))
					  ||  (_rpcss_dll>zero && _rpcss_dll<fver(5,1,2600,7594)) )) {
		NN("2018-11 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4466388)");
		XX(p+"windowsxp-kb4466388-x86-embedded-enu_b177578d5bc6c95cf31dd1848a22ddbe10b9501b.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ((_kernel32_dll>zero && _kernel32_dll<fver(5,1,2600,7594))
		              ||  (_ntdll_dll>zero && _ntdll_dll<fver(5,1,2600,7594))
					  ||  (_ole32_dll>zero && _ole32_dll<fver(5,1,2600,7594))
					  ||  (_rpcrt4_dll>zero && _rpcrt4_dll<fver(5,1,2600,7594))
					  ||  (_rpcss_dll>zero && _rpcss_dll<fver(5,1,2600,7594)) )) {
		NN("2018-11 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4467689)");
		XX(p+"windowsxp-kb4467689-x86-embedded-enu_b974cbc4675474ddb38c41f7926f493cde207eee.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _tzchange_exe<fver(5,1,2600,7607)) {
		NN("Update for WES09 and POSReady 2009 (KB4468323)");
		XX(p+"windowsxp-kb4468323-x86-embedded-enu_6a46468fe73b3b793934d70768ab2c4ce11a03ab.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _scrrun_dll>zero && _scrrun_dll<fver(5,7,6002,24514)) {
		NN("2018-11 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4470118)");
		XX(p+"windowsxp-kb4470118-x86-embedded-enu_eb3fe70a186545d1745f48eda37ecf21a1d330d9.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _gdiplus_dll_24533<fver(5,2,6002,24533)) {
		NN("2018-12 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4473077)");
		XX(p+"windowsxp-kb4473077-x86-embedded-enu_2472bfb0a1921bccc30621166b26004b22203e63.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _msrd3x40_dll>zero && _msrd3x40_dll<fver(4,0,9801,7)) {
		NN("2019-01 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4481275)");
		XX(p+"windowsxp-kb4481275-x86-embedded-enu_5160cf8226e525f266f39e7279ce39e6176cece9.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ((_ntkrnlmp_exe>zero && _ntkrnlmp_exe<fver(5,1,2600,7649))
		                || !regTestKey(L"SOFTWARE\\Microsoft\\Updates\\Windows XP\\SP4\\KB4486463"))) {
		NN("2019-02 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4486463)");
		XX(p+"windowsxp-kb4486463-x86-embedded-enu_8ab6c1cbd442dbdd8667993af4ee093abf09fb41.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (_srv_sys>zero && _srv_sys<fver(5,1,2600,7651))
					  /*||  ( _xpsp4res_dll<fver(5,1,2600,7651))*/ )) {
		NN("2019-02 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4486465)");
		XX(p+"windowsxp-kb4486465-x86-embedded-enu_80c4f32abd257968871a840fd4d87cfedd0b3d4b.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _npfs_sys>zero && _npfs_sys<fver(5,1,2600,7661)) {
		NN("2019-03 Security Update for POSReady 2009 for x86-based Systems (KB4486468)");
		XX(p+"windowsxp-kb4486468-x86-embedded-enu_491ec2dcd578becacf2ec08f65b6c6f62ca3822c.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _msv1_0_dll>zero && _msv1_0_dll<fver(5,1,2600,7666)) {
		NN("2019-03 Security Update for POSReady 2009 for x86-based Systems (KB4486536)");
		XX(p+"windowsxp-kb4486536-x86-embedded-enu_4dd89d4cfab9ba6526747646e320adf407b6ece4.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _kerberos_dll>zero && _kerberos_dll<fver(5,1,2600,7663)) {
		NN("2019-03 Security Update for POSReady 2009 for x86-based Systems (KB4486538)");
		XX(p+"windowsxp-kb4486538-x86-embedded-enu_b46b867293bb21b12fe5867ff98806a98ae50fa2.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _win32k_sys>zero && _win32k_sys<fver(5,1,2600,7649)) {
		NN("2019-02 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4486924)");
		XX(p+"windowsxp-kb4486924-x86-embedded-enu_49ea7629701a23dd3dbf3c21e60af54eec3baf19.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (_mf3216_dll>zero && _mf3216_dll<fver(5,1,2600,7663))
					  ||  (_gdiplus_dll_24561<fver(5,2,6002,24561)) )) {
		NN("Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4487085)");
		XX(p+"windowsxp-kb4487085-v2-x86-embedded-enu_667f051ffe98ff99495e9b6ede2b8c321aba1ca3.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _hidparse_sys>zero && _hidparse_sys<fver(5,1,2600,7640)) {
		NN("2019-02 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4487086)");
		XX(p+"windowsxp-kb4487086-x86-embedded-enu_d87dffd2310c8df09892de204e6216bb447ad0db.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _msjet40_dll>zero && _msjet40_dll<fver(4,0,9801,8)) {
		NN("2019-02 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4487385)");
		XX(p+"windowsxp-kb4487385-x86-embedded-enu_2556bb7bd7217d6295d877c731c07acd62c9c59c.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _tzchange_exe<fver(5,1,2600,7654)) {
		NN("2019-03 Update for POSReady 2009 for x86-based Systems (KB4487989)");
		XX(p+"windowsxp-kb4487989-x86-embedded-enu_3c32c4f284fcd7208b3cf4428320e95a27513c3a.exe"+a1);
	}
	if( sp==3 && (sku & XPE_POSREADY2009) && _tzchange_exe<fver(5,1,2600,7676)) {
		NN("2019-04 Update for POSReady 2009 for x86-based Systems (KB4487990)");
		XX(p+"windowsxp-kb4487990-x86-embedded-enu_8a1f84ccfc4cdcb9536864fa91ae82ef62cb3e79.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && (_msfs_sys>zero && _msfs_sys<fver(5,1,2600,7657))) {
		NN("2019-03 Security Update for POSReady 2009 for x86-based Systems (KB4489493)");
		XX(p+"windowsxp-kb4489493-x86-embedded-enu_bf5786db231541e562a8f4dececac9b8fdc3cdce.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (_msado15_dll>zero && _msado15_dll<fver(2,81,3016,0))
					  ||  (_msado28_tlb>zero && _msado28_tlb<fver(2,81,3016,0)) )) {
		NN("2019-03 Security Update for POSReady 2009 for x86-based Systems (KB4489973)");
		XX(p+"windowsxp-kb4489973-x86-embedded-enu_1c50f081b27c440d89300b59cd5df9ed689390b0.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (_msxml3_dll>zero && _msxml3_dll<fver(8,100,1064,0))
					  ||  (_msxml6_dll>zero && _msxml6_dll<fver(6,20,2508,0)) )) {
		NN("2019-03 Security Update for POSReady 2009 for x86-based Systems (KB4489974)");
		XX(p+"windowsxp-kb4489974-x86-embedded-enu_fb9d178e51321bf47246a752c2d79dcfe6bada40.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && (_msrd2x40_dll>zero && _msrd2x40_dll<fver(4,0,9801,12))) {
		NN("2019-03 Security Update for POSReady 2009 for x86-based Systems (KB4489977)");
		XX(p+"windowsxp-kb4489977-x86-embedded-enu_e1b835d2c684f2953946a414fddc7fe40ef6eec2.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _comctl32_dll_7660<fver(6,0,2900,7660)) {
		NN("2019-03 Security Update for POSReady 2009 for x86-based Systems (KB4490228)");
		XX(p+"windowsxp-kb4490228-x86-embedded-enu_a81c03f3f775390169d890d6f99cd9d3e58cc62e.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( ( _mf3216_dll>zero && _mf3216_dll<fver(5,1,2600,7666))
					  ||  ( _gdiplus_dll_24563<fver(5,2,6002,24563)) )) {
		NN("2019-03 Security Update for POSReady 2009 for x86-based Systems (KB4490385)");
		XX(p+"windowsxp-kb4490385-x86-embedded-enu_47b193ff117da1c86efc4d275b52a829c5cb00b9.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ((_localspl_dll>zero && _localspl_dll<fver(5,1,2600,7661))
		              ||  (_win32spl_dll>zero && _win32spl_dll<fver(5,1,2600,7661))
					  ||  (_winspool_drv>zero && _winspool_drv<fver(5,1,2600,7661)) )) {
		NN("2019-03 Security Update for POSReady 2009 for x86-based Systems (KB4490501)");
		XX(p+"windowsxp-kb4490501-x86-embedded-enu_c0b31474df5c52c3f1794914050fdf27b0692c65.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && (_netbt_sys>zero && _netbt_sys<fver(5,1,2600,7675))) {
		NN("2019-04 Security Update for POSReady 2009 for x86-based Systems (KB4491443)");
		XX(p+"windowsxp-kb4491443-x86-embedded-enu_38dcb7ca4adee2236e202651b1a9feff15123fc5.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && (_hidparse_sys>zero && _hidparse_sys<fver(5,1,2600,7667))) {
		NN("2019-03 Security Update for POSReady 2009 for x86-based Systems (KB4493341)");
		XX(p+"windowsxp-kb4493341-x86-embedded-enu_cfd32e1875c216557e94e1763fbc114c216d6b0c.exe"+a1);
	}
	if( sp==3 && (sku & XPE_POSREADY2009) && ((_kernel32_dll>zero && _kernel32_dll<fver(5,1,2600,7682))
		              ||  (_ntdll_dll>zero && _ntdll_dll<fver(5,1,2600,7682))
					  ||  (_ole32_dll>zero && _ole32_dll<fver(5,1,2600,7682))
					  ||  (_oleaut32_dll>zero && _oleaut32_dll<fver(5,1,2600,7682))
					  ||  (_rpcss_dll>zero && _rpcss_dll<fver(5,1,2600,7682)) )) {
		// The version of ntdll.dll installed by KB4493563 has an alleged bug in non-English languages
		// that causes error messages to display improperly?
		NN("2019-04 Security Update for POSReady 2009 for x86-based Systems (KB4493563)");
		XX(p+"windowsxp-kb4493563-x86-embedded-enu_b7acbe01322ca903c920fa8a85328057ba2d9703.exe"+a1);
	}
	if( sp==3 && (sku & XPE_POSREADY2009) && ( (_msxml3_dll>zero && _msxml3_dll<fver(8,100,1065,0))
					  ||  (_msxml6_dll>zero && _msxml6_dll<fver(6,20,2509,0)) )) {
		NN("2019-04 Security Update for POSReady 2009 for x86-based Systems (KB4493790)");
		XX(p+"windowsxp-kb4493790-x86-embedded-enu_e078da675e389cbfaba2354eae8040e438a42476.exe"+a1);
	}
	if( sp==3 && (sku & XPE_POSREADY2009) && (_msxbde40_dll>zero && _msxbde40_dll<fver(4,0,9801,14))) {
		NN("2019-04 Security Update for POSReady 2009 for x86-based Systems (KB4493793)");
		XX(p+"windowsxp-kb4493793-x86-embedded-enu_c238b0eab410f55d9b8fdc7485eba2ba0459559a.exe"+a1);
	}
	if( sp==3 && (sku & XPE_POSREADY2009) && (_mspbde40_dll>zero && _mspbde40_dll<fver(4,0,9801,14))) {
		NN("2019-04 Security Update for POSReady 2009 for x86-based Systems (KB4493794)");
		XX(p+"windowsxp-kb4493794-x86-embedded-enu_89c4913943e563851bd4c458f222aee21edb7bb3.exe"+a1);
	}
	if( sp==3 && (sku & XPE_POSREADY2009) && (_msexcl40_dll>zero && _msexcl40_dll<fver(4,0,9801,14))) {
		NN("2019-04 Security Update for POSReady 2009 for x86-based Systems (KB4493795)");
		XX(p+"windowsxp-kb4493795-x86-embedded-enu_9a084800b33b40776da30e70293cccfe4b0342f5.exe"+a1);
	}
	if( sp==3 && (sku & XPE_POSREADY2009) && (_msjet40_dll>zero && _msjet40_dll<fver(4,0,9801,14))) {
		NN("2019-04 Security Update for POSReady 2009 for x86-based Systems (KB4493796)");
		XX(p+"windowsxp-kb4493796-x86-embedded-enu_92495fc42b0bf0fd51baa584af55f02b7d8deaa5.exe"+a1);
	}
	if( sp==3 && (sku & XPE_POSREADY2009) && (_msrd3x40_dll>zero && _msrd3x40_dll<fver(4,0,9801,14))) {
		NN("2019-04 Security Update for POSReady 2009 for x86-based Systems (KB4493797)");
		XX(p+"windowsxp-kb4493797-x86-embedded-enu_40d780178b80a6bd6b4398ca1ca3e47973997b0b.exe"+a1);
	}
	if( sp==3 && (sku & XPE_POSREADY2009) && (_win32k_sys>zero && _win32k_sys<fver(5,1,2600,7684))) {
		NN("2019-04 Security Update for POSReady 2009 for x86-based Systems (KB4493927)");
		XX(p+"windowsxp-kb4493927-x86-embedded-enu_4123fcdc99a7976cd5791f78536a0585e0264462.exe"+a1);
	}
	if( sp==3 && (sku & XPE_POSREADY2009) && (_oleprn_dll>zero && _oleprn_dll<fver(5,1,2600,7680))) {
		NN("2019-04 Security Update for POSReady 2009 for x86-based Systems (KB4494059)");
		XX(p+"windowsxp-kb4494059-x86-embedded-enu_58109e2522cd4e5b41840ce5d24931d52e421f72.exe"+a1);
	}
	if( sp==3 && (sku & XPE_POSREADY2009) && ( (_mf3216_dll>zero && _mf3216_dll<fver(5,1,2600,7683))
					  ||  (_gdiplus_dll_20488<fver(5,2,6003,20488)) )) {
		NN("2019-04 Security Update for POSReady 2009 for x86-based Systems (KB4495022)");
		XX(p+"windowsxp-kb4495022-x86-embedded-enu_6fce82c569aa5a6af6ff63769b2e901032d401ee.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _gdiplus_dll_23693<fver(5,2,6002,23693)) {
		NN("Update for WEPOS and POSReady 2009 (KB3065979)");
		XX(p+"windowsxp-kb3065979-x86-embedded-enu_fe0a4fbb1fcb7c6062ae3ff88dcbf388f1ec4b07.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ((_dssenh_dll>zero && _dssenh_dll<fver(5,1,2600,7345))
		              ||  (_ksecdd_sys>zero && _ksecdd_sys<fver(5,1,2600,7346))
					  ||  (_lsasrv_dll>zero && _lsasrv_dll<fver(5,1,2600,7346))
					  ||  (_rsaenh_dll>zero && _rsaenh_dll<fver(5,1,2600,7345))
					  ||  (_schannel_dll>zero && _schannel_dll<fver(5,1,2600,7346))
					  ||  (_secur32_dll>zero && _secur32_dll<fver(5,1,2600,7346)) )) {	
		NN("Update for WES09 and POSReady 2009 (KB4019276)");
		XX(p+"windowsxp-kb4019276-x86-embedded-enu_3822fc1692076429a7dc051b00213d5e1240ce3d.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _winhttp_dll>zero && _winhttp_dll<fver(5,1,2600,7587)) {
		NN("Update for WES09 and POSReady 2009 (KB4467770)");
		XX(p+"windowsxp-kb4467770-x86-embedded-enu_f61e16be7e32887953b29ae1b8ba90064f3ef0a5.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _msrd2x40_dll>zero && _msrd2x40_dll<fver(4,0,9801,8)) {
		NN("2019-02 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4487396)");
		XX(p+"windowsxp-kb4487396-x86-embedded-enu_ab24ff1d4e7592d3642accd2557bf48c1ee4e4e2.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _termdd_sys>zero && _termdd_sys<fver(5,1,2600,7701)) {
		NN("2019-05 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4500331)");
		XX(p+"windowsxp-kb4500331-x86-embedded-enu_8aee1065d848661cbbf5a5b5b21d94803247741e.exe"+a1);
	}
	if( sp==3 && (sku & XP_CLIENT) && _httpext_dll>zero && _httpext_dll<fver(6,0,2600,7150)) {
		NN("Security Update for Windows XP Service Pack 3 (KB3197835)");
		XX(p+"windowsxp-kb3197835-x86-custom-enu_a65f07e2c62156b47d3940873c516b05ad775086.exe"+a1);
	}
	if( sp==3 && (sku & XP_CLIENT) && ((_gdi32_dll>zero && _gdi32_dll<fver(5,1,2600,7209))
		              ||  (_usp10_dll>zero && _usp10_dll<fver(1,420,2600,7209))
					  ||  (_gdiplus_dll_24064<fver(5,2,6002,24064)) )) {
		NN("Security Update for Windows XP Service Pack 3 (KB4012583)");
		XX(p+"windowsxp-kb4012583-x86-custom-enu_36fa3aa9ed1507506cc79e6bb2e4d0fd774dd941.exe"+a1);
	}
	if( sp==3 && (sku & XP_CLIENT) && _srv_sys>zero && _srv_sys<fver(5,1,2600,7208)) {
		NN("Security Update for Windows XP SP3 (KB4012598)");
		XX(p+"windowsxp-kb4012598-x86-custom-enu_eceb7d5023bbb23c0dc633e46b9c2f14fa6ee9dd.exe"+a1);
	}
	if( sp==3 && (sku & XP_CLIENT) && _srv_sys>zero && _srv_sys<fver(5,1,2600,7238)) {
		NN("Security Update for Windows XP SP3 (KB4018466)");
		XX(p+"windowsxp-kb4018466-x86-custom-enu_97472f796e84cd9869c02c32178886115a77daf6.exe"+a1);
	}
	if( sp==3 && (sku & XP_CLIENT) && _win32k_sys>zero && _win32k_sys<fver(5,1,2600,7258)) {
		NN("Security Update for Windows XP SP3 (KB4019204)");
		XX(p+"windowsxp-kb4019204-x86-custom-enu_4498d4a6c2faf1232afa6595b9a0bdb9e39526c5.exe"+a1);
	}
	if( sp==3 && (sku & XP_CLIENT) && _gpkcsp_dll>zero && _gpkcsp_dll<fver(5,1,2600,7264)) {
		NN("Security Update for Windows XP SP3 (KB4022747)");
		XX(p+"windowsxp-kb4022747-x86-custom-enu_79307511c6d11bf713a33e53683b594c8876e78e.exe"+a1);
	}
	if( sp==3 && (sku & XP_CLIENT) && (
		                  (_iprtrmgr_dll >zero && _iprtrmgr_dll <fver(5,1,2600,7272))
		              ||  (_ndiswan_sys  >zero && _ndiswan_sys  <fver(5,1,2600,7272))
					  ||  (_rasctrs_dll  >zero && _rasctrs_dll  <fver(5,1,2600,7272))
					  ||  (_rasdlg_dll   >zero && _rasdlg_dll   <fver(5,1,2600,7272))
					  ||  (_rasmxs_dll   >zero && _rasmxs_dll   <fver(5,1,2600,7272)) )) {	
		NN("Security Update for Windows XP SP3 (KB4024323)");
		XX(p+"windowsxp-kb4024323-x86-custom-enu_edb5a850bcbcc640c2b8c8aad3ab4d36a3b6fb26.exe"+a1);
	}
	if( sp==3 && (sku & XP_CLIENT) && _query_dll>zero && _query_dll<fver(5,1,2600,7273)) {
		NN("Security Update for Windows XP SP3 (KB4024402)");
		XX(p+"windowsxp-kb4024402-x86-custom-enu_c2334a5c8087e107fe26570650966a8f42602b0a.exe"+a1);
	}
	if( sp==3 && (sku & XP_CLIENT) && _olecnv32_dll>zero && _olecnv32_dll<fver(5,1,2600,7285)) {
		NN("Security Update for Windows XP SP3 (KB4025218)");
		XX(p+"windowsxp-kb4025218-x86-custom-enu_f72f48762b3c7354919c86f10d3139b65ddc8a26.exe"+a1);
	}
	if( sp==3 && (sku & XP_PLUS_FLP) && _termdd_sys>zero && _termdd_sys<fver(5,1,2600,7701)) {
		NN("Security Update for Windows XP SP3 (KB4500331)");
		XX(p+"windowsxp-kb4500331-x86-custom-enu_d7206aca53552fececf72a3dee93eb2da0421188.exe"+a1);
	}


	if( sp>=2 &&  (*wga || ( (sku & XP_ALL)
		    && ( _LegitCheckControl_dll>zero && _LegitCheckControl_dll<fver(1,9,40,0))
			&& ( _WgaLogon_dll>zero          && _WgaLogon_dll         <fver(1,9,40,0))
			&& ( _WgaTray_exe>zero           && _WgaTray_exe          <fver(1,9,40,0)) ))) {
		NN("Windows Genuine Advantage Notification (KB905474)");
		XX(p+"WindowsXP-KB905474-ENU-x86.exe /extract:%TEMP%\\KB905474 /quiet\n"
		    +"pushd %TEMP%\\KB905474\n"
		    //+"wgasetup.exe /autoauto /quiet /norestart\n"
		    +"wganotifypackageinner.exe"+a1+"\n"
		    +"REG ADD \"HKLM\\SOFTWARE\\Microsoft\\Windows Genuine Advantage\" /v InstallID /t REG_SZ /d {30F230D0-7940-499e-8A01-748682288985} /f\n"
		    +"popd\n"
		    +"rd /S /Q %TEMP%\\KB905474");
	}
	else if( sp>=2 && ( _LegitCheckControl_dll<fver(1,7,69,2))) {
		NN("Windows Genuine Advantage Validation Tool (KB892130)");
		XX(p+"WindowsXP-KB892130-x86-ENU.exe"+a1);
	}
	if( sp>=2 && (sku & XP_ALL) && _mrt_exe != fver(5,39,12900,0)) {
		NN("Windows Malicious Software Removal Tool - August 2016 (KB890830)");
		XX(p+"Windows-KB890830-v5.39.exe"+a2);
	}
	if(sp==3 && (sku & XP_PLUS_FLP) && (*xpeos || (_xp_eos_exe>zero && _xp_eos_exe<fver(5,1,2600,6526)))){
		NN("Windows XP End of Support Notification (KB2934207)");
		XX(p+"WindowsXP-KB2934207-x86-ENU.exe"+a1);
	}
	if(*rktools) {
		NN("Windows Server 2003 Resource Kit Tools");
		XX("Extras\\rktools.exe /R:N /C:\"msiexec /i rktools.msi /passive\"");
	}

	// Internet Explorer Updates
	if( sp==2 && (sku & XP_ALL) && (_iexplore_exe>=fver(6,0,0,0) && _iexplore_exe<fver(7,0,0,0)) && (
		   (_browseui_dll >zero && _browseui_dll <fver(6,0,2900,3314))
	    || (_cdfview_dll  >zero && _cdfview_dll  <fver(6,0,2900,3314))
		|| (_danim_dll    >zero && _danim_dll    <fver(6,3,1,148))
		|| (_dxtmsft_dll  >zero && _dxtmsft_dll  <fver(6,3,2900,3314))
		|| (_dxtrans_dll  >zero && _dxtrans_dll  <fver(6,3,2900,3314))
		|| (_extmgr_dll   >zero && _extmgr_dll   <fver(6,0,2900,3314))
		|| (_iedw_exe     >zero && _iedw_exe     <fver(5,1,2600,3314))
		|| (_iepeers_dll  >zero && _iepeers_dll  <fver(6,0,2900,3314))
		|| (_inseng_dll   >zero && _inseng_dll   <fver(6,0,2900,3314))
		|| (_jsproxy_dll  >zero && _jsproxy_dll  <fver(6,0,2900,3314))
		|| (_mshtml_dll   >zero && _mshtml_dll   <fver(6,0,2900,3314))
		|| (_mshtmled_dll >zero && _mshtmled_dll <fver(6,0,2900,3314))
		|| (_msrating_dll >zero && _msrating_dll <fver(6,0,2900,3314))
		|| (_mstime_dll   >zero && _mstime_dll   <fver(6,0,2900,3314))
		|| (_pngfilt_dll  >zero && _pngfilt_dll  <fver(6,0,2900,3314))
		|| (_shdocvw_dll  >zero && _shdocvw_dll  <fver(6,0,2900,3314))
		|| (_shlwapi_dll  >zero && _shlwapi_dll  <fver(6,0,2900,3314))
		|| (_urlmon_dll   >zero && _urlmon_dll   <fver(6,0,2900,3314))
		|| (_wininet_dll  >zero && _wininet_dll  <fver(6,0,2900,3314)) )) {
		NN("Cumulative Security Update for Internet Explorer for Windows XP Service Pack 2 (KB947864)");
		XX(p+"WindowsXP-KB947864-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && (_iexplore_exe>=fver(6,0,0,0) && _iexplore_exe<fver(7,0,0,0)) && (
		   (_browseui_dll >zero && _browseui_dll <fver(6,0,2900,6452))
		|| (_html_iec     >zero && _html_iec     <fver(2011,1,31,10))
		|| (_ieencode_dll >zero && _ieencode_dll <fver(2011,1,31,10))
		|| (_iepeers_dll  >zero && _iepeers_dll  <fver(6,0,2900,6452))
		|| (_mshtml_dll   >zero && _mshtml_dll   <fver(6,0,2900,6452))
		|| (_mshtmled_dll >zero && _mshtmled_dll <fver(6,0,2900,6452))
		|| (_mstime_dll   >zero && _mstime_dll   <fver(6,0,2900,6452))
		|| (_shdocvw_dll  >zero && _shdocvw_dll  <fver(6,0,2900,6452))
		|| (_tdc_ocx      >zero && _tdc_ocx      <fver(1,3,0,3131))
		|| (_url_dll      >zero && _url_dll      <fver(6,0,2900,6452))
		|| (_urlmon_dll   >zero && _urlmon_dll   <fver(6,0,2900,6452))
		|| (_vgx_dll      >zero && _vgx_dll      <fver(6,0,2900,6452))
		|| (_wininet_dll  >zero && _wininet_dll  <fver(6,0,2900,6452)) )) {
		NN("Cumulative Security Update for Internet Explorer 6 for Windows XP (KB2879017)");
		XX(p+"windowsxp-kb2879017-x86-enu_e6b354acd732c385254babb1a6a61568bf185b65.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && (_iexplore_exe>=fver(7,0,5730,11) && _iexplore_exe<fver(7,1,0,0)) && (
		/* GDR */
		   (_advpack_dll     >zero && _advpack_dll     <fver(7,0,6000,16640))
		|| (_dxtmsft_dll     >zero && _dxtmsft_dll     <fver(7,0,6000,16640))
		|| (_dxtrans_dll     >zero && _dxtrans_dll     <fver(7,0,6000,16640))
		|| (_extmgr_dll      >zero && _extmgr_dll      <fver(7,0,6000,16640))
		|| (_icardie_dll     >zero && _icardie_dll     <fver(7,0,6000,16640))
		|| (_ie4uinit_exe    >zero && _ie4uinit_exe    <fver(7,0,6000,16640))
		|| (_ieakeng_dll     >zero && _ieakeng_dll     <fver(7,0,6000,16640))
		|| (_ieaksie_dll     >zero && _ieaksie_dll     <fver(7,0,6000,16640))
		|| (_ieakui_dll      >zero && _ieakui_dll      <fver(7,0,6000,16640))
		|| (_ieapfltr_dat    >zero && _ieapfltr_dat    <fver(7,0,6011,0))
		|| (_ieapfltr_dll    >zero && _ieapfltr_dll    <fver(7,0,6000,16461))
		|| (_iedkcs32_dll    >zero && _iedkcs32_dll    <fver(17,0,6000,16640))
		|| (_ieframe_dll     >zero && _ieframe_dll     <fver(7,0,6000,16640))
		|| (_ieframe_dll_mui >zero && _ieframe_dll_mui <fver(7,0,6000,16414))
		|| (_iernonce_dll    >zero && _iernonce_dll    <fver(7,0,6000,16640))
		|| (_iertutil_dll    >zero && _iertutil_dll    <fver(7,0,6000,16640))
		|| (_ieudinit_exe    >zero && _ieudinit_exe    <fver(7,0,6000,16640))
		|| (_iexplore_exe    >zero && _iexplore_exe    <fver(7,0,6000,16640))
		|| (_inetcpl_cpl     >zero && _inetcpl_cpl     <fver(7,0,6000,16640))
		|| (_jsproxy_dll     >zero && _jsproxy_dll     <fver(7,0,6000,16640))
		|| (_msfeeds_dll     >zero && _msfeeds_dll     <fver(7,0,6000,16640))
		|| (_msfeedsbs_dll   >zero && _msfeedsbs_dll   <fver(7,0,6000,16640))
		|| (_mshtml_dll      >zero && _mshtml_dll      <fver(7,0,6000,16640))
		|| (_mshtmled_dll    >zero && _mshtmled_dll    <fver(7,0,6000,16640))
		|| (_msrating_dll    >zero && _msrating_dll    <fver(7,0,6000,16640))
		|| (_mstime_dll      >zero && _mstime_dll      <fver(7,0,6000,16640))
		|| (_occache_dll     >zero && _occache_dll     <fver(7,0,6000,16640))
		|| (_pngfilt_dll     >zero && _pngfilt_dll     <fver(7,0,6000,16640))
		|| (_url_dll         >zero && _url_dll         <fver(7,0,6000,16640))
		|| (_urlmon_dll      >zero && _urlmon_dll      <fver(7,0,6000,16640))
		|| (_webcheck_dll    >zero && _webcheck_dll    <fver(7,0,6000,16640))
		|| (_wininet_dll     >zero && _wininet_dll     <fver(7,0,6000,16640))
		/* QFE */
		|| (_advpack_dll     >fver(7,0,6000,16640) && _advpack_dll     <fver(7,0,6000,20772))
		|| (_dxtmsft_dll     >fver(7,0,6000,16640) && _dxtmsft_dll     <fver(7,0,6000,20772))
		|| (_dxtrans_dll     >fver(7,0,6000,16640) && _dxtrans_dll     <fver(7,0,6000,20772))
		|| (_extmgr_dll      >fver(7,0,6000,16640) && _extmgr_dll      <fver(7,0,6000,20772))
		|| (_icardie_dll     >fver(7,0,6000,16640) && _icardie_dll     <fver(7,0,6000,20772))
		|| (_ie4uinit_exe    >fver(7,0,6000,16640) && _ie4uinit_exe    <fver(7,0,6000,20772))
		|| (_ieakeng_dll     >fver(7,0,6000,16640) && _ieakeng_dll     <fver(7,0,6000,20772))
		|| (_ieaksie_dll     >fver(7,0,6000,16640) && _ieaksie_dll     <fver(7,0,6000,20772))
		|| (_ieakui_dll      >fver(7,0,6000,16640) && _ieakui_dll      <fver(7,0,6000,20772))
		|| (_iedkcs32_dll    >fver(17,0,6000,16640)&& _iedkcs32_dll    <fver(17,0,6000,20772))
		|| (_ieframe_dll     >fver(7,0,6000,16640) && _ieframe_dll     <fver(7,0,6000,20772))
		|| (_iernonce_dll    >fver(7,0,6000,16640) && _iernonce_dll    <fver(7,0,6000,20772))
		|| (_iertutil_dll    >fver(7,0,6000,16640) && _iertutil_dll    <fver(7,0,6000,20772))
		|| (_ieudinit_exe    >fver(7,0,6000,16640) && _ieudinit_exe    <fver(7,0,6000,20772))
		|| (_iexplore_exe    >fver(7,0,6000,16640) && _iexplore_exe    <fver(7,0,6000,20772))
		|| (_inetcpl_cpl     >fver(7,0,6000,16640) && _inetcpl_cpl     <fver(7,0,6000,20772))
		|| (_jsproxy_dll     >fver(7,0,6000,16640) && _jsproxy_dll     <fver(7,0,6000,20772))
		|| (_msfeeds_dll     >fver(7,0,6000,16640) && _msfeeds_dll     <fver(7,0,6000,20772))
		|| (_msfeedsbs_dll   >fver(7,0,6000,16640) && _msfeedsbs_dll   <fver(7,0,6000,20772))
		|| (_mshtml_dll      >fver(7,0,6000,16640) && _mshtml_dll      <fver(7,0,6000,20772))
		|| (_mshtmled_dll    >fver(7,0,6000,16640) && _mshtmled_dll    <fver(7,0,6000,20772))
		|| (_msrating_dll    >fver(7,0,6000,16640) && _msrating_dll    <fver(7,0,6000,20772))
		|| (_mstime_dll      >fver(7,0,6000,16640) && _mstime_dll      <fver(7,0,6000,20772))
		|| (_occache_dll     >fver(7,0,6000,16640) && _occache_dll     <fver(7,0,6000,20772))
		|| (_pngfilt_dll     >fver(7,0,6000,16640) && _pngfilt_dll     <fver(7,0,6000,20772))
		|| (_url_dll         >fver(7,0,6000,16640) && _url_dll         <fver(7,0,6000,20772))
		|| (_urlmon_dll      >fver(7,0,6000,16640) && _urlmon_dll      <fver(7,0,6000,20772))
		|| (_webcheck_dll    >fver(7,0,6000,16640) && _webcheck_dll    <fver(7,0,6000,20772))
		|| (_wininet_dll     >fver(7,0,6000,16640) && _wininet_dll     <fver(7,0,6000,20772)))) {
		NN("Cumulative Security Update for Internet Explorer 7 for Windows XP (KB947864)");
		XX(p+"IE7-WindowsXP-KB947864-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && (_iexplore_exe>=fver(7,0,5730,11) && _iexplore_exe<fver(7,1,0,0)) && (
		   (_advpack_dll>zero && _advpack_dll<fver(7,0,6000,21357))
	    || (_corpol_dll>zero && _corpol_dll<fver(2007,0,0,21306))
		|| (_dxtmsft_dll>zero && _dxtmsft_dll<fver(7,0,6000,21357))
		|| (_dxtrans_dll>zero && _dxtrans_dll<fver(7,0,6000,21357))
		|| (_extmgr_dll>zero && _extmgr_dll<fver(7,0,6000,21357))
		|| (_html_iec>zero && _html_iec<fver(2017,0,0,21306))
		|| (_icardie_dll>zero && _icardie_dll<fver(7,0,6000,21357))
		|| (_ie4uinit_exe>zero && _ie4uinit_exe<fver(7,0,6000,21357))
		|| (_ieakeng_dll>zero && _ieakeng_dll<fver(7,0,6000,21357))
		|| (_ieaksie_dll>zero && _ieaksie_dll<fver(7,0,6000,21357))
		|| (_ieakui_dll>zero && _ieakui_dll<fver(7,0,6000,21357))
		|| (_ieapfltr_dat>zero && _ieapfltr_dat<fver(7,0,6011,1))
		|| (_ieapfltr_dll>zero && _ieapfltr_dll<fver(7,0,6000,16730))
		|| (_iedkcs32_dll>zero && _iedkcs32_dll<fver(17,0,6000,21357))
		|| (_ieencode_dll>zero && _ieencode_dll<fver(2017,0,0,21306))
		|| (_ieframe_dll>zero && _ieframe_dll<fver(7,0,6000,21357))
		|| (_ieframe_dll_mui>zero && _ieframe_dll_mui<fver(7,0,6000,16414))
		|| (_iepeers_dll>zero && _iepeers_dll<fver(7,0,6000,21357))
		|| (_iernonce_dll>zero && _iernonce_dll<fver(7,0,6000,21357))
		|| (_iertutil_dll>zero && _iertutil_dll<fver(7,0,6000,21357))
		|| (_ieudinit_exe>zero && _ieudinit_exe<fver(7,0,6000,21357))
		|| (_iexplore_exe>zero && _iexplore_exe<fver(7,0,6000,21357))
		|| (_inetcpl_cpl>zero && _inetcpl_cpl<fver(7,0,6000,21357))
		|| (_jsproxy_dll>zero && _jsproxy_dll<fver(7,0,6000,21357))
		|| (_msfeeds_dll>zero && _msfeeds_dll<fver(7,0,6000,21357))
		|| (_msfeedsbs_dll>zero && _msfeedsbs_dll<fver(7,0,6000,21357))
		|| (_mshtml_dll>zero && _mshtml_dll<fver(7,0,6000,21357))
		|| (_mshtmled_dll>zero && _mshtmled_dll<fver(7,0,6000,21357))
		|| (_msrating_dll>zero && _msrating_dll<fver(7,0,6000,21357))
		|| (_mstime_dll>zero && _mstime_dll<fver(7,0,6000,21357))
		|| (_occache_dll>zero && _occache_dll<fver(7,0,6000,21357))
		|| (_pngfilt_dll>zero && _pngfilt_dll<fver(7,0,6000,21357))
		|| (_url_dll>zero && _url_dll<fver(7,0,6000,21357))
		|| (_urlmon_dll>zero && _urlmon_dll<fver(7,0,6000,21357))
		|| (_vgx_dll>zero && _vgx_dll<fver(7,0,6000,21357))
		|| (_webcheck_dll>zero && _webcheck_dll<fver(7,0,6000,21357))
		|| (_wininet_dll>zero && _wininet_dll<fver(7,0,6000,21357))
		)) {
		NN("Cumulative Security Update for Internet Explorer 7 for Windows XP (KB2879017)");
		XX(p+"IE7-WindowsXP-KB2879017-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && (_iexplore_exe>=fver(7,0,5730,11) && _iexplore_exe<fver(7,1,0,0)) && (
		   (_advpack_dll>zero && _advpack_dll<fver(7,0,6000,21582))
	    || (_corpol_dll>zero && _corpol_dll<fver(2007,0,0,21306))
		|| (_dxtmsft_dll>zero && _dxtmsft_dll<fver(7,0,6000,21582))
		|| (_dxtrans_dll>zero && _dxtrans_dll<fver(7,0,6000,21582))
		|| (_extmgr_dll>zero && _extmgr_dll<fver(7,0,6000,21582))
		|| (_html_iec>zero && _html_iec<fver(2017,0,0,21306))
		|| (_icardie_dll>zero && _icardie_dll<fver(7,0,6000,21582))
		|| (_ie4uinit_exe>zero && _ie4uinit_exe<fver(7,0,6000,21582))
		|| (_ieakeng_dll>zero && _ieakeng_dll<fver(7,0,6000,21582))
		|| (_ieaksie_dll>zero && _ieaksie_dll<fver(7,0,6000,21582))
		|| (_ieakui_dll>zero && _ieakui_dll<fver(7,0,6000,21582))
		|| (_ieapfltr_dat>zero && _ieapfltr_dat<fver(7,0,6011,1))
		|| (_ieapfltr_dll>zero && _ieapfltr_dll<fver(7,0,6000,16730))
		|| (_iedkcs32_dll>zero && _iedkcs32_dll<fver(17,0,6000,21582))
		|| (_ieencode_dll>zero && _ieencode_dll<fver(2017,0,0,21306))
		|| (_ieframe_dll>zero && _ieframe_dll<fver(7,0,6000,21582))
		|| (_ieframe_dll_mui>zero && _ieframe_dll_mui<fver(7,0,6000,16414))
		|| (_iepeers_dll>zero && _iepeers_dll<fver(7,0,6000,21582))
		|| (_iernonce_dll>zero && _iernonce_dll<fver(7,0,6000,21582))
		|| (_iertutil_dll>zero && _iertutil_dll<fver(7,0,6000,21582))
		|| (_ieudinit_exe>zero && _ieudinit_exe<fver(7,0,6000,21582))
		|| (_iexplore_exe>zero && _iexplore_exe<fver(7,0,6000,21582))
		|| (_inetcpl_cpl>zero && _inetcpl_cpl<fver(7,0,6000,21582))
		|| (_jsproxy_dll>zero && _jsproxy_dll<fver(7,0,6000,21582))
		|| (_msfeeds_dll>zero && _msfeeds_dll<fver(7,0,6000,21582))
		|| (_msfeedsbs_dll>zero && _msfeedsbs_dll<fver(7,0,6000,21582))
		|| (_mshtml_dll>zero && _mshtml_dll<fver(7,0,6000,21582))
		|| (_mshtmled_dll>zero && _mshtmled_dll<fver(7,0,6000,21582))
		|| (_msrating_dll>zero && _msrating_dll<fver(7,0,6000,21582))
		|| (_mstime_dll>zero && _mstime_dll<fver(7,0,6000,21582))
		|| (_occache_dll>zero && _occache_dll<fver(7,0,6000,21582))
		|| (_pngfilt_dll>zero && _pngfilt_dll<fver(7,0,6000,21582))
		|| (_url_dll>zero && _url_dll<fver(7,0,6000,21582))
		|| (_urlmon_dll>zero && _urlmon_dll<fver(7,0,6000,21582))
		|| (_vgx_dll>zero && _vgx_dll<fver(7,0,6000,21582))
		|| (_webcheck_dll>zero && _webcheck_dll<fver(7,0,6000,21582))
		|| (_wininet_dll>zero && _wininet_dll<fver(7,0,6000,21582))
		)) {
		NN("Cumulative Security Update for Internet Explorer 7 for WEPOS and POSReady 2009 (KB3148198)");
		XX(p+"ie7-windowsxp-kb3148198-x86-embedded-enu_254750ce40d33a8a4985de8339d8e66df6861f19.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && (_iexplore_exe>=fver(8,0,6001,18702) && _iexplore_exe<fver(8,1,0,0)) && (
		/* GDR */
		   (_ie4uinit_exe  >zero && _ie4uinit_exe  <fver(8,0,6001,18876))
		|| (_iedkcs32_dll  >zero && _iedkcs32_dll  <fver(18,0,6001,18876))
		|| (_ieframe_dll   >zero && _ieframe_dll   <fver(8,0,6001,18876))
		|| (_iepeers_dll   >zero && _iepeers_dll   <fver(8,0,6001,18876))
		|| (_ieproxy_dll   >zero && _ieproxy_dll   <fver(8,0,6001,18876))
		|| (_iertutil_dll  >zero && _iertutil_dll  <fver(8,0,6001,18876))
		|| (_inetcpl_cpl   >zero && _inetcpl_cpl   <fver(8,0,6001,18876))
		|| (_jsproxy_dll   >zero && _jsproxy_dll   <fver(8,0,6001,18876))
		|| (_msfeeds_dll   >zero && _msfeeds_dll   <fver(8,0,6001,18876))
		|| (_msfeedsbs_dll >zero && _msfeedsbs_dll <fver(8,0,6001,18876))
		|| (_mshtml_dll    >zero && _mshtml_dll    <fver(8,0,6001,18876))
		|| (_occache_dll   >zero && _occache_dll   <fver(8,0,6001,18876))
		|| (_urlmon_dll    >zero && _urlmon_dll    <fver(8,0,6001,18876))
		|| (_wininet_dll   >zero && _wininet_dll   <fver(8,0,6001,18876))
		|| (_xpshims_dll   >zero && _xpshims_dll   <fver(8,0,6001,18876))
		/* QFE */
		|| (_ie4uinit_exe  >fver(8,0,6001,18876) && _ie4uinit_exe  <fver(8,0,6001,22967))
		|| (_iedkcs32_dll  >fver(18,0,6001,18876)&& _iedkcs32_dll  <fver(18,0,6001,22967))
		|| (_ieframe_dll   >fver(8,0,6001,18876) && _ieframe_dll   <fver(8,0,6001,22967))
		|| (_iepeers_dll   >fver(8,0,6001,18876) && _iepeers_dll   <fver(8,0,6001,22967))
		|| (_ieproxy_dll   >fver(8,0,6001,18876) && _ieproxy_dll   <fver(8,0,6001,22967))
		|| (_iertutil_dll  >fver(8,0,6001,18876) && _iertutil_dll  <fver(8,0,6001,22967))
		|| (_inetcpl_cpl   >fver(8,0,6001,18876) && _inetcpl_cpl   <fver(8,0,6001,22967))
		|| (_jsproxy_dll   >fver(8,0,6001,18876) && _jsproxy_dll   <fver(8,0,6001,22967))
		|| (_msfeeds_dll   >fver(8,0,6001,18876) && _msfeeds_dll   <fver(8,0,6001,22967))
		|| (_msfeedsbs_dll >fver(8,0,6001,18876) && _msfeedsbs_dll <fver(8,0,6001,22967))
		|| (_mshtml_dll    >fver(8,0,6001,18876) && _mshtml_dll    <fver(8,0,6001,22967))
		|| (_occache_dll   >fver(8,0,6001,18876) && _occache_dll   <fver(8,0,6001,22967))
		|| (_urlmon_dll    >fver(8,0,6001,18876) && _urlmon_dll    <fver(8,0,6001,22967))
		|| (_wininet_dll   >fver(8,0,6001,18876) && _wininet_dll   <fver(8,0,6001,22967))
		|| (_xpshims_dll   >fver(8,0,6001,18876) && _xpshims_dll   <fver(8,0,6001,22967)) )) {
		NN("Cumulative Security Update for Internet Explorer 8 for Windows XP (KB978207)");
		XX(p+"IE8-WindowsXP-KB978207-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XPE_FLP) && (_iexplore_exe>=fver(8,0,6001,18702) && _iexplore_exe<fver(8,1,0,0)) && (
		/* QFE */
		   (_corpol_dll       >zero && _corpol_dll       <fver(2008,0,0,23532))
		|| (_html_iec         >zero && _html_iec         <fver(2018,0,0,23532))
		|| (_ie4uinit_exe     >zero && _ie4uinit_exe     <fver(8,0,6001,23532))
		|| (_iedkcs32_dll     >zero && _iedkcs32_dll     <fver(18,0,6001,23532))
		|| (_iedvtool_dll     >zero && _iedvtool_dll     <fver(8,0,6001,23532))
		|| (_ieframe_dll      >zero && _ieframe_dll      <fver(8,0,6001,23532))
		|| (_iepeers_dll      >zero && _iepeers_dll      <fver(8,0,6001,23532))
		|| (_ieproxy_dll      >zero && _ieproxy_dll      <fver(8,0,6001,23532))
		|| (_iertutil_dll     >zero && _iertutil_dll     <fver(8,0,6001,23532))
		|| (_inetcpl_cpl      >zero && _inetcpl_cpl      <fver(8,0,6001,23532))
		|| (_jsdbgui_dll      >zero && _jsdbgui_dll      <fver(8,0,6001,23532))
		|| (_jsproxy_dll      >zero && _jsproxy_dll      <fver(8,0,6001,23532))
		|| (_licmgr10_dll     >zero && _licmgr10_dll     <fver(8,0,6001,23532))
		|| (_msfeeds_dll      >zero && _msfeeds_dll      <fver(8,0,6001,23532))
		|| (_msfeedsbs_dll    >zero && _msfeedsbs_dll    <fver(8,0,6001,23532))
		|| (_mshtml_dll       >zero && _mshtml_dll       <fver(8,0,6001,23532))
		|| (_mshtmled_dll     >zero && _mshtmled_dll     <fver(8,0,6001,23532))
		|| (_mstime_dll       >zero && _mstime_dll       <fver(8,0,6001,23532))
		|| (_occache_dll      >zero && _occache_dll      <fver(8,0,6001,23532))
		|| (_url_dll          >zero && _url_dll          <fver(8,0,6001,23532))
		|| (_urlmon_dll       >zero && _urlmon_dll       <fver(8,0,6001,23532))
		|| (_vgx_dll          >zero && _vgx_dll          <fver(8,0,6001,23532))
		|| (_wininet_dll      >zero && _wininet_dll      <fver(8,0,6001,23532))
		|| (_xpshims_dll      >zero && _xpshims_dll      <fver(8,0,6001,23532)) )) {
		NN("Cumulative Security Update for Internet Explorer 8 for Windows XP (KB2879017)");
		XX(p+"IE8-WindowsXP-KB2879017-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_CLIENT) && (_iexplore_exe>=fver(8,0,6001,18702) && _iexplore_exe<fver(8,1,0,0)) && (
		   (_corpol_dll       >zero && _corpol_dll       <fver(2008,0,0,23942))
		|| (_dxtmsft_dll      >zero && _dxtmsft_dll      <fver(8,0,6001,23942))
		|| (_dxtrans_dll      >zero && _dxtrans_dll      <fver(8,0,6001,23942))
		|| (_hlink_dll        >zero && _hlink_dll        <fver(6,0,6001,23942))
		|| (_html_iec         >zero && _html_iec         <fver(2018,0,0,23942))
		|| (_ie4uinit_exe     >zero && _ie4uinit_exe     <fver(8,0,6001,23942))
		|| (_iedkcs32_dll     >zero && _iedkcs32_dll     <fver(18,0,6001,23942))
		|| (_iedvtool_dll     >zero && _iedvtool_dll     <fver(8,0,6001,23942))
		|| (_ieframe_dll      >zero && _ieframe_dll      <fver(8,0,6001,23942))
		|| (_iepeers_dll      >zero && _iepeers_dll      <fver(8,0,6001,23942))
		|| (_ieproxy_dll      >zero && _ieproxy_dll      <fver(8,0,6001,23942))
		|| (_iertutil_dll     >zero && _iertutil_dll     <fver(8,0,6001,23942))
		|| (_iexplore_exe     >zero && _iexplore_exe     <fver(8,0,6001,23942))
		|| (_inetcomm_dll     >zero && _inetcomm_dll     <fver(6,0,2900,7246))
		|| (_inetcpl_cpl      >zero && _inetcpl_cpl      <fver(8,0,6001,23942))
		|| (_jscript_dll      >zero && _jscript_dll      <fver(5,8,6001,23942))
		|| (_jsdbgui_dll      >zero && _jsdbgui_dll      <fver(8,0,6001,23942))
		|| (_jsprofilerui_dll >zero && _jsprofilerui_dll <fver(8,0,6001,23942))
		|| (_jsproxy_dll      >zero && _jsproxy_dll      <fver(8,0,6001,23942))
		|| (_licmgr10_dll     >zero && _licmgr10_dll     <fver(8,0,6001,23942))
		|| (_msfeeds_dll      >zero && _msfeeds_dll      <fver(8,0,6001,23942))
		|| (_msfeedsbs_dll    >zero && _msfeedsbs_dll    <fver(8,0,6001,23942))
		|| (_mshtml_dll       >zero && _mshtml_dll       <fver(8,0,6001,23942))
		|| (_mshtmled_dll     >zero && _mshtmled_dll     <fver(8,0,6001,23942))
		|| (_msrating_dll     >zero && _msrating_dll     <fver(8,0,6001,23942))
		|| (_mstime_dll       >zero && _mstime_dll       <fver(8,0,6001,23942))
		|| (_occache_dll      >zero && _occache_dll      <fver(8,0,6001,23942))
		|| (_tdc_ocx          >zero && _tdc_ocx          <fver(8,0,6001,23942))
		|| (_url_dll          >zero && _url_dll          <fver(8,0,6001,23942))
		|| (_urlmon_dll       >zero && _urlmon_dll       <fver(8,0,6001,23942))
		|| (_vbscript_dll     >zero && _vbscript_dll     <fver(5,8,6001,23942))
		|| (_vgx_dll          >zero && _vgx_dll          <fver(8,0,6001,23942))
		|| (_webcheck_dll     >zero && _webcheck_dll     <fver(8,0,6001,23942))
		|| (_wininet_dll      >zero && _wininet_dll      <fver(8,0,6001,23942))
		|| (_xpshims_dll      >zero && _xpshims_dll      <fver(8,0,6001,23942))
		)) {
		NN("Cumulative Security Update for Internet Explorer 8 for Windows XP SP3 (KB4018271)");
		XX(p+"ie8-windowsxp-kb4018271-x86-custom-enu_122a1122404b9862e6998bcec0917de02c378de6.exe"+a1);
	}
	if( sp==3 && (sku & XPE_WES2009) && (_iexplore_exe>=fver(8,0,6001,18702) && _iexplore_exe<fver(8,1,0,0)) && (
		   (_corpol_dll       >zero && _corpol_dll       <fver(2008,0,0,24159))
		|| (_dxtmsft_dll      >zero && _dxtmsft_dll      <fver(8,0,6001,24159))
		|| (_dxtrans_dll      >zero && _dxtrans_dll      <fver(8,0,6001,24159))
		|| (_hlink_dll        >zero && _hlink_dll        <fver(6,0,6001,24159))
		|| (_html_iec         >zero && _html_iec         <fver(2018,0,0,24159))
		|| (_ie4uinit_exe     >zero && _ie4uinit_exe     <fver(8,0,6001,24159))
		|| (_iedkcs32_dll     >zero && _iedkcs32_dll     <fver(18,0,6001,24159))
		|| (_iedvtool_dll     >zero && _iedvtool_dll     <fver(8,0,6001,24159))
		|| (_ieframe_dll      >zero && _ieframe_dll      <fver(8,0,6001,24159))
		|| (_iepeers_dll      >zero && _iepeers_dll      <fver(8,0,6001,24159))
		|| (_ieproxy_dll      >zero && _ieproxy_dll      <fver(8,0,6001,24159))
		|| (_iertutil_dll     >zero && _iertutil_dll     <fver(8,0,6001,24159))
		|| (_iexplore_exe     >zero && _iexplore_exe     <fver(8,0,6001,24159))
		|| (_inetcomm_dll     >zero && _inetcomm_dll     <fver(6,0,2900,7641))
		|| (_inetcpl_cpl      >zero && _inetcpl_cpl      <fver(8,0,6001,24159))
		|| (_jscript_dll      >zero && _jscript_dll      <fver(5,8,6001,24159))
		|| (_jsdbgui_dll      >zero && _jsdbgui_dll      <fver(8,0,6001,24159))
		|| (_jsprofilerui_dll >zero && _jsprofilerui_dll <fver(8,0,6001,24159))
		|| (_jsproxy_dll      >zero && _jsproxy_dll      <fver(8,0,6001,24159))
		|| (_licmgr10_dll     >zero && _licmgr10_dll     <fver(8,0,6001,24159))
		|| (_msfeeds_dll      >zero && _msfeeds_dll      <fver(8,0,6001,24159))
		|| (_msfeedsbs_dll    >zero && _msfeedsbs_dll    <fver(8,0,6001,24159))
		|| (_mshtml_dll       >zero && _mshtml_dll       <fver(8,0,6001,24159))
		|| (_mshtmled_dll     >zero && _mshtmled_dll     <fver(8,0,6001,24159))
		|| (_msrating_dll     >zero && _msrating_dll     <fver(8,0,6001,24159))
		|| (_mstime_dll       >zero && _mstime_dll       <fver(8,0,6001,24159))
		|| (_occache_dll      >zero && _occache_dll      <fver(8,0,6001,24159))
		|| (_tdc_ocx          >zero && _tdc_ocx          <fver(8,0,6001,24159))
		|| (_url_dll          >zero && _url_dll          <fver(8,0,6001,24159))
		|| (_urlmon_dll       >zero && _urlmon_dll       <fver(8,0,6001,24159))
		|| (_vbscript_dll     >zero && _vbscript_dll     <fver(5,8,6001,24159))
		|| (_vgx_dll          >zero && _vgx_dll          <fver(8,0,6001,24159))
		|| (_webcheck_dll     >zero && _webcheck_dll     <fver(8,0,6001,24159))
		|| (_wininet_dll      >zero && _wininet_dll      <fver(8,0,6001,24159))
		|| (_xpshims_dll      >zero && _xpshims_dll      <fver(8,0,6001,24159))
		)) {
		NN("2019-02 Cumulative Security Update for Internet Explorer 8 for WES09 and POSReady 2009 for x86-based systems (KB4486474)");
		XX(p+"ie8-windowsxp-kb4486474-x86-embedded-enu_c48cdecf2affafccc0668791ac03df6f110922c4.exe"+a1);
	}
	if( sp==3 && (sku & XPE_POSREADY2009) && (_iexplore_exe>=fver(8,0,6001,18702) && _iexplore_exe<fver(8,1,0,0)) && (
		   (_corpol_dll       >zero && _corpol_dll       <fver(2008,0,0,24177))
		|| (_dxtmsft_dll      >zero && _dxtmsft_dll      <fver(8,0,6001,24177))
		|| (_dxtrans_dll      >zero && _dxtrans_dll      <fver(8,0,6001,24177))
		|| (_hlink_dll        >zero && _hlink_dll        <fver(6,0,6001,24177))
		|| (_html_iec         >zero && _html_iec         <fver(2018,0,0,24177))
		|| (_ie4uinit_exe     >zero && _ie4uinit_exe     <fver(8,0,6001,24177))
		|| (_iedkcs32_dll     >zero && _iedkcs32_dll     <fver(18,0,6001,24177))
		|| (_iedvtool_dll     >zero && _iedvtool_dll     <fver(8,0,6001,24177))
		|| (_ieframe_dll      >zero && _ieframe_dll      <fver(8,0,6001,24177))
		|| (_iepeers_dll      >zero && _iepeers_dll      <fver(8,0,6001,24177))
		|| (_ieproxy_dll      >zero && _ieproxy_dll      <fver(8,0,6001,24177))
		|| (_iertutil_dll     >zero && _iertutil_dll     <fver(8,0,6001,24177))
		|| (_iexplore_exe     >zero && _iexplore_exe     <fver(8,0,6001,24177))
		|| (_inetcomm_dll     >zero && _inetcomm_dll     <fver(6,0,2900,7681))
		|| (_inetcpl_cpl      >zero && _inetcpl_cpl      <fver(8,0,6001,24177))
		|| (_jscript_dll      >zero && _jscript_dll      <fver(5,8,6001,24177))
		|| (_jsdbgui_dll      >zero && _jsdbgui_dll      <fver(8,0,6001,24177))
		|| (_jsprofilerui_dll >zero && _jsprofilerui_dll <fver(8,0,6001,24177))
		|| (_jsproxy_dll      >zero && _jsproxy_dll      <fver(8,0,6001,24177))
		|| (_licmgr10_dll     >zero && _licmgr10_dll     <fver(8,0,6001,24177))
		|| (_msfeeds_dll      >zero && _msfeeds_dll      <fver(8,0,6001,24177))
		|| (_msfeedsbs_dll    >zero && _msfeedsbs_dll    <fver(8,0,6001,24177))
		|| (_mshtml_dll       >zero && _mshtml_dll       <fver(8,0,6001,24177))
		|| (_mshtmled_dll     >zero && _mshtmled_dll     <fver(8,0,6001,24177))
		|| (_msrating_dll     >zero && _msrating_dll     <fver(8,0,6001,24177))
		|| (_mstime_dll       >zero && _mstime_dll       <fver(8,0,6001,24177))
		|| (_occache_dll      >zero && _occache_dll      <fver(8,0,6001,24177))
		|| (_tdc_ocx          >zero && _tdc_ocx          <fver(8,0,6001,24177))
		|| (_url_dll          >zero && _url_dll          <fver(8,0,6001,24177))
		|| (_urlmon_dll       >zero && _urlmon_dll       <fver(8,0,6001,24177))
		|| (_vbscript_dll     >zero && _vbscript_dll     <fver(5,8,6001,24177))
		|| (_vgx_dll          >zero && _vgx_dll          <fver(8,0,6001,24177))
		|| (_webcheck_dll     >zero && _webcheck_dll     <fver(8,0,6001,24177))
		|| (_wininet_dll      >zero && _wininet_dll      <fver(8,0,6001,24177))
		|| (_xpshims_dll      >zero && _xpshims_dll      <fver(8,0,6001,24177))
		)) {
		NN("2019-04 Cumulative Security Update for Internet Explorer 8 for POSReady 2009 for x86-based systems (KB4493435)");
		XX(p+"ie8-windowsxp-kb4493435-x86-embedded-enu_1029e7193ca4b683d1d0c1996bc7c66f1a2ae37f.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && (_iexplore_exe>=fver(8,0,6001,18702) && _iexplore_exe<fver(8,1,0,0)) && (
		/* GDR */
		   (_iecompat_dll     <fver(8,0,6001,19130))
		/* QFE */
		|| (_iecompat_dll      >fver(8,0,6001,19130) && _iecompat_dll      <fver(8,0,6001,23226)) )) {
		NN("Update for Internet Explorer 8 Compatibility View List for Windows XP (KB2598845)");
		XX(p+"IE8-WindowsXP-KB2598845-x86-ENU.exe"+a1);
	}

	// Jscript and Vbscript 5.6 (IE6)
	if( sp==2 && (sku & XP_ALL) && 
		( _jscript_dll <= fver(5,6,0,8835) && _vbscript_dll <= fver(5,6,0,8835)) &&
		(                 (_jscript_dll >=fver(5,6,0,0) && _jscript_dll <fver(5,6,0,8835))
		              ||  (_vbscript_dll>=fver(5,6,0,0) && _vbscript_dll<fver(5,6,0,8835)) )) {
		NN("Security Update for Windows XP (KB944338)");
		XX(p+"windowsxp-kb944338-v2-x86-enu_d8cade8456591867f22cdae4c42db7f473afbd67.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _jscript_dll>=fver(5,6,0,0) && _jscript_dll<fver(5,6,0,8837)) {
		NN("Security Update for Jscript 5.6 for Windows XP (KB971961)");
		XX(p+"windowsxp-kb971961-js56-x86-enu_9ce6ccd0ed6a7b00212e2a90afe2b4747017b3a5.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _vbscript_dll>=fver(5,6,0,0) && _vbscript_dll<fver(5,6,0,8838)) {
		NN("Security Update for Windows XP (KB981350)");
		XX(p+"windowsxp-kb981350-x86-enu_9fc0909c468c17870ab868d0d7f4eb815b309045.exe"+a1);
	}

	// Jscript and Vbscript 5.7 (IE7)
	if( sp==2 && (sku & XP_ALL) && _jscript_dll>=fver(5,7,0,0) && _jscript_dll<fver(5,7,6002,22145)) {
		NN("Security Update for Jscript 5.7 for Windows XP (KB971961)");
		XX(p+"windowsxp-kb971961-js57-x86-enu_9722544230b316cbd21740ed56dc7a9e7af9603b.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _vbscript_dll>=fver(5,7,0,0) && _vbscript_dll<fver(5,7,6002,22354)) {
		NN("Security Update for Windows XP (KB981349)");
		XX(p+"windowsxp-kb981349-x86-enu_e5fec41b4a9647bc5050dbbd1a08b469d72c69f4.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && (
		                  (_cscript_exe  >zero && _cscript_exe  <fver(5,7,0,18066))
		              ||  (_jscript_dll  >zero && _jscript_dll  <fver(5,7,0,18066))
					  ||  (_scrobj_dll   >zero && _scrobj_dll   <fver(5,7,0,18066))
					  ||  (_scrrun_dll   >zero && _scrrun_dll   <fver(5,7,0,18066))
					  ||  (_vbscript_dll >zero && _vbscript_dll <fver(5,7,0,18066))
					  ||  (_wscript_exe  >zero && _wscript_exe  <fver(5,7,0,18066))
					  ||  (_wshext_dll   >zero && _wshext_dll   <fver(5,7,0,18066))
					  ||  (_wshom_ocx    >zero && _wshom_ocx    <fver(5,7,0,18066)) )) {
		NN("Update for Windows XP (KB951978)");
		XX(p+"WindowsXP-KB951978-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( (_jscript_dll>=fver(5,7,0,0) && _jscript_dll<fver(5,7,6002,22589))
					  ||  (_vbscript_dll>=fver(5,7,0,0) && _vbscript_dll<fver(5,7,6002,22589)) )) {
		NN("Security Update for Windows XP (KB2510581)");
		XX(p+"WindowsXP-KB2510581-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _vbscript_dll>=fver(5,7,0,0) && _vbscript_dll<fver(5,7,6002,23292)) {
		NN("Security Update for Windows XP (KB2909212)");
		XX(p+"WindowsXP-KB2909212-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && ( (_jscript_dll>=fver(5,7,0,0) && _jscript_dll<fver(5,7,6002,23877))
					  ||  (_vbscript_dll>=fver(5,7,0,0) && _vbscript_dll<fver(5,7,6002,23877)) )) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3124624)");
		XX(p+"windowsxp-kb3124624-x86-embedded-enu_31d357f9fffe58a498ac46282610be31263ab3de.exe"+a1);
	}

	// Jscript and Vbscript 5.8 (IE8)
//	if( sp==2 && (sku & XP_ALL) && ( _iexplore_exe>=fver(8,0,6001,18702) && _iexplore_exe<fver(8,1,0,0))
//		/*&& _jscript_dll>=fver(5,8,0,0)*/ && _jscript_dll<fver(5,8,6001,22886)) {
//		NN("Security Update for Jscript 5.8 for Windows XP (KB971961)");
//		XX(p+"ie8-windowsxp-kb971961-x86-enu_49f7f3fb604da50265f558ff5db3542dcd947d87.exe"+a1);
//	}
	if( sp==2 && (sku & XP_ALL) && ( _iexplore_exe>=fver(8,0,6001,18702) && _iexplore_exe<fver(8,1,0,0))
		/*&& _jscript_dll>=fver(5,8,0,0)*/ && _jscript_dll<fver(5,8,6001,22960)) {
		NN("Update for Windows XP (KB976662)");
		XX(p+"ie8-windowsxp-kb976662-x86-enu_6060b6dd1afd7d41be7f1ae4632bd31657d5e0c4.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ( _iexplore_exe>=fver(8,0,6001,18702) && _iexplore_exe<fver(8,1,0,0))
		/*&& _vbscript_dll>=fver(5,8,0,0)*/ && _vbscript_dll<fver(5,8,6001,23000)) {
		NN("Security Update for Windows XP (KB981332)");
		XX(p+"ie8-windowsxp-kb981332-x86-enu_d1ba1076cb5d94b06333011c6eb29dedb5c2c48d.exe"+a1);
	}
	if( sp==3 && (sku & XP_PLUS_FLP) && ( _iexplore_exe>=fver(8,0,6001,18702) && _iexplore_exe<fver(8,1,0,0))
		                && ( (_jscript_dll>zero && _jscript_dll<fver(5,8,6001,23141))
					  ||  (_vbscript_dll>zero && _vbscript_dll<fver(5,8,6001,23141)) )) {
		NN("Security Update for Windows XP (KB2510531)");
		XX(p+"IE8-WindowsXP-KB2510531-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( _iexplore_exe>=fver(8,0,6001,18702) && _iexplore_exe<fver(8,1,0,0))
		/*&& _jscript_dll>=fver(5,8,0,0)*/ && _jscript_dll<fver(5,8,6001,23259)) {
		NN("Update for Windows XP (KB2632503)");
		XX(p+"IE8-WindowsXP-KB2632503-x86-ENU.exe"+a1);
	}
	

	// Windows Media Player Updates
	if( sp>=2 && (sku & XP_ALL) && _wmpeffects_dll>=fver(11,0,5721,5145) && _wmpeffects_dll<fver(11,0,5721,5252)) {
		NN("Security Update for Windows XP (KB954154)");
		XX(p+"windowsmedia11-kb954154-x86-enu_9b75815f8c5d57dc8e2e22e221fb1a106f9ce6a0.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ((_wmp_dll  >=fver(9,0,0,2980) /* WM9L Section */
		                 && _wmpdxm_dll >=fver(9,0,0,2980)
					     && (_wmp_dll<fver(9,0,0,3364) || _wmpdxm_dll<fver(9,0,0,3364)))

					  || (_wmp_dll  >=fver(10,0,0,3646) /* WM10L Section */
		                 && _wmpdxm_dll >=fver(10,0,0,3646)
					     && (_wmp_dll<fver(10,0,0,4074) || _wmpdxm_dll<fver(10,0,0,4074)))

					  || (_wmp_dll  >=fver(11,0,5721,5145) /* WM11 Section */
		                 && _wmpdxm_dll >=fver(11,0,5721,5145)
					     && (_wmp_dll<fver(11,0,5721,5268) || _wmpdxm_dll<fver(11,0,5721,5268)))					   
					   )) {
		NN("Security Update for Windows XP Service Pack 2 (KB973540)");
		XX(p+"windowsxp-sp2-windowsmedia-kb973540-x86-enu_dff9fd1cafd1b740784f00e43a3aff588d0c810d.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && _wmp_dll>=fver(9,0,0,2980) && _wmp_dll<fver(9,0,0,3367)) {
		NN("Security Update for Windows Media Player 9 for Windows XP SP2 (KB979402)");
		XX(p+"windowsxp-sp2-windowsmedia-kb979402-x86-enu_09cff283401228cd946081159b796d5d1abdfbd3.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ((_wmp_dll  >=fver(9,0,0,2980) /* WM9 Section */
		                 && _wmpdxm_dll >=fver(9,0,0,2980)
					     && (_wmp_dll<fver(9,0,0,4507) || _wmpdxm_dll<fver(9,0,0,4507)))

					  || (_wmp_dll  >=fver(10,0,0,3646) /* WM10L Section */
		                 && _wmpdxm_dll >=fver(10,0,0,3646)
					     && (_wmp_dll<fver(10,0,0,4074) || _wmpdxm_dll<fver(10,0,0,4074)))

					  || (_wmp_dll  >=fver(11,0,5721,5145) /* WM11 Section */
		                 && _wmpdxm_dll >=fver(11,0,5721,5145)
					     && (_wmp_dll<fver(11,0,5721,5268) || _wmpdxm_dll<fver(11,0,5721,5268)))					   
					   )) {
		NN("Security Update for Windows XP Service Pack 3 (KB973540)");
		XX(p+"WindowsXP-WindowsMedia-KB973540-x86-ENU.exe"+a1);
	}
	if( sp>=2 && (sku & XP_ALL) && ( (_l3codeca_acm>zero && _l3codeca_acm<fver(1,9,0,306))
					  ||  (_l3codecx_ax>zero && _l3codecx_ax<fver(1,6,0,51)) )) {
		NN("Security Update for Windows XP (KB977816)");
		XX(p+"WindowsXP-KB977816-x86-ENU.exe"+a1);
	}
	if( sp>=2 && (sku & XP_ALL) && ((_wmasf_dll     >=fver(9,0,0,2980) /* WMP9NL Section */
					     && (_wmasf_dll<fver(9,0,0,3267) || (_wmvcore_dll>zero && _wmvcore_dll<fver(9,0,0,3267))))
						 
					  || (_wmasf_dll     >=fver(9,0,0,3287) /* WMP9L Section */
					     && (_wmasf_dll<fver(9,0,0,3355) || (_wmvcore_dll>zero && _wmvcore_dll<fver(9,0,0,3355))))

					  || (_wmasf_dll  >=fver(10,0,0,3646) /* WMP10 Section */
		                 && _wmasf_dll < fver(10,0,0,4060))

					  || (_wmasf_dll  >=fver(10,0,0,4332) /* EMERALD Section */
		                 && _wmasf_dll < fver(10,0,0,4359))

					  || (_wmasf_dll  >=fver(11,0,5721,5145) /* WMP11 Section */
		                 && _wmasf_dll < fver(11,0,5721,5238))					   
					   )) {
		if(_wmasf_dll<fver(10,0,0,3646)) {
			NN("Security Update for Windows XP with Windows Media Format Runtime 9 (KB941569)");
		} else {
			NN("Security Update for Windows XP with Windows Media Format Runtime 9.5 and 11 (KB941569)");
		}
		XX(p+"windowsxp-kb941569-x86-enu_55e8b619dd436e8be22c381fa8643a58b5f7c2a8.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ((_logagent_exe  >=fver(9,0,0,2980) /* WM9 Section */
		                 && _wmnetmgr_dll >=fver(9,0,0,2980)
						 && _wmvcore_dll  >=fver(9,0,0,2980)
					     && (_logagent_exe<fver(9,0,0,3268) || _wmnetmgr_dll<fver(9,0,0,3268) || _wmvcore_dll<fver(9,0,0,3268)))

					  || (_logagent_exe   >=fver(9,0,0,3287) /* WM9L Section */
		                 && _wmnetmgr_dll >=fver(9,0,0,3287)
						 && _wmvcore_dll  >=fver(9,0,0,3287)
					     && (_logagent_exe<fver(9,0,0,3358) || _wmnetmgr_dll<fver(9,0,0,3358) || _wmvcore_dll<fver(9,0,0,3358)))
					  
					  || (_logagent_exe   >=fver(10,0,0,3646) /* WM10 Section */
		                 && _wmnetmgr_dll >=fver(10,0,0,3646)
						 && _wmvcore_dll  >=fver(10,0,0,3646)
					     && (_logagent_exe<fver(10,0,0,3703) || _wmnetmgr_dll<fver(10,0,0,3703) || _wmvcore_dll<fver(10,0,0,3703)))

					  || (_logagent_exe   >=fver(10,0,0,3802) /* WM10L Section */
		                 && _wmnetmgr_dll >=fver(10,0,0,3802)
						 && _wmvcore_dll  >=fver(10,0,0,3802)
					     && (_logagent_exe<fver(10,0,0,4066) || _wmnetmgr_dll<fver(10,0,0,4066) || _wmvcore_dll<fver(10,0,0,4066)))

					  || (_logagent_exe   >=fver(10,0,0,3646) /* WM10MIX Section */
		                 && _wmnetmgr_dll >=fver(10,0,0,3802)
						 && _wmvcore_dll  >=fver(10,0,0,3646)
					     && (_logagent_exe<fver(10,0,0,3703) || _wmnetmgr_dll<fver(10,0,0,4066) || _wmvcore_dll<fver(10,0,0,3703)))

					  || (_logagent_exe   >=fver(10,0,0,4332) /* MCEUR2 Section */
		                 && _wmnetmgr_dll >=fver(10,0,0,4332)
						 && _wmvcore_dll  >=fver(10,0,0,4332)
					     && (_logagent_exe<fver(10,0,0,4362) || _wmnetmgr_dll<fver(10,0,0,4362) || _wmvcore_dll<fver(10,0,0,4362)))

					  || (_logagent_exe   >=fver(11,0,5721,5145) /* WM11 Section */
		                 && _wmnetmgr_dll >=fver(11,0,5721,5145)
						 && _wmvcore_dll  >=fver(11,0,5721,5145)
					     && (_logagent_exe<fver(11,0,5721,5251) || _wmnetmgr_dll<fver(11,0,5721,5251) || _wmvcore_dll<fver(11,0,5721,5251)))
					   
					   )) {
		NN("Security Update for Windows XP Service Pack 2 (KB952069)");
		XX(p+"windowsxp-sp2-windowsmedia-kb952069-v2-x86-enu_97de22fb2fed50fb03e1107b579975f2295feb8a.exe"+a1);
	}
	else if( sp==3 && (sku & XP_ALL) && ((_logagent_exe  >=fver(9,0,0,4500) /* WM9 Section */
		                 && _wmnetmgr_dll >=fver(9,0,0,4500)
						 && _wmvcore_dll  >=fver(9,0,0,4500)
					     && (_logagent_exe<fver(9,0,0,4504) || _wmnetmgr_dll<fver(9,0,0,4504) || _wmvcore_dll<fver(9,0,0,4504)))

					  || (_logagent_exe   >=fver(10,0,0,3646) /* WM10 Section */
		                 && _wmnetmgr_dll >=fver(10,0,0,3646)
						 && _wmvcore_dll  >=fver(10,0,0,3646)
					     && (_logagent_exe<fver(10,0,0,3703) || _wmnetmgr_dll<fver(10,0,0,3703) || _wmvcore_dll<fver(10,0,0,3703)))

					  || (_logagent_exe   >=fver(10,0,0,3802) /* WM10L Section */
		                 && _wmnetmgr_dll >=fver(10,0,0,3802)
						 && _wmvcore_dll  >=fver(10,0,0,3802)
					     && (_logagent_exe<fver(10,0,0,4066) || _wmnetmgr_dll<fver(10,0,0,4066) || _wmvcore_dll<fver(10,0,0,4066)))

					  || (_logagent_exe   >=fver(10,0,0,3646) /* WM10MIX Section */
		                 && _wmnetmgr_dll >=fver(10,0,0,3802)
						 && _wmvcore_dll  >=fver(10,0,0,3646)
					     && (_logagent_exe<fver(10,0,0,3703) || _wmnetmgr_dll<fver(10,0,0,4066) || _wmvcore_dll<fver(10,0,0,3703)))

					  || (_logagent_exe   >=fver(10,0,0,4332) /* MCEUR2 Section */
		                 && _wmnetmgr_dll >=fver(10,0,0,4332)
						 && _wmvcore_dll  >=fver(10,0,0,4332)
					     && (_logagent_exe<fver(10,0,0,4362) || _wmnetmgr_dll<fver(10,0,0,4362) || _wmvcore_dll<fver(10,0,0,4362)))

					  || (_logagent_exe   >=fver(11,0,5721,5145) /* WM11 Section */
		                 && _wmnetmgr_dll >=fver(11,0,5721,5145)
						 && _wmvcore_dll  >=fver(11,0,5721,5145)
					     && (_logagent_exe<fver(11,0,5721,5251) || _wmnetmgr_dll<fver(11,0,5721,5251) || _wmvcore_dll<fver(11,0,5721,5251)))
					   
					   )) {
		NN("Security Update for Windows XP Service Pack 3 (KB952069)");
		XX(p+"WindowsXP-WindowsMedia-KB952069-v2-x86-ENU.exe"+a1);
	}
	else if( sp==3 && (sku & XP_ALL) && (
		/* WM9 Section */
		   (_logagent_exe >= fver(9,0,0,2980)  && _logagent_exe <= fver(9,0,0,9999) && _logagent_exe < fver(9,0,0,4504) )
		/* WM10 Section */
		|| (_logagent_exe >= fver(10,0,0,3646) && _logagent_exe <= fver(10,0,0,3801) && _logagent_exe < fver(10,0,0,3703) )
		/* WM10L Section */
		|| (_logagent_exe >= fver(10,0,0,3802) && _logagent_exe <= fver(10,0,0,4299) && _logagent_exe < fver(10,0,0,4066) )
		/* MCEUR2 Section */
		|| (_logagent_exe >= fver(10,0,0,4332) && _logagent_exe <= fver(10,0,0,4999) && _logagent_exe < fver(10,0,0,4362) )
		/* WM11 Section */
		|| (_logagent_exe >= fver(11,0,5721,5145) && _logagent_exe <= fver(11,0,5721,9999) && _logagent_exe < fver(11,0,5721,5251) ) )) {
		NN("Security Update for Windows XP Service Pack 3 (KB972187)");
		XX(p+"windowsxp-windowsmedia-kb972187-x86-enu_d07aa4b7065e73bb2e9db2e7ce3289e4d91848ce.exe"+a1);
	}
	//	if( sp==2 && (sku & XP_ALL) && ((_wmvcore_dll  >=fver(9,0,0,2980) /* WMP9NL Section */
//		                 && _wmvcore_dll < fver(9,0,0,3265))
//
//					  || (_wmvcore_dll  >=fver(9,0,0,3287) /* WMP9L Section */
//		                 && _wmvcore_dll < fver(9,0,0,3353))
//
//					  || (_wmvcore_dll  >=fver(10,0,0,3646) /* WMP10NL Section */
//		                 && _wmvcore_dll < fver(10,0,0,3702))
//
//					  || (_wmvcore_dll  >=fver(10,0,0,3802) /* WMP10L Section */
//		                 && _wmvcore_dll < fver(10,0,0,4054))
//
//					  || (_wmvcore_dll  >=fver(10,0,0,4332) /* Emerald Section */
//		                 && _wmvcore_dll < fver(10,0,0,4357))					   
//					   )) {
//		// KB923689 is replaced by KB978695 on SP2
//		NN("Security Update for Windows XP with Windows Media Format Runtime 9 (KB923689)");
//		XX(p+"windowsxp-kb923689-v2-x86-enu_2cd41423c99bd3bd35d64fd2afcbe1005fd92a62.exe"+a1);
//	}
	if( sp==2 && (sku & XP_ALL) && ((_wmvcore_dll  >=fver(9,0,0,2980) /* WM9 Section */
		                 && _wmvcore_dll < fver(9,0,0,3272))

					  || (_wmvcore_dll  >=fver(9,0,0,3287) /* WM9L Section */
		                 && _wmvcore_dll < fver(9,0,0,3369))

					  || (_wmvcore_dll  >=fver(10,0,0,3646) /* WM10 Section */
		                 && _wmvcore_dll < fver(10,0,0,3706))

					  || (_wmvcore_dll  >=fver(10,0,0,3802) /* WM10L Section */
		                 && _wmvcore_dll < fver(10,0,0,4078))

					  || (_wmvcore_dll  >=fver(10,0,0,4332) /* MCEUR2 Section */
		                 && _wmvcore_dll < fver(10,0,0,4374))

					  || (_wmvcore_dll  >=fver(11,0,5721,5145) /* WM11 Section */
		                 && _wmvcore_dll < fver(11,0,5721,5275))					   
					   )) {
		NN("Security Update for Windows Media Format Runtime 9, 9.5 & 11 for Windows XP SP2 (KB978695)");
		XX(p+"windowsxp-sp2-windowsmedia-kb978695-x86-enu_0903920ab44d6cd39bcb6c8d192547e30d2c5809.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ((_wmvcore_dll  >=fver(9,0,0,2980) /* WM9 Section */
		                 && _wmvcore_dll < fver(9,0,0,4509))

					  || (_wmvcore_dll  >=fver(10,0,0,3646) /* WM10 Section */
		                 && _wmvcore_dll < fver(10,0,0,3706))

					  || (_wmvcore_dll  >=fver(10,0,0,3802) /* WM10L Section */
		                 && _wmvcore_dll < fver(10,0,0,4078))

					  || (_wmvcore_dll  >=fver(10,0,0,4332) /* MCEUR2 Section */
		                 && _wmvcore_dll < fver(10,0,0,4374))

					  || (_wmvcore_dll  >=fver(11,0,5721,5145) /* WM11 Section */
		                 && _wmvcore_dll < fver(11,0,5721,5275))					   
					   )) {
		NN("Security Update for Windows Media Format Runtime 9, 9.5 & 11 for Windows XP SP3 (KB978695)");
		XX(p+"WindowsXP-WindowsMedia-KB978695-x86-ENU.exe"+a1);
	}
	if( sp==3 && !kb891122 && !kb913800 
		      && (sku & XP_ALL) && _wmvdmod_dll>=fver(10,0,0,3646) && _wmvdmod_dll<fver(10,0,0,3708)) {
		NN("Security Update for Windows Media Format Runtime 9.5 for Windows XP (KB2834902)");
		XX(p+"windowsxp-windowsmedia-kb2834902-v2-x86-enu_ecc8652da2b85688917a4f7aa48ac1efe84975fd.exe"+a1);
	}
	if( sp==3 && !kb913800 && (sku & XP_ALL) && (kb891122 || _wmvdmod_dll>=fver(10,0,0,3802)) && _wmvdmod_dll<fver(10,0,0,4084)) {
		NN("Security Update for Windows Media Format Runtime 9.5 for Windows XP (KB2834903)");
		XX(p+"WindowsXP-WindowsMedia-KB2834903-v2-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _wmvdecod_dll>=fver(11,0,5721,5145) && _wmvdecod_dll<fver(11,0,5721,5289)) {
		NN("Security Update for Windows Media Format Runtime 11 for Windows XP (KB2834904)");
		XX(p+"WindowsXP-WindowsMedia-KB2834904-v2-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && (kb913800 || _wmvdmod_dll>=fver(10,0,0,4332)) && _wmvdmod_dll<fver(10,0,0,4377)) {
		NN("Security Update for Windows Media Format Runtime 9.5 for Windows XP (KB2834905)");
		XX(p+"windowsxp-windowsmedia-kb2834905-v2-x86-enu_c34829167838fddcc6d679649a1f79d39280a2f0.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && ((_wmspdmod_dll >=fver(9,0,0,2980) /* WM9 Section */
					     && _wmspdmod_dll<fver(9,0,0,3269))

					  || (_wmspdmod_dll  >=fver(10,0,0,3646) /* WM10 Section */
		                 && _wmspdmod_dll < fver(10,0,0,3704))

					  || (_wmspdmod_dll  >=fver(10,0,0,3802) /* WM10L Section */
		                 && _wmspdmod_dll < fver(10,0,0,4070))

					  || (_wmspdmod_dll  >=fver(10,0,0,4332) /* MCEUR2 Section */
		                 && _wmspdmod_dll < fver(10,0,0,4365))

					  || (_wmspdmod_dll  >=fver(11,0,5721,5145) /* WM11 Section */
		                 && _wmspdmod_dll < fver(11,0,5721,5263))					   
					   )) {
		NN("Security Update for Windows Media Format Runtime 9, 9.5 & 11 for Windows XP SP 2 (KB954155)");
		XX(p+"windowsxp-sp2-windowsmedia-kb954155-x86-enu_77db0c2aa79d2a5049fb9fb0fa4fbc5e98d4f30d.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ((_wmspdmod_dll >=fver(9,0,0,2980) /* WM9 Section */
					     && _wmspdmod_dll<fver(9,0,0,4505))

					  || (_wmspdmod_dll  >=fver(10,0,0,3646) /* WM10 Section */
		                 && _wmspdmod_dll < fver(10,0,0,3704))

					  || (_wmspdmod_dll  >=fver(10,0,0,3802) /* WM10L Section */
		                 && _wmspdmod_dll < fver(10,0,0,4070))

					  || (_wmspdmod_dll  >=fver(10,0,0,4332) /* MCEUR2 Section */
		                 && _wmspdmod_dll < fver(10,0,0,4365))

					  || (_wmspdmod_dll  >=fver(11,0,5721,5145) /* WM11 Section */
		                 && _wmspdmod_dll < fver(11,0,5721,5263))					   
					   )) {
		NN("Security Update for Windows Media Format Runtime 9, 9.5 & 11 for Windows XP SP 3 (KB954155)");
		XX(p+"WindowsXP-WindowsMedia-KB954155-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ((_wmp_dll >=fver(9,0,0,2980) /* WM9 Section */
					     && _wmp_dll<fver(9,0,0,4510))

					  || (_wmp_dll  >=fver(10,0,0,3646) /* WM10L Section */
		                 && _wmp_dll < fver(10,0,0,4081))

					  || (_wmp_dll  >=fver(11,0,5721,5145) /* WM11 Section */
		                 && _wmp_dll < fver(11,0,5721,5280))					   
					   )) {
		NN("Security Update for Windows XP (KB2378111)");
		XX(p+"WindowsXP-WindowsMedia-KB2378111-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _wmp_dll>=fver(9,0,0,0) && _wmp_dll<fver(9,0,0,4513)) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3033890)");
		XX(p+"windowsmedia9-kb3033890-x86-enu_cd51ca12cb3275c117784ab38eced274b61012bd.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _wmp_dll>=fver(10,0,0,0) && _wmp_dll<fver(10,0,0,4092)) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3033890)");
		XX(p+"windowsmedia10-kb3033890-x86-enu_ef4e82e7007462fb64ed10c4e127d828e5126efc.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _wmp_dll>=fver(11,0,5721,0) && _wmp_dll<fver(11,0,5721,5293)) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3033890)");
		XX(p+"windowsmedia11-kb3033890-x86-enu_3da047c27ca7b35c2a15e2bdf4a87fd7243e2b49.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _l3codecx_ax>zero && _l3codecx_ax<fver(1,6,0,52)) {
		NN("Security Update for Windows XP (KB2115168)");
		XX(p+"WindowsXP-KB2115168-x86-ENU.exe"+a1);
	}
	if( sp>=2 && *wmp6cdcs) {
		NN("Codecs Package for Windows Media Player 6.4");
		XX(p+"wmp6cdcs.EXE /Q /C:\"setup_wm.exe /Q /R:N\"");
	}
	if( sp>=2 && (sku & XP_ALL) && ( *wmp6cdcs || (_wmavds32_ax>=fver(9,0,0,3224) && _wmavds32_ax<fver(9,0,0,3360)))) {
		NN("Security Update for Windows 2000, Windows XP and Windows 2003 (KB969878)");
		XX(p+"windowsmedia9-kb969878-x86-enu_c512a3bf6fb3be974534bbaf4dacb78808741128.exe"+a1);
	}
	if( sp>=2 && (sku & XP_ALL) && ( *wmp6cdcs || (_msaud32_acm>zero && _msaud32_acm<fver(8,0,0,4502)))) {
		NN("Security Update for Windows XP (KB975025)");
		XX(p+"WindowsXP-KB975025-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ( *wmp6cdcs || (_wmv9vcm_dll>=fver(9,0,1,369) && _wmv9vcm_dll<fver(9,0,1,3073)))) {
		NN("Security Update for Windows XP (KB2845142)");
		XX(p+"windowsxp-windowsmedia-kb2845142-x86-enu_560190cc172d1009450366fa48a274f2ae0672a7.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && ((_mpg4ds32_ax >=fver(8,0,0,0) /* WM8 Section */
					     && _mpg4ds32_ax<fver(8,0,0,4504))

					  || (_mp4sds32_ax  >=fver(8,0,0,0) /* V8DME Section */
		                 && _mp4sds32_ax < fver(8,0,0,406))

					  || (_mp4sdmod_dll  >=fver(9,0,0,0) /* WM9 Section */
		                 && _mp4sdmod_dll < fver(9,0,0,4509))

					  || (_mp4sdmod_dll >=fver(10,0,0,0) /* WM10 Section */
		                 && _mp4sdmod_dll < fver(10,0,0,3706))

					  || (_mp4sdecd_dll  >=fver(11,0,5721,0) /* WM11 Section */
		                 && _mp4sdecd_dll < fver(11,0,5721,5274))

					  || *wmp6cdcs   )) {
		NN("Security Update for Windows XP (KB975558)");
		XX(p+"WindowsXP-WindowsMedia-KB975558-x86-ENU.exe"+a1);
	}
	if( sp>=2 && (sku & XP_ALL) && ( _unregmp2_exe >= fver(11,0,5721,5145) && _unregmp2_exe < fver(11,0,5721,5235))) {
		NN("Update for Windows Media Player 11 for Windows XP (KB939683)");
		XX(p+"windowsmedia11-kb939683-x86-enu_17d48a1cb35ad4f1cac39bd758158efe690730b5.exe"+a1);
	}
	if( sp==3 && !kb913800 && (sku & XP_ALL) && _cewmdm_dll>=fver(10,0,3790,3646) && _cewmdm_dll<fver(10,0,3790,3747)) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3067903)");   /* WM10 Version */
		XX(p+"windowsmedia-kb3067903-x86-enu_65c7ef0682d0b3b8a23e579145728ac9d27ea242.exe"+a1);
	}
	if( sp==3 && !kb913800 && (sku & XP_ALL) && _cewmdm_dll>=fver(10,0,3790,3802) && _cewmdm_dll<fver(10,0,3790,4093)) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3067903)");   /* WM10L Version */
		XX(p+"windowsmedia-kb3067903-x86-enu_17cadd59515063d00d9f0d92d023ae1aa2349e4e.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _cewmdm_dll>=fver(11,0,5721,5145) && _cewmdm_dll<fver(11,0,5721,5295)) {
		NN("Security Update for WEPOS and POSReady 2009 (KB3067903)");   /* WM11 Version */
		XX(p+"windowsmedia-kb3067903-x86-enu_510155e48bfc1c08e7f2023fde514be960fa8fed.exe"+a1);
	}
	if( sp>=2 && (sku & XP_ALL) && _npdsplay_dll>zero && _npdsplay_dll<fver(3,0,2,629)) {
		NN("Security Update for Windows Media Player Plug-in (KB911564)");
		XX(p+"windowsmedia-kb911564-x86-enu_8f5e0c3d5c50c32200a78dab3ccadb175649858c.exe"+a1);
	}
	if( sp>=2 && !kb913800 && (sku & XP_ALL)
			&& ( kb891122 || _wmvcore_dll >= fver(10,0,0,3802))
		    && ((kb891122 || _msscp_dll   >= fver(10,0,0,3646)) && _msscp_dll<fver(10,0,0,3934)) ) {
		NN("Update for WMDRM-enabled Media Players (KB902344)");
		XX(p+"WindowsMedia10-KB902344-x86-INTL.exe"+a1);
	}
	if( sp>=2 && (sku & XP_ALL) && _msscp_dll>=fver(11,0,5721,5145) && _msscp_dll<fver(11,0,5721,5201)) {
		NN("Update for Windows Media Format 11 SDK for Windows XP (KB929399)");
		XX(p+"windowsmedia11-kb929399-v2-x86-intl_50e2bc6b5e79909a0061043e6211600548368431.exe"+a1);
	}

	if( sp==3 && (sku & XP_ALL) && _infocomm_dll>zero && _infocomm_dll<fver(6,0,2600,6018)) {
		NN("Security Update for Windows XP (KB2290570)");
		XX(p+"WindowsXP-KB2290570-x86-ENU.exe"+a1);
	}
	if((sp==2 && (sku & XP_ALL) && ( (_msw3prt_dll >zero && _msw3prt_dll <fver(5,1,2600,3435))
	                             ||  (_win32spl_dll>zero && _win32spl_dll<fver(5,1,2600,3435)) ))
	 ||(sp==3 && (sku & XP_ALL) && ( (_msw3prt_dll> zero && _msw3prt_dll <fver(5,1,2600,5664))
	                             ||  (_win32spl_dll>zero && _win32spl_dll<fver(5,1,2600,5664)) )) ) {
		NN("Security Update for Windows XP (KB953155)");
		XX(p+"WindowsXP-KB953155-x86-ENU.exe"+a1);
	}
	if((sp==2 && (sku & XP_ALL) && _httpext_dll>zero && _httpext_dll<fver(6,0,2600,3574))
	 ||(sp==3 && (sku & XP_ALL) && _httpext_dll>zero && _httpext_dll<fver(6,0,2600,5817))) {
		NN("Security Update for Windows XP (KB970483)");
		XX(p+"WindowsXP-KB970483-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _asp_dll>zero && _asp_dll<fver(5,1,2600,6007)) {
		NN("Security Update for Windows XP (KB2124261)");
		XX(p+"WindowsXP-KB2124261-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XPE_2009) && _httpext_dll>zero && _httpext_dll<fver(6,0,2600,7150)) {
		NN("Security Update for WES09 and POSReady 2009 (KB3197835)");
		XX(p+"windowsxp-kb3197835-x86-embedded-enu_96f4567c9e6d92956a85f4c8cbddb66cb6d8c8b3.exe"+a1);
	}
	if((sp==2 && (sku & XP_ALL) && _ftpsvc2_dll>zero && _ftpsvc2_dll<fver(6,0,2600,3624))
	 ||(sp==3 && (sku & XP_ALL) && _ftpsvc2_dll>zero && _ftpsvc2_dll<fver(6,0,2600,5875))) {
		NN("Security Update for Windows XP (KB975254)");
		XX(p+"WindowsXP-KB975254-x86-ENU.exe"+a1);
	}
	if( sp==3 && (sku & XP_ALL) && _fxscover_exe>zero && _fxscover_exe<fver(5,2,2600,6078)) {
		NN("Security Update for Windows XP (KB2491683)");
		XX(p+"WindowsXP-KB2491683-x86-ENU.exe"+a1);
	}
	if((sp==2 && (sku & XP_ALL) && _smtpsvc_dll>zero && _smtpsvc_dll<fver(6,0,2600,3680))
	 ||(sp==3 && (sku & XP_ALL) && _smtpsvc_dll>zero && _smtpsvc_dll<fver(6,0,2600,5949))) {
		NN("Security Update for Windows XP (KB976323)");
		XX(p+"WindowsXP-KB976323-x86-ENU.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && (
		                  (_magnify_exe  >zero && _magnify_exe  <fver(5,1,2600,3008))
		              ||  (_narrator_exe >zero && _narrator_exe <fver(5,1,2600,3008))
					  ||  (_osk_exe      >zero && _osk_exe      <fver(5,1,2600,3008))
					  ||  (_umandlg_dll  >zero && _umandlg_dll  <fver(5,1,2600,3008))
					  ||  (_utilman_exe  >zero && _utilman_exe  <fver(5,1,2600,3008)) )) {
		NN("February 2007 CardSpace Update for Windows XP (KB925720)");
		XX(p+"windowsxp-kb925720-x86-enu_b9966c90242454bce1f2edb066141ad45851008f.exe"+a1);
	}
	
	if( sp==2 && *rdp60) {
		NN("Remote Desktop Connection (Terminal Services Client 6.0) for Windows XP (KB925876)");
		XX(p+"windowsxp-kb925876-x86-enu_402612fb3d6636c721ff8d8edc0c4241aa68e477.exe"+a1);
	}
	if( sp>=2 && *rdp61) {
		NN("Remote Desktop Connection (Terminal Services Client 6.1) for Windows XP (KB952155)");
		XX(p+"WindowsXP-KB952155-x86-ENU.exe"+a1);
	}
	if( sp==2 && !(*rdp60 || *rdp61) && ((sku & XP_ALL) && (_mstscax_dll >= fver(5,1,0,0) && _mstscax_dll<=fver(5,2,0,0)))
		              && (
					  /* GDR and QFE */
					      (_mstscax_dll     >zero && _mstscax_dll  <fver(5,1,2600,3581))
					  /*||  (_msrdp_ocx       >zero && _msrdp_ocx    <fver(5,2,3790,4522))*/ )) {
		NN("Security Update for Windows XP (KB958470)");
		XX(p+"windowsxp-kb958470-x86-enu_887a259c39636eaf1ab9bfec71172203224415cc.exe"+a1);
	}
	if( sp==2 && (sku & XP_ALL) && (
		// Update for RDP 6.0
		(*rdp60 || ( _mstscax_dll >= fver(6,0,6000,0) && _mstscax_dll<fver(6,0,6000,16865)))

		// Update for RDP 6.1
	 || (*rdp61 || ( _mstscax_dll >= fver(6,0,6001,0) && _mstscax_dll<fver(6,0,6002,0))
			&& ( (_aaclient_dll     >zero && _aaclient_dll     <fver(6,0,6001,22443))
			  || (_aaclient_dll_mui >zero && _aaclient_dll_mui <fver(6,0,6001,22403))
			  || (_mstsc_exe        >zero && _mstsc_exe        <fver(6,0,6001,22443))
			  || (_mstsc_exe_mui    >zero && _mstsc_exe_mui    <fver(6,0,6001,22403))
			  || (_mstscax_dll      >zero && _mstscax_dll      <fver(6,0,6001,22443))
			  || (_mstscax_dll_mui  >zero && _mstscax_dll_mui  <fver(6,0,6001,22403))
			  || (_rhttpaa_dll      >zero && _rhttpaa_dll      <fver(6,0,6001,22443))
			  || (_tsgqec_dll       >zero && _tsgqec_dll       <fver(6,0,6001,22443))
			)))) { // Also installs on SP3, but superceded by KB2813345 on that platform
		NN("Security Update for Windows XP (KB956744)");
		XX(p+"windowsxp-kb956744-x86-enu_3d8a4f0990d13778983b5f2c4dcb982d4d7bfd20.exe"+a1);
	}
	if( sp==3 && *rdp70){
		NN("Remote Desktop Connection 7.0 client update for Windows XP (KB969084)");
		XX(p+"windowsxp-kb969084-x86-enu_153587172bf060ddc9d2c966f1eaaf4f47926f28.exe"+a1);
	}
	if( sp==3 && (*rdp70 || ((sku & XP_ALL) && (_mstscax_dll >= fver(6,1,7600,16385) && _mstscax_dll<fver(6,1,7601,0))
		              && (
					  /* GDR */
					      (_aaclient_dll    >zero && _aaclient_dll <fver(6,1,7600,17233))
		              ||  (_mstsc_exe       >zero && _mstsc_exe    <fver(6,1,7600,16722))
					  ||  (_mstscax_dll     >zero && _mstscax_dll  <fver(6,1,7600,17233))
					  ||  (_tsgqec_dll      >zero && _tsgqec_dll   <fver(6,1,7600,17233))
					  ||  (_wksprt_exe      >zero && _wksprt_exe   <fver(6,1,7600,16385))
					  /* QFE */
					  ||  (_aaclient_dll    >fver(6,1,7600,17233) && _aaclient_dll    <fver(6,1,7600,21448))
		              ||  (_mstsc_exe       >fver(6,1,7600,16722) && _mstsc_exe       <fver(6,1,7600,21448))
					  ||  (_mstscax_dll     >fver(6,1,7600,17233) && _mstscax_dll     <fver(6,1,7600,21448))
					  ||  (_mstscax_dll_mui >fver(6,1,7600,16385) && _mstscax_dll_mui <fver(6,1,7600,21448))
					  ||  (_tsgqec_dll      >fver(6,1,7600,17233) && _tsgqec_dll      <fver(6,1,7600,21448))
					  ||  (_wksprt_exe      >fver(6,1,7600,16385) && _wksprt_exe      <fver(6,1,7600,21448)) )))) {
		NN("Security Update for Windows XP (KB2813347)");
		XX(p+"windowsxp-kb2813347-v2-x86-enu_7f5b07e5464702cdfa24a869d21f55335223069a.exe"+a1);
	}
	if( sp==3 && (!*rdp70 && ((sku & XP_ALL) && (_mstscax_dll >= fver(6,0,6001,0) && _mstscax_dll<=fver(6,0,6002,0))
		              && ( *rdp61
					  /* GDR */
					  ||  (_mstsc_exe       >zero && _mstsc_exe    <fver(6,0,6001,18589))
					  ||  (_mstscax_dll     >zero && _mstscax_dll  <fver(6,0,6001,18589))
					  /* QFE */
					  ||  (_aaclient_dll    >fver(6,0,6001,18000) && _aaclient_dll    <fver(6,0,6001,22840))
		              ||  (_mstsc_exe       >fver(6,0,6001,18589) && _mstsc_exe       <fver(6,0,6001,22840))
					  ||  (_mstscax_dll     >fver(6,0,6001,18926) && _mstscax_dll     <fver(6,0,6001,22840))
					  ||  (_tsgqec_dll      >fver(6,0,6001,18000) && _tsgqec_dll      <fver(6,0,6001,22840)) )))) {
		NN("Security Update for Windows XP (KB2481109)");
		XX(p+"WindowsXP-KB2481109-x86-ENU.exe"+a1);
	}
	if( sp==3 && (!*rdp70 && ((sku & XP_ALL) && (_mstscax_dll >= fver(6,0,6001,0) && _mstscax_dll<=fver(6,0,6002,0))
		              && ( *rdp61
					  /* GDR */
					  ||  (_mstscax_dll >zero && _mstscax_dll <fver(6,0,6001,18926))
					  /* QFE */
					  ||  (_aaclient_dll    >fver(6,0,6001,18000) && _aaclient_dll    <fver(6,0,6001,23191))
					  ||  (_mstscax_dll     >fver(6,0,6001,18926) && _mstscax_dll     <fver(6,0,6001,23191))
					  ||  (_tsgqec_dll      >fver(6,0,6001,18000) && _tsgqec_dll      <fver(6,0,6001,23191)) )))) {
		NN("Security Update for Windows XP (KB2813345)");
		XX(p+"WindowsXP-KB2813345-x86-ENU.exe"+a1);
	}

	if( sp>=2 && (*imapi2 || (   (sku & XP_ALL)
		            && (_imapi2_dll >zero && _imapi2fs_dll >zero)
		            && (
						(_cdrom_sys   <fver(5,1,2600,3126))
		            ||  (_imapi2_dll  <fver(6,0,6001,18164))
					||  (_imapi2fs_dll<fver(6,0,6001,18164)) )))) {
		NN("Windows Feature Pack for Storage (32-bit) - IMAPI update for Blu-Ray (KB952011)");
		XX(p+"IMAPI_XP_SRV2003_x86.exe"+a1);
	}
	if( sp>=2 && (*smartcard || (   (sku & XP_ALL)
		            && (_winusbcoinstaller_dll >zero && _wudfupdate_01007_dll >zero
					     && _wudfusbcciddriver_dll >zero)
		            && (
						(_WdfCoInstaller01007_dll <fver(1,7,6001,18000))
					||  (_winusbcoinstaller_dll <fver(6,0,5841,16388))
		            ||  (_wudfupdate_01007_dll  <fver(6,0,6001,18000))
					||  (_wudfusbcciddriver_dll <fver(6,0,6001,18158)) )))) {
		NN("Windows Feature Pack for Storage (32-bit) - Smart Card driver (KB952013)");
		XX(p+"SmartCard_XP_x86.exe"+a1);
	}
	if( sp>=2 && (*wsearch4 || (   (sku & XP_ALL)
		            && (_searchfilterhost_exe >=fver(7,0,6001,16503) && _searchindexer_exe >=fver(7,0,6001,16503)
					     && _searchprotocolhost_exe >=fver(7,0,6001,16503))
		            && (
						(_dbres_dll                <fver(7,0,6001,16503))
		            ||  (_dbres_dll_mui            <fver(7,0,6001,16503))
					||  (_dbsetup_dll              <fver(7,0,6001,16503))
					||  (_deskbar_dll              <fver(7,0,6001,16503))
					||  (_mapine_dll               <fver(7,0,6001,16503))
					||  (_mapineres_dll            <fver(7,0,6001,16503))
					||  (_mapineres_dll_mui        <fver(7,0,6001,16503))
					||  (_msnldl_dll               <fver(7,0,6001,16503))
					||  (_msnldlps_dll             <fver(7,0,6001,16503))
					||  (_msnlext_dll              <fver(7,0,6001,16503))
					||  (_msnlextres_dll           <fver(7,0,6001,16503))
					||  (_msnlextres_dll_mui       <fver(7,0,6001,16503))
					||  (_msnlnamespacemgr_dll     <fver(7,0,6001,16503))
					||  (_msnlred_dll              <fver(7,0,6001,16503))
					||  (_msscb_dll                <fver(7,0,6001,16503))
					||  (_msscntrs_dll             <fver(7,0,6001,16503))
					||  (_msshooks_dll             <fver(7,0,6001,16503))
					||  (_msshsq_dll               <fver(7,0,6001,16503))
					||  (_mssitlb_dll              <fver(7,0,6001,16503))
					||  (_mssph_dll                <fver(7,0,6001,16503))
					||  (_mssph_dll_mui            <fver(7,0,6001,16503))
					||  (_mssphtb_dll              <fver(7,0,6001,16503))
					||  (_mssphtb_dll_mui          <fver(7,0,6001,16503))
					||  (_mssprxy_dll              <fver(7,0,6001,16503))
					||  (_mssrch_dll               <fver(7,0,6001,16503))
					||  (_mssrch_dll_mui           <fver(7,0,6001,16503))
					||  (_msstrc_dll               <fver(7,0,6001,16503))
					||  (_oeph_dll                 <fver(7,0,6001,16503))
					||  (_oephres_dll              <fver(7,0,6001,16503))
					||  (_oephres_dll_mui          <fver(7,0,6001,16503))
					||  (_propdefs_dll             <fver(7,0,6001,16503))
					||  (_propsys_dll              <fver(7,0,6001,16503))
					||  (_propsys_dll_mui          <fver(7,0,6001,16503))
					||  (_searchfilterhost_exe     <fver(7,0,6001,16503))
					||  (_searchindexer_exe        <fver(7,0,6001,16503))
					||  (_searchindexer_exe_mui    <fver(7,0,6001,16503))
					||  (_searchprotocolhost_exe   <fver(7,0,6001,16503))
					||  (_srchadmin_dll            <fver(7,0,6001,16503))
					||  (_srchadmin_dll_mui        <fver(7,0,6001,16503))
					||  (_tquery_dll               <fver(7,0,6001,16503))
					||  (_tquery_dll_mui           <fver(7,0,6001,16503))
					||  (_unccplext_dll            <fver(7,0,6001,16503))
					||  (_uncdms_dll               <fver(7,0,6001,16503))
					||  (_uncne_dll                <fver(7,0,6001,16503))
					||  (_uncph_dll                <fver(7,0,6001,16503))
					||  (_uncres_dll               <fver(7,0,6001,16503))
					||  (_uncres_dll_mui           <fver(7,0,6001,16503))
					||  (_wdsmkttools_dll          <fver(7,0,6001,16503))
					||  (_wdsshell_dll             <fver(7,0,6001,16503))
					||  (_wdsview_dll              <fver(7,0,6001,16503))
					||  (_windowssearch_exe        <fver(7,0,6001,16503))
					||  (_windowssearchres_dll     <fver(7,0,6001,16503))
					||  (_windowssearchres_dll_mui <fver(7,0,6001,16503))
					||  (_wordwheel_dll            <fver(7,0,6001,16503))
					||  (_wwres_dll                <fver(7,0,6001,16503))
					||  (_wwres_dll_mui            <fver(7,0,6001,16503))
					||  (_xppreviewproxy_dll       <fver(7,0,6001,16503))
					||  (_xmlfilter_dll            <fver(2006,0,6001,16503))
					||  (_rtffilt_dll              <fver(2006,0,6001,16503)) )))) {
		NN("Windows Search 4.0 for Windows XP (KB940157)");
		XX(p+"WindowsSearch-KB940157-XP-x86-enu.exe"+a4);
	}
	if( sp>=2 && (*wsearch4 || ((sku & XP_ALL) && ((   _searchfilterhost_exe >=fver(7,0,6001,16503)
		                                 && _searchindexer_exe >=fver(7,0,6001,16503)
					                     && _searchprotocolhost_exe >=fver(7,0,6001,16503))
							|| (regQueryValue(L"SOFTWARE\\Microsoft\\Windows Search",L"CurrentVersion",&status)==L"04.00.6001.503"))
		              && ((_mapine_dll           <fver(7,0,6001,18260))
		              ||  (_msnlext_dll          <fver(7,0,6001,18260))
					  ||  (_msnlnamespacemgr_dll <fver(7,0,6001,18260))
					  ||  (_mssph_dll            <fver(7,0,6001,18260)))))) {
		// KB963093: This security update resolves a privately reported vulnerability in Windows
		// Search. The vulnerability could allow information disclosure if a user performs a
		// search that returns a specially crafted file as the first result or if the user
		// previews a specially crafted file from the search results.
		NN("Security Update for Windows XP (KB963093)");
		XX(p+"WindowsServer2003.WindowsXP-KB963093-x86-ENU.exe"+a1);
	}

	if( sp>=2 && !(sku & XP_TABLET) && (*jview || (_jntview_exe>zero && _jntview_exe<fver(1,5,2315,3)))) {
		NN("Microsoft Windows Journal Viewer 1.5");
		XX(p+"JournalViewer1.5.exe /Q /R:N /C:\"msiexec /i \"\"Microsoft Windows Journal Viewer.msi\"\" /passive\"");
	}
	if( sp==2 && (sku & XP_TABLET) && (
		                  (_ContextTagger_dll >zero && _ContextTagger_dll <fver(1,7,2600,2663))
		              ||  (_TabTip_exe        >zero && _TabTip_exe        <fver(1,7,2600,2663))
					  ||  (_TCServer_exe      >zero && _TCServer_exe      <fver(1,7,2600,2663))
					  ||  (_TipLibrary_dll    >zero && _TipLibrary_dll    <fver(1,7,2600,2657))
					  ||  (_tiptsf_dll        >zero && _tiptsf_dll        <fver(1,7,2600,2663)) )) {
		NN("Update for Windows XP Tablet PC Edition 2005 (KB895953)");
		XX(p+"windowsxp-kb895953-v4-x86-enu_9e8006f0a1303efca8503b73f9a4243cfba236ff.exe"+a1);
	}
	if( sp==3 && *pshell) {
		NN("Windows Powershell 2.0 and WinRM 2.0 for Windows XP and Windows Embedded (KB968930)");
		XX(p+"WindowsXP-KB968930-x86-ENG.exe"+a1);
	}
	if( sp>=2 && (*bitlocker || (_BitLockerToGo_exe>zero && _BitLockerToGo_exe<fver(6,1,7600,16385)))) {
		NN("BitLocker To Go Reader (KB970401)");
		XX(p+"KB970401-x86.exe"+a1);
	}
	if( sp>=2 && (*winrms ||   (     (_msdrm_dll              >zero && _msdrm_dll              <fver(5,2,3790,433))
		              ||  (_RmActivate_exe         >zero && _RmActivate_exe         <fver(6,0,6406,0))
					  ||  (_RmActivate_isv_exe     >zero && _RmActivate_isv_exe     <fver(6,0,6406,0))
					  ||  (_RmActivate_ssp_exe     >zero && _RmActivate_ssp_exe     <fver(6,0,6406,0))
					  ||  (_RmActivate_ssp_isv_exe >zero && _RmActivate_ssp_isv_exe <fver(6,0,6406,0))
					  ||  (_SecProc_dll            >zero && _SecProc_dll            <fver(6,0,6406,0))
					  ||  (_SecProc_isv_dll        >zero && _SecProc_isv_dll        <fver(6,0,6406,0))
					  ||  (_SecProc_ssp_dll        >zero && _SecProc_ssp_dll        <fver(6,0,6406,0))
					  ||  (_SecProc_ssp_isv_dll    >zero && _SecProc_ssp_isv_dll    <fver(6,0,6406,0))))) {
		NN("Rights Management Services Client with Service Pack 2 for Windows XP (KB979099)");
		//XX(sw+p+"windowsrightsmanagementservicessp2-kb979099-client-x86-enu_c57a952c1c55300114000b769e6f97e8a44322e3.exe -override 1 /I MsDrmClient.msi REBOOT=ReallySuppress /q -override 2 /I RmClientBackCompat.msi REBOOT=ReallySuppress /q");
		XX(sw+p+"windowsrightsmanagementservicessp2-kb979099-client-x86-enu_c57a952c1c55300114000b769e6f97e8a44322e3.exe -override 1 /I MsDrmClient.msi REBOOT=ReallySuppress /passive -override 2 /I RmClientBackCompat.msi REBOOT=ReallySuppress /passive");
	}
	if( sp>=2 && (*msxml4 || ((sku & XP_ALL) && (
		                  (_msxml4_dll  >zero && _msxml4_dll  <fver(4,30,2117,0))
					  ||  (_msxml4r_dll >zero && _msxml4r_dll <fver(4,30,2100,0)))))) {
		NN("Security Update for Microsoft XML Core Services 4.0 Service Pack 3 (KB2758694)");
		XX(p+"msxml4-kb2758694-enu_24abccbcceaf5bea9c3e34ff1f64c2aa3d57e308.exe"+a3);
	}
	/*if( sp>=2 && (*msxml6 || ((sku & XP_ALL) && ( 
		                  ( _msxml6_dll  >zero && _msxml6_dll  <fver(6,10,1129,0))
					  ||  ( _msxml6r_dll >zero && _msxml6r_dll <fver(6,0,3883,0)))))) {
		NN("Update for Microsoft Core XML Services (MSXML) 6.0 Service Pack 1 (KB934268)");
		XX(p+"msxml6sp1-kb934268-enu-x86_dac3db6bec59d839c713a6a2d1e169fd7dec1e25.exe"+a3);
	}
	if( sp>=2 && (*msxml6 || ((sku & XP_ALL) && ( 
		                  ( _msxml6_dll  >zero && _msxml6_dll  <fver(6,20,1099,0))))) {
		NN("Security Update for Microsoft XML Core Services 6.0 Service Pack 2 (KB954459)");
		NN(p+"windowsxp-kb954459-x86-enu_a409813d4541734043c419d399d20a463b52f8e1.exe"+a1);
	}*/
	if( sp>=2 && (*msxml6 || ((sku & XP_ALL) && ( 
		                  ( _msxml6_dll  >zero && _msxml6_dll  <fver(6,20,2003,0))
					  ||  ( _msxml6r_dll >zero && _msxml6r_dll <fver(6,0,3883,0)))))) {
		NN("Update for Microsoft XML Core Services 6.0 Service Pack 2 (KB973686)");
		XX(p+"msxml6-kb973686-enu-x86_e139664a78bc2806cf0c5bcf0bedec7ea073c3b1.exe"+a3);
	}

	if( sp>=2 && ((sku & XP_ALL) && _agcore_debug_dll>zero
		              && ( _slup_exe>zero || _sllauncher_exe>zero || _agcore_dll>zero )
		              &&   _sllauncher_exe<fver(5,1,50918,0) )) {
		NN("Update for Microsoft Silverlight (KB4481252)");
		XX(sw+p+"silverlight_developer_16882b33f82a87224e6b6dd1150ca5665e6f309d.exe"+a2l);
	}
	else if( sp>=2 && (*silverlight || ((sku & XP_ALL) && ( _slup_exe>zero || _sllauncher_exe>zero || _agcore_dll>zero )
					  &&   _sllauncher_exe<fver(5,1,50918,0)))) {
		NN("Update for Microsoft Silverlight (KB4481252)");
		XX(sw+p+"silverlight_a6ea175976edbdeeddfbfb65e64d9d1cfded84a1.exe"+a2l);
	}
	

	if( sp>=2 && *msi45 ) {
		NN("Microsoft Windows Installer 4.5 (KB942288)");
		XX(p+"WindowsXP-KB942288-v3-x86.exe"+a1);
	}
	if( !*msi45 && (sku & XP_ALL) && (  _msi_dll      <fver(3,1,4000,2435)
					  ||    _msiexec_exe  <fver(3,1,4000,1823)
					  ||    _msihnd_dll   <fver(3,1,4000,1823)
					  ||    _msimsg_dll   <fver(3,1,4000,1823)
					  ||    _msisip_dll   <fver(3,1,4000,1823) )) {
		NN("Microsoft Windows Installer 3.1");
		XX(p+"WindowsInstaller-KB893803-v2-x86.exe"+a1);
	}
	if( sp==2 && !*msi45 && (sku & XP_ALL) && (  _msi_dll <fver(3,1,4000,4039) )) {
		NN("Update for Windows XP (KB927891)");
		XX(p+"windowsxp-kb927891-v3-x86-enu_831fc2b9075b0a490adf15d2c5452e01e6feaa17.exe"+a1);
	}
	if( sp==3 && (sku & XPE_WES2009) && ((_msi_dll>=fver(4,5,0,0) && _msi_dll<fver(4,5,6002,24556))
		              ||  (_msiexec_exe>=fver(4,5,0,0) && _msiexec_exe<fver(4,5,6002,24433))
					  ||  (_msihnd_dll>=fver(4,5,0,0) && _msihnd_dll<fver(4,5,6002,23415))
					  ||  *msi45 )) {
		NN("2019-02 Security Update for WES09 and POSReady 2009 for x86-based Systems (KB4486464)");
		XX(p+"windowsxp-kb4486464-x86-embedded-enu_ba779c434ec819e2932b7f70422b74c526ae244d.exe"+a1);
	}
	if( sp==3 && (sku & XPE_POSREADY2009) && ((_msi_dll>=fver(4,5,0,0) && _msi_dll<fver(4,5,6003,20488))
		              ||  (_msiexec_exe>=fver(4,5,0,0) && _msiexec_exe<fver(4,5,6002,24433))
					  ||  (_msihnd_dll>=fver(4,5,0,0) && _msihnd_dll<fver(4,5,6002,23415))
					  ||  *msi45 )) {
		// Out of the box, update KB4494528 messes up Windows Installer.
		// Fixing it requires manually re-registering a few MSI DLLs.
		NN("2019-04 Security Update for POSReady 2009 for x86-based Systems (KB4494528)");
		XX(p+"windowsxp-kb4494528-x86-embedded-enu_77ead5923f0753b90d935f2d6546886b063a6fe1.exe"+a1
			+"\n"
			+"regsvr32.exe /s %SystemRoot%\\system32\\MSI.DLL\n"
			+"regsvr32.exe /s %SystemRoot%\\system32\\MSIHND.DLL\n"
			+"regsvr32.exe /s %SystemRoot%\\system32\\MSISIP.DLL");
	}
}
