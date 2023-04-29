#include <string>
#include <iostream>
#include <fstream> // ���ļ� 
#include <windows.h> // ���������
#include <filesystem> // �����ļ����ڼ�� 

#include "tools.h"
#include "tools_flags.h" // ��־
#include "opencv2/imgproc.hpp" // ɫ�ʿռ�ת��

// ����ת��
#include<codecvt>
std::wstring_convert<std::codecvt_utf8<wchar_t>> conv_Ustr_Wstr; // string utf-8 �� wstring utf-16 ��˫��ת����

using namespace std;
using namespace nlohmann;

namespace tool {
    // ==================== ������� ====================
    // �����������ǰ�趨 
    bool ensureAscii = false; // ���json�Ƿ�תascii 
    bool ensureLogger = false; // ������־��� 
    string modelsDir = "models"; // ģ�Ϳ�Ŀ¼ 
    string modelDetPath = "ch_PP-OCRv3_det_infer.onnx"; // Ĭ�ϼ��ģ�� 
    string modelClsPath = "ch_ppocr_mobile_v2.0_cls_infer.onnx"; // Ĭ�Ϸ������ģ�� 
    string modelRecPath = "ch_PP-OCRv3_rec_infer.onnx"; // Ĭ��ʶ��ģ�� 
    string keysPath = "ppocr_keys_v1.txt"; // Ĭ���ֵ� 
    int numThread = 4; // �߳��� 
    int flagGpu = -1; // ʹ�õ�GPU��ţ���ʹ��Ϊ-1 
    // �Ȳ�������������޸� 
    int padding = 50; // Ԥ����ױ߿�� 
    int maxSideLen = 1024; // ������������ֵ 
    float boxScoreThresh = 0.5f; // ���ֿ����Ŷ����� 
    float boxThresh = 0.3f; 
    float unClipRatio = 1.6f; // �������ֿ��С���ʣ�Խ��ʱ�������ֿ�Խ��
    bool doAngle = true; // �������ַ����� 
    bool mostAngle = true; // ���ýǶ�ͶƱ


    // ==================== ִ��״̬ ====================

    int ToolCode = 0; // ���汾�غϴ�����
    string ToolMsg = ""; // ���汾�غϴ�����ʾ
    // ��ȡ״̬
    void get_state(int& code, string& msg) {
        code = ToolCode;
        msg = ToolMsg;
    }
    // ����״̬
    void set_state(int code, string msg) {
        ToolCode = code;
        ToolMsg = msg;
    }
    // ר��������Ϣ��wstringתstring��ת��ʧ��ʱ����Ĭ����ʾ����
    string msg_wstr_2_ustr(wstring& msg) {
        try {
            string msgU8 = conv_Ustr_Wstr.to_bytes(msg); // ת��u8
            return msgU8;
        }
        catch (...) {
            return "wstring failed to convert to utf-8 string";
        }
    }
    // ���ֽ�ANSI�ַ�����ת���ַ�����
    wchar_t* char_2_wchar(char* c) {
        setlocale(LC_ALL, ""); // �������������Ϊwindowsϵͳ��ǰ����
        size_t lenWchar = mbstowcs(NULL, c, 0); // �õ�תΪ���ַ����ĳ���
        wchar_t* wc = new wchar_t[lenWchar + 1]; // ����ļ����Ŀ��ַ���
        int n = mbstowcs(wc, c, lenWchar + 1); // ���ֽ�ת���ַ�
        setlocale(LC_ALL, "C"); // ��ԭ��������ΪĬ��
        return wc;
    }

	// ==================== JSON��� ==================== 

	// ���һ��json 
	void print_json(const json& j) { 
		try {
			cout << j.dump(-1, ' ', ensureAscii) << endl;
		}
		catch (...) {
			json j2;
			j2["code"] = 300;
			j2["data"] = "JSON dump failed. Coding error.";
			cout << j2.dump(-1, ' ', ensureAscii) << endl;
		}
	}

	// ���������Ϣ 
	void print_ocr_fail(int code, const string& msg) {
		json j;
		j["code"] = code;
		j["data"] = msg;
		print_json(j);
	}

    // �����ǰ������Ϣ
    void print_now_fail()
    {
        int code;
        std::string msg;
        get_state(code, msg);
        if (code != CODE_INIT) { // �б����쳣
            print_ocr_fail(code, msg); // ���json
            return;
        }
        print_ocr_fail(CODE_ERR_UNKNOW, MSG_ERR_UNKNOW); // δ֪����
    }

    // {"image_path":"D:\Test\Test.png"}
    // ����һ��json������str_in�ַ�������str_in����Ϊjson�е�·��
    void load_json_str(string& str_in) {
        set_state(); // ����״̬����ʼ��
        string origin_str = str_in;
        bool is_image = false;
        try {
            auto j = json::parse(str_in); // תjson����
            for (auto& el : j.items()) { // ������ֵ 
                string value = to_string(el.value());
                int vallen = value.length();
                if (vallen > 2 && value[0] == '\"' && value[vallen - 1] == '\"') {
                    value = value.substr(1, vallen - 2); // ɾȥnlohmann�ַ�������������
                }
                if (el.key() == "imagePath") { // ͼƬ·��
                    str_in = value; // ֱ�ӷ���utf-8·��
                    is_image = true;
                }
                // �����������ݲ�����
            }
            if (!is_image) { // תjson�ɹ�����json��û��ͼƬ·�� 
                set_state(CODE_ERR_JSON_NO_IMAGE, MSG_ERR_JSON_NO_IMAGE(origin_str)); // ����״̬��δ�ҵ�ͼƬ·��
                str_in = "";
            }
        }
        catch (...) {
            set_state(CODE_ERR_LOAD_JSON, MSG_ERR_LOAD_JSON(origin_str)); // ����״̬�������л�JSONʧ��
            str_in = "";
        }
    }



    // ==================== ��ͼ ====================

    // ���·��pathW�Ƿ�Ϊ�ļ����Ƿ���true
    bool is_exists_wstr(wstring pathW) {
        struct _stat buf;
        int result = _wstat((wchar_t*)pathW.c_str(), &buf);
        if (result != 0) { // ��������
            return false;
        }
        if (S_IFREG & buf.st_mode) { // ���ļ�
            return true;
        }
        // else if (S_IFDIR & buf.st_mode) { // ��Ŀ¼
           //return false;
        // }
        return false;
    }

    // ���� cv::imread ����·��pathW����һ��ͼƬ��pathW����Ϊunicode��wstring
    cv::Mat imread_wstr(wstring pathW, int flags = cv::IMREAD_COLOR) {
        string pathU8 = msg_wstr_2_ustr(pathW); // ��ת��utf-8���Ա��������
        // �� �����������Ҫ��������CF_UNICODETEXT�ȸ��ã����ܵ��÷�ֻ���ṩwstring�����Զ��һ��ת��һ�Ρ�
        if (!is_exists_wstr(pathW)) { // ·��������
            set_state(CODE_ERR_PATH_EXIST, MSG_ERR_PATH_EXIST(pathU8)); // ����״̬��·�����������޷����
            return cv::Mat();
        }
        FILE* fp = _wfopen((wchar_t*)pathW.c_str(), L"rb"); // wpathǿ������ת����whar_t�����Դ��ļ�
        if (!fp) { // ��ʧ��
            set_state(CODE_ERR_PATH_READ, MSG_ERR_PATH_READ(pathU8)); // ����״̬���޷���ȡ
            return cv::Mat();
        }
        // ���ļ������ڴ�
        fseek(fp, 0, SEEK_END); // ������ fp ���ļ�λ��Ϊ SEEK_END �ļ���ĩβ
        long sz = ftell(fp); // ��ȡ�� fp �ĵ�ǰ�ļ�λ�ã����ܴ�С���ֽڣ�
        char* buf = new char[sz]; // ����ļ��ֽ�����
        fseek(fp, 0, SEEK_SET); // ������ fp ���ļ�λ��Ϊ SEEK_SET �ļ��Ŀ�ͷ
        long n = fread(buf, 1, sz, fp); // �Ӹ����� fp ��ȡ���ݵ� buf ��ָ��������У����سɹ���ȡ��Ԫ������
        cv::_InputArray arr(buf, sz); // ת��ΪOpenCV����
        cv::Mat img = cv::imdecode(arr, flags); // �����ڴ����ݣ����cv::Mat����
        delete[] buf; // �ͷ�buf�ռ�
        fclose(fp); // �ر��ļ�
        if (!img.data) {
            set_state(CODE_ERR_PATH_DECODE, MSG_ERR_PATH_DECODE(pathU8)); // ����״̬������ʧ��
        }
        return img;
    }

    // �Ӽ��������һ��ͼƬ��
    cv::Mat imread_clipboard(int flags = cv::IMREAD_COLOR) {
        // �ο��ĵ��� https://docs.microsoft.com/zh-cn/windows/win32/dataxchg/using-the-clipboard

        // ���Դ򿪼����壬��������ֹ����Ӧ�ó����޸ļ���������
        if (!OpenClipboard(NULL)) {
            set_state(CODE_ERR_CLIP_OPEN, MSG_ERR_CLIP_OPEN); // ����״̬���������ʧ��
        }
        else {
            static UINT auPriorityList[] = {  // �������ļ������ʽ��
              CF_BITMAP,                      // λͼ
              CF_HDROP,                       // �ļ��б������ļ�������ѡ���ļ����ƣ�
            };
            int auPriorityLen = sizeof(auPriorityList) / sizeof(auPriorityList[0]); // �б���
            int uFormat = GetPriorityClipboardFormat(auPriorityList, auPriorityLen); // ��ȡ��ǰ���������ݵĸ�ʽ
            // ���ݸ�ʽ���䲻ͬ����
            //     ������ɹ����ͷ�ȫ����Դ���رռ����壬����ͼƬmat��
            //     ������ʧ�ܣ��ͷ��Ѵ򿪵���Դ����������״̬������switch��ͳһ�رռ�����ͷ��ؿ�mat
            switch (uFormat)
            {

            case CF_BITMAP: { // 1. λͼ ===================================================================
                HBITMAP hbm = (HBITMAP)GetClipboardData(uFormat); // 1.1. �Ӽ�������¼��ָ�룬�õ��ļ����
                if (hbm) {
                    // GlobalLock(hbm); // ����ֵ������Ч�ģ���λͼ�ƺ�����Ҫ����
                  // https://social.msdn.microsoft.com/Forums/vstudio/en-US/d2a6aa71-68d7-4db0-8b1f-5d1920f9c4ce/globallock-and-dib-transform-into-hbitmap-issue?forum=vcgeneral
                    BITMAP bmp; // ���ָ�򻺳�����ָ�룬�����������й�ָ��ͼ�ζ������Ϣ
                    GetObject(hbm, sizeof(BITMAP), &bmp); // 1.2. ��ȡͼ�ζ������Ϣ������ͼƬ���ݱ���
                    if (!hbm) {
                        set_state(CODE_ERR_CLIP_GETOBJ, MSG_ERR_CLIP_GETOBJ); // ����״̬������ͼ�ζ�����Ϣʧ��
                        break;
                    }
                    int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8; // ����ɫ�����ͨ������32bitΪ4��24bitΪ3
                    // 1.3. �����hbm�е�λͼ���Ƶ�������
                    long sz = bmp.bmHeight * bmp.bmWidth * nChannels; // ͼƬ��С���ֽڣ�
                    cv::Mat mat(cv::Size(bmp.bmWidth, bmp.bmHeight), CV_MAKETYPE(CV_8U, nChannels));  // ����վ��󣬴���λͼ��С�����
                    long getsz = GetBitmapBits(hbm, sz, mat.data); // �����hbm��sz���ֽڸ��Ƶ�������img.data
                    if (!getsz) {
                        set_state(CODE_ERR_CLIP_BITMAP, MSG_ERR_CLIP_BITMAP); // ����״̬����ȡλͼ����ʧ��
                        break;
                    }
                    CloseClipboard();  // �ͷ���Դ
                    // 1.4. ���غ��ʵ�ͨ��
                    if (mat.data) {
                        if (nChannels == 1 || nChannels == 3) { // 1��3ͨ����PPOCR��ʶ��ֱ�ӷ���
                            return mat;
                        }
                        else if (nChannels == 4) { // 4ͨ����PPOCR����ʶ��ɾȥalphaת3ͨ���ٷ���
                            cv::Mat mat_c3;
                            cv::cvtColor(mat, mat_c3, cv::COLOR_BGRA2BGR); // ɫ�ʿռ�ת��
                            return mat_c3;
                        }
                        set_state(CODE_ERR_CLIP_CHANNEL, MSG_ERR_CLIP_CHANNEL(nChannels)); // ����״̬��ͨ�����쳣
                        break;
                    }
                    // ���������� !getsz �Ѿ� break �ˣ������ߵ������������ٱ���һ��
                    set_state(CODE_ERR_CLIP_BITMAP, MSG_ERR_CLIP_BITMAP); // ����״̬����ȡλͼ����ʧ��
                    break;
                }
                set_state(CODE_ERR_CLIP_DATA, MSG_ERR_CLIP_DATA); // ����״̬����ȡ����������ʧ��
                break;
            }

            case CF_HDROP: { // 2. �ļ��б��� =========================================================== 
                HDROP hClip = (HDROP)GetClipboardData(uFormat); // 2.1. �õ��ļ��б�ľ��
                if (hClip) {
                    // https://docs.microsoft.com/zh-CN/windows/win32/api/shellapi/nf-shellapi-dragqueryfilea
                    GlobalLock(hClip); // 2.2. ����ȫ���ڴ����
                    int iFiles = DragQueryFile(hClip, 0xFFFFFFFF, NULL, 0); // 2.3. 0xFFFFFFFF��ʾ�����ļ��б�ļ���
                    if (iFiles != 1) { // ֻ����1���ļ�
                        GlobalUnlock(hClip);
                        set_state(CODE_ERR_CLIP_FILES, MSG_ERR_CLIP_FILES(iFiles)); // ����״̬���ļ���������Ϊ1
                        break;
                    }
                    //for (int i = 0; i < iFiles; i++) {
                    int i = 0; // ֻȡ��1���ļ�
                    UINT lenChar = DragQueryFile(hClip, i, NULL, 0); // 2.4. �õ���i���ļ����������軺�����Ĵ�С���ֽڣ�
                    char* nameC = new char[lenChar + 1]; // ����ļ������ֽ�����
                    DragQueryFileA(hClip, i, nameC, lenChar + 1); // 2.5. �����i���ļ���
                    wchar_t* nameW = char_2_wchar(nameC); // 2.6. �ļ���תΪ���ֽ�����
                    cv::Mat mat = imread_wstr(nameW); // 2.7. ���Զ�ȡ�ļ�
                    // �ͷ���Դ
                    delete[] nameC;
                    delete[] nameW;
                    GlobalUnlock(hClip); // 2.x.1 �ͷ��ļ��б���
                    CloseClipboard(); // 2.x.2 �رռ�����
                    return mat;
                }
                set_state(CODE_ERR_CLIP_DATA, MSG_ERR_CLIP_DATA); // ����״̬����ȡ����������ʧ��
                break;
            }

            case NULL: // ������Ϊ��
                set_state(CODE_ERR_CLIP_EMPTY, MSG_ERR_CLIP_EMPTY); // ����״̬��������Ϊ��
                break;
            case -1: // ������֧�ֵĸ�ʽ
            default: // δ֪
                set_state(CODE_ERR_CLIP_FORMAT, MSG_ERR_CLIP_FORMAT); // ����״̬�� ������ĸ�ʽ��֧��
                break;
            }
            CloseClipboard(); // Ϊbreak������رռ����壬ʹ���������ܹ��������ʼ����塣
        }
        return cv::Mat();
    }

    // ===== ÿ�غϵ���ڣ���ȡͼƬ =============
    // ���� cv::imread ����·��pathU8����һ��ͼƬ��pathU8����Ϊutf-8��string
    cv::Mat imread_utf8(string pathU8, int flags) {
        set_state(); // ����״̬����ʼ��
        if (pathU8 == u8"clipboard") { // ��Ϊ����������
            return imread_clipboard(flags);
        }
        // string u8 ת wchar_t
        std::wstring wpath;
        try {
            wpath = conv_Ustr_Wstr.from_bytes(pathU8); // ����ת����ת��
        }
        catch (...) {
            set_state(CODE_ERR_PATH_CONV, MSG_ERR_PATH_CONV(pathU8)); // ����״̬��תwstringʧ��
            return cv::Mat();
        }
        return imread_wstr(wpath);
    }
}

