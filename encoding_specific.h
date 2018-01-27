
#if !defined(encoding_specific_h)
#define encoding_specific_h

//2007.3.17 Percend encoding
class _CPercentEncoding : public std::vector<char>{

private:

	bool			m_bNullTerminated;

protected:

	void _init(const std::string& sTxt)
	{
		std::string::const_iterator it;
		for(it=sTxt.begin(); it!=sTxt.end(); ++it){
			char ch=*it; if(!ch) break;
			if(ch<0 || ch=='%'){
				char vBuf[8]; ::memset(vBuf, 0, sizeof(vBuf));
				int x=(unsigned char)ch;
				::sprintf(vBuf, "%%%02x", x);
				std::copy(vBuf, vBuf+strlen(vBuf), std::back_inserter(*this));
			}else{
				push_back(ch);
			}
		}
		if(m_bNullTerminated) push_back('\0');
	}

public:

	_CPercentEncoding(const std::string& sTxt, bool bNullTerminated=true)
		: m_bNullTerminated(bNullTerminated)
	{
		_init(sTxt);
	}

	operator std::string()const
	{
		if(m_bNullTerminated){
			const std::vector<char>& vBuf=*this;
			return &vBuf[0];
		}else{
			std::string tmp(begin(), end());
			return tmp;
		}
	}
};

class _CPercentDecoding : public std::vector<char>{

private:

	bool			m_bNullTerminated;

protected:

	void _init(const std::string& sUrl)
	{
		std::string::const_iterator it;
		for(it=sUrl.begin(); it!=sUrl.end(); ++it){
			std::string::value_type ch=*it;
			if(ch=='%'){
				std::string tmp;
				it++;
				if(it!=sUrl.end()) tmp+=*it; else break;
				it++;
				if(it!=sUrl.end()) tmp+=*it; else break;
				int x; if(1!=::sscanf(tmp.c_str(), "%2x", &x)) x='?';
				ch=static_cast<char>(x);
				if(!ch) break;
				push_back(ch);
			}else if(ch=='+'){
				push_back(' ');
			}else{
				push_back(ch);
			}
		}
		if(m_bNullTerminated) push_back('\0');
	}

public:

	_CPercentDecoding(const std::string& sTxt, bool bNullTerminated=true)
		: m_bNullTerminated(bNullTerminated)
	{
		_init(sTxt);
	}

	operator std::string()const
	{
		if(m_bNullTerminated){
			const std::vector<char>& vBuf=*this;
			return &vBuf[0];
		}else{
			std::string tmp(begin(), end());
			return tmp;
		}
	}

};

//2011.1.17 ASCII encoding;
class _CAsciiEncoding : public std::vector<char>{

private:

	bool			m_bNullTerminated;

protected:

	void _init(const char* lpBuf, int nLen)
	{
		while(nLen-->0){
			int c=(unsigned char)*lpBuf++;
			char v[8]; ::memset(v, 0, sizeof(v));
			::sprintf(v, "%02x", c);
			push_back(v[0]);
			push_back(v[1]);
		}
		if(m_bNullTerminated) push_back('\0');
	}

public:

	_CAsciiEncoding(const char* lpBuf, int nLen, bool bNullTerminated=false)
		: m_bNullTerminated(bNullTerminated)
	{
		_init(lpBuf, nLen);
	}

	operator std::string()const
	{
		if(m_bNullTerminated){
			const std::vector<char>& vBuf=*this;
			return &vBuf[0];
		}else{
			std::string tmp(begin(), end());
			return tmp;
		}
	}
};

//2011.1.17 ASCII decoding;
class _CAsciiDecoding : public std::vector<char>{

protected:

	void _init(const char* lpBuf, int nLen)
	{
		const char* p=lpBuf;
		for(int i=0; i<nLen/2; ++i){
			char v[]={*p, *(p+1), '\0'}; int c=0;
			if(1==::sscanf(v, "%02x", &c)){
				push_back(c);
			}
			p+=2;
		}
	}

public:

	_CAsciiDecoding(const char* lpBuf, int nLen)
	{
		_init(lpBuf, nLen);
	}

};

//UTF-8: http://www.ietf.org/rfc/rfc2044.txt

//U-00000000 - U-0000007F: (1) 0xxxxxxx
//U-00000080 - U-000007FF: (2) 110xxxxx 10xxxxxx
//U-00000800 - U-0000FFFF: (3) 1110xxxx 10xxxxxx 10xxxxxx
//U-00010000 - U-001FFFFF: (4) 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
//U-00200000 - U-03FFFFFF: (5) 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
//U-04000000 - U-7FFFFFFF: (6) 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

//+-------------------------------------------------------------------------------------------+
//|   UNICODE                                    |   UTF-8                                    |
//|-------------------------------------------------------------------------------------------|
//|   code                 |   range             |   1st Byte   |   2nd byte   |   3rd Byte   |
//|----------------------------------------------------------------------------|--------------|
//|   00000000 0xxxxxxx    |   0000   -   007F   |   0xxxxxxx   |              |              |
//|----------------------------------------------------------------------------|--------------|
//|   00000yyy yyxxxxxx    |   0080   -   07FF   |   110yyyyy   |   10xxxxxx   |              |
//|----------------------------------------------------------------------------|--------------|
//|   zzzzyyyy yyxxxxxx    |   0800   -   FFFF   |   1110zzzz   |   10yyyyyy   |   10xxxxxx   |
//+-------------------------------------------------------------------------------------------+

//2007.3.15 wchar_t string (Unicode) ==> UTF-8 string
class _CUtf8Encoding : public std::vector<char>{

private:

	bool			m_bNullTerminated;

protected:

	//2012.10.9 change "wchar_t" to "unsigned int"
	//static int _wchar_to_utf8(wchar_t ch, char* pBuf)
	static int _wchar_to_utf8(unsigned int ch, char* pBuf)
	{
		int nBytes=-1;
		if(ch<=0x7f){
			nBytes=1;
		}else if(ch<=0x7ff){
			nBytes=2;
		}else if(ch<=0xffff){
			nBytes=3;
		}else if(ch<=0x1fffffL){
			nBytes=4;
		}else if(ch<=0x3ffffffL){
			nBytes=5;
		}else if(ch<=0x7fffffffL){
			nBytes=6;
		}

		if(nBytes==1){
			pBuf[0]=(char)ch;
		}else{
			for(int i=0;i<nBytes;++i){
				char x;
				if(i==nBytes-1){
					x = ch & (0xff>>(i+2));
					x = x | (0xff<<(7-i));
				}else{
					x = ch & (0xff>>2);
					x = x | (1<<7);
					ch = ch >> 6;
				}
				pBuf[nBytes-i-1]=x;
			}
		}
		return nBytes;
	}

	void _init(const wchar_t* lpwszStr)
	{
		wchar_t wch;
		while((wch=*lpwszStr++)){
			char pBuf[7]; memset(pBuf, 0, sizeof(pBuf));
			int n=_wchar_to_utf8(wch, pBuf);
			if(n>0){
				std::copy(pBuf, pBuf+n, std::back_inserter(*this));
			}else{
				//ignore invalid characters;
			}
		}
		if(m_bNullTerminated) push_back('\0');
	}

public:

	_CUtf8Encoding(const wchar_t* lpwszStr, bool bNullTerminated=true)
		: m_bNullTerminated(bNullTerminated)
	{
		if(lpwszStr) _init(lpwszStr);
	}

	operator std::string()const
	{
		if(m_bNullTerminated){
			const std::vector<char>& vBuf=*this;
			return &vBuf[0];
		}else{
			std::string tmp(begin(), end());
			return tmp;
		}
	}

	std::string _percent_text(void)const
	{
#if 0

		std::string txt;
		const_iterator it;
		for(it=begin(); it!=end(); ++it){
			int ch=(unsigned char)*it;
			if(ch=='\0') break;
			if(ch>0x7f || ch=='%'){
				char pBuf[32]; memset(pBuf, 0, sizeof(pBuf));
				::sprintf(pBuf, "%%%02x", ch);
				txt+=pBuf;
			}else{
				txt+=ch;
			}
		}
		return txt;

#else

		return _CPercentEncoding(*this, m_bNullTerminated);

#endif
	}

};

//2007.3.15 UTF-8 string ==> wchar_t string (Unicode)
class _CUtf8Decoding : public std::vector<wchar_t>{

private:

	bool			m_bNullTerminated;

	bool			m_bSucc;

protected:

	static int _utf8_to_wchar(const char* pUtf8, int nLen, wchar_t& wchRes)
	{
		int nBytes=-1;
		unsigned char wchFirst=(unsigned char)*pUtf8;
		if(wchFirst>(unsigned char)(0xff<<2)){
			nBytes=6;
		}else if(wchFirst>(unsigned char)(0xff<<3)){
			nBytes=5;
		}else if(wchFirst>(unsigned char)(0xff<<4)){
			nBytes=4;
		}else if(wchFirst>(unsigned char)(0xff<<5)){
			nBytes=3;
		}else if(wchFirst>(unsigned char)(0xff<<6)){
			nBytes=2;
		}else if(wchFirst<=0x7f){
			nBytes=1;
		}

		//2008.12.16 pass 'nLen' into here, to avoid recalculating its length;
		//After improving this line, it runs much faster than ever!!!!

		//if( nBytes>strlen(pUtf8) || nBytes<=0 ) return -1;
		if( nBytes>nLen || nBytes<=0 ) return -1;

		if(nBytes==1){
			wchRes=wchFirst;
		}else if(nBytes>1){
			wchar_t wch=0;
			for(int i=0; i<nBytes; ++i){
				unsigned int tmp;
				unsigned char x=pUtf8[nBytes-i-1];
				if(i==nBytes-1){
					//2011.9.15 BUG in the line: 'tmp=(x & (0xff>>(6-i)));'
					//For Chinese chars (3-byte utf8char), it happens to right-move '4' bits;
					//But, for Russian (2-bytes), it should left-move 3 bits, not '5';
					//tmp=(x & (0xff>>(6-i))); //??????????????
					tmp=(x & (0xff>>(nBytes+1)));
				}else{
					tmp=(x & (0xff>>2));
				}
				tmp <<= (i*6);
				wch |= tmp;
			}
			wchRes=wch;
		}

		return nBytes;
	}

	void _init(const char* pUtf8)
	{
		int nLen=pUtf8 ? strlen(pUtf8) : 0;
		while(*pUtf8){
			wchar_t wch=0;
			int n=_utf8_to_wchar(pUtf8, nLen, wch);
			if(n>0){
				pUtf8+=n; //seek to next char;
				nLen-=n; //bytes left in string;
				push_back(wch);
			}else{
				m_bSucc=false;
				break; //invalid UTF-8 string;
			}
		}
		if(m_bNullTerminated) push_back(L'\0');
	}

public:

	_CUtf8Decoding(const std::string& sUtf8, bool bNullTerminated=true)
		: m_bNullTerminated(bNullTerminated), m_bSucc(true)
	{
		reserve(sUtf8.size()+1); //2007.10.23 for performance
		_init(sUtf8.c_str());
	}

	operator bool()const{return m_bSucc;}

	operator const wchar_t*()const
	{
		//if(!m_bNullTerminated) throw "Unavailable!";
		if(m_bNullTerminated){
			const std::vector<wchar_t>& vBuf=*this;
			return &vBuf[0];
		}else{
			return NULL;
		}
	}

};

//2007.7.29 About Unicode Encoding/Decoding on Linux/FreeBSD
//To encode Chinese chars on Linux/FreeBSD, we must set locale at first, as below,
//setlocale(LC_ALL, "") or setlocale(LC_CTYPE, "");
//It indicates to use the current system locale for all conversion,
//therefore, we must properly set the system locale, otherwise conversion may fail.
//To set locale on Linux, edit /etc/sysconfig/i18n.
//For FreeBSD see handbook at:
//http://www.freebsd.org/doc/en_US.ISO8859-1/books/handbook/using-localization.html
//setenv LC_ALL zh_CN.GB18030 seems not to work with FreeBSD61!
//edit /etc/profile, add these lines:
//export LANG=zh_CN.GBK
//export LC_ALL=zh_CN.GBK
//export LC_CTYPE=zh_CN.GBK

//2008.12.19 Regarding Unicode on Unix, it is a big problem;
//First, we expected to use 'mbstowcs' system call, however, the 'wcs' is not the 'UCS',
//it actually depends on the specific system implementations. On FreeBSD/Solaris, it is NOT,
//nevertherless, GNU/Linux happends to comply with the 'UCS' specs.
//Recently, we consulted ADOAL and got the idea of using 'libiconv' for the Unicode on Unix,
//However, libiconv has weird behaviour in some cases, for example, the version shipped within Linux
//has a different 'const' declaration from the downloaded version. Specifying a wrong Encoding
//for the input bytes, the program simply aborted! So it would crash programs on any incorrect user input.
//By now, we have no solutions for Unicode on FreeBSD/Solaris, all non-ansi characters are ignored.

#if defined(WIN32)

struct _CLocaleCharset{
	_CLocaleCharset(void){return;}
	operator std::string(void)const{return "";}
};

#elif defined(unix) || defined(__APPLE__)

#include <langinfo.h>

struct _CLocaleCharset{
	_CLocaleCharset(void){return;}
	operator std::string(void)const
	{
		return nl_langinfo(CODESET); //for "C": "US-ASCII";
	}
};

/*
#if defined(__USING_LIBICONV)

    #include <iconv.h>

    class _CLibIconv{

    private:

	std::string	m_sFromCoding;
	std::string	m_sToCoding;

	iconv_t		m_xIconv;

    public:

	virtual ~_CLibIconv(void){if(_is_open()) iconv_close(m_xIconv);}

	_CLibIconv(const std::string& f, const std::string& t)
	    : m_sFromCoding(f), m_sToCoding(t), m_xIconv((iconv_t)-1)
	{
	    m_xIconv=iconv_open(m_sToCoding.c_str(), m_sFromCoding.c_str());
	}

	bool _is_open(void)const{return m_xIconv!=(iconv_t)(-1);}

	int operator()(const char* pSrc, size_t nSrcLeft, std::vector<char>& vDstBuf)const
	{
	    int nRes=-1;
	    try{
		nRes=_convert(pSrc, nSrcLeft, vDstBuf);
	    }catch(...){
	    }
	    return nRes;
	}

	int _convert(const char* pSrc, size_t nSrcLeft, std::vector<char>& vDstBuf)const
	{
	    vDstBuf.clear();
	    int nRes=-1, nMaxLen=1024*1024*32L; //32MB
	    if(_is_open() && pSrc && nSrcLeft>0 && nSrcLeft<=nMaxLen){

		int nIncr=4;
		do{
		    size_t nDstLeft=nSrcLeft*nIncr; vDstBuf.resize(nDstLeft);

		    #if defined(__linux__)
		    char* pSrcBuf=(char*)pSrc;
		    #else
		    const char* pSrcBuf=pSrc;
		    #endif

		    char* pDstBuf0=&vDstBuf[0];
		    char* pDstBuf=&vDstBuf[0];

		    size_t x=iconv(m_xIconv, &pSrcBuf, &nSrcLeft, &pDstBuf, &nDstLeft);
		    if(x!=(size_t)-1){
			nRes=(pDstBuf-pDstBuf0);
			if(nRes>0) vDstBuf.resize(nRes); else vDstBuf.clear();
			break;
		    }else{
			if(errno==E2BIG){
			    nIncr++;
			    if(nIncr>6){
				break; //allow 6 times of original text length;
			    }
			}else{
			    break;
			}
		    }
		}while(nIncr>0);
	    }
	    return nRes;
	}
    };

#endif
*/

#endif

//2007.3.15 Multi-byte string (ANSI CodePage) ==> Unicode wchar_t string
class _CUnicodeEncoding : public std::vector<wchar_t>{

private:

	std::string		m_sEncoding;

	bool			m_bNullTerminated;

protected:

	void _init(const char* lpszStr)
	{
		int nLen=::strlen(lpszStr);

#if defined(WIN32)

		unsigned int cp=::GetACP(); //CP_ACP, CP_THREAD_ACP; win98 doesn't support CP_THREAD_ACP?
		int n=::MultiByteToWideChar(cp, 0, lpszStr, nLen, NULL, 0);
		if(n>0){
			std::vector<wchar_t>& vBuf=*this;
			vBuf.resize(n, L'\0');
			::MultiByteToWideChar(cp, 0, lpszStr, nLen, &vBuf[0], n);
		}

#elif defined(unix) || defined(__APPLE__)

#if defined(__USING_LIBICONV)

		/*
	//2008.12.18 Requires libiconv for FreeBSD/Solaris;
	//UCS-4, UCS-4-INTERNAL, UCS-4BE, UCS-4LE, UCS-2, UCS-2-INTERNAL, UCS-2BE, UCS-2LE
	int nWidth=sizeof(wchar_t); std::string sUCS;
	if(nWidth==2) sUCS="UCS-2-INTERNAL"; else if(nWidth==4) sUCS="UCS-4-INTERNAL"; else throw "Not Implemented";
	_CLibIconv xConv(m_sEncoding, sUCS); std::vector<char> vBuf;
	size_t nBytes=xConv(lpszStr, nLen, vBuf);
	size_t nChars=nBytes/nWidth;
	if(nBytes>0 && nChars>0){
	    const wchar_t* lpw=(const wchar_t*)(&vBuf[0]);
	    this->assign(lpw, lpw+nChars);
	}*/

#else

#if defined(linux) || defined(__APPLE__)

		//2008.12.18 On GNU/Linux, mbstowcs() happens to comply with UCS-4;
		//2008.12.22 Just realized that 'size_t' is 'unsigned'.
		size_t n=::mbstowcs(NULL, lpszStr, 0);
		if(n!=(size_t)(-1)){
			//2007.7.29 also see: manual page of wcstombs on Linux;
			//In order to avoid the case 2 above, the programmer should
			//make sure n is greater or equal to wcstombs(NULL,src,0)+1.
			//2008.12.18 in order for consistency with WIN32 platform, Do Not actively add the ending ZERO;
			//The ending ZERO is controlled by 'm_bNullTerminated'. (see: file_utilities.h)
			//n++;
			std::vector<wchar_t>& vBuf=*this;
			vBuf.resize(n, L'\0');
			::mbstowcs(&vBuf[0], lpszStr, n);
		}

#else

		//_PLATFORM_PLACEHOLDER;

		//2008.12.19 for FreeBSD/Solaris:
		//No success with LIBICONV by now, but have to filter out any Non-ANSI characters;
		unsigned char ch;
		while( ch=*lpszStr++ ){
			if(ch>0 && ch<0x80) push_back((wchar_t)ch);
			else push_back(L'.');
		}

#endif

#endif

#else

		_PLATFORM_PLACEHOLDER;

#endif

		Q_UNUSED(nLen);

		if(m_bNullTerminated) push_back(L'\0');
	}

public:

	_CUnicodeEncoding(const std::string& sText, bool bNullTerminated=true)
		: m_sEncoding(_CLocaleCharset())
		, m_bNullTerminated(bNullTerminated)
	{
		_init(sText.c_str());
	}

	operator const wchar_t*()const
	{
		//if(!m_bNullTerminated) throw "Unavailable!";
		if(m_bNullTerminated){
			const std::vector<wchar_t>& vBuf=*this;
			return &vBuf[0];
		}else{
			return NULL;
		}
	}

};

//2007.3.15 Unicode wchar_t string ==> Multi-byte string (ANSI CodePage)
class _CUnicodeDecoding : public std::vector<char>{

private:

	std::string		m_sEncoding;

	bool			m_bNullTerminated;

protected:

	void _init(const wchar_t* lpwszStr)
	{
		int nLen=::wcslen(lpwszStr);

#if defined(WIN32)

		unsigned cp=::GetACP(); //CP_ACP;
		int n=::WideCharToMultiByte(cp, 0, lpwszStr, nLen, NULL, 0, NULL, NULL);
		if(n>0){
			std::vector<char>& vBuf=*this;
			vBuf.resize(n, '\0');
			::WideCharToMultiByte(cp, 0, lpwszStr, nLen, &vBuf[0], n, NULL, NULL);
		}

#elif defined(unix) || defined(__APPLE__)

#if defined(__USING_LIBICONV)

		//2008.12.18 Requires libiconv for FreeBSD/Solaris;
		//UCS-4, UCS-4-INTERNAL, UCS-4BE, UCS-4LE, UCS-2, UCS-2-INTERNAL, UCS-2BE, UCS-2LE
		int nWidth=sizeof(wchar_t); std::string sUCS;
		if(nWidth==2) sUCS="UCS-2-INTERNAL"; else if(nWidth==4) sUCS="UCS-4-INTERNAL"; else throw "Not Implemented";
		_CLibIconv xConv(sUCS, m_sEncoding); std::vector<char> vBuf;
		size_t nBytes=xConv((const char*)lpwszStr, nWidth*nLen, vBuf);
		if(nBytes>0){
			const char* lps=&vBuf[0];
			this->assign(lps, lps+nBytes);
		}

#else

#if defined(linux) || defined(__APPLE__)

		//2008.12.22 Just realized that 'size_t' is 'unsigned'.
		size_t n=::wcstombs(NULL, lpwszStr, 0);
		if(n!=(size_t)-1){
			//2007.7.29 also see: manual page of wcstombs on Linux;
			//In order to avoid the case 2 above, the programmer should
			//make sure n is greater or equal to wcstombs(NULL,src,0)+1.
			//2008.12.18 in order for consistency with WIN32 platform, Do Not actively add the ending ZERO;
			//The ending ZERO is controlled by 'm_bNullTerminated'. (see: file_utilities.h)
			//n++;
			std::vector<char>& vBuf=*this;
			vBuf.resize(n, '\0');
			::wcstombs(&vBuf[0], lpwszStr, n);
		}

#else

		//_PLATFORM_PLACEHOLDER;

		//2008.12.19 for FreeBSD/Solaris:
		//No success with LIBICONV by now, but have to filter out any Non-ANSI characters;
		wchar_t wch;
		while( wch=*lpwszStr++ ){
			if(wch>0 && wch<0x80) push_back((char)wch);
			else push_back('.');
		}

#endif

#endif

#else

		_PLATFORM_PLACEHOLDER;

#endif

		Q_UNUSED(nLen);

		if(m_bNullTerminated) push_back('\0');

	}

public:

	_CUnicodeDecoding(const wchar_t* lpwszStr, bool bNullTerminated=true)
		: m_sEncoding(_CLocaleCharset())
		, m_bNullTerminated(bNullTerminated)
	{
		if(lpwszStr) _init(lpwszStr);
	}

	operator std::string()const
	{
		if(m_bNullTerminated){
			const std::vector<char>& vBuf=*this;
			return &vBuf[0];
		}else{
			std::string tmp(begin(), end());
			return tmp;
		}
	}

};

class _CUtf8ToAnsi{

private:

	std::string	m_sTxt;

public:

	operator std::string()const{return m_sTxt;}
	const char* c_str(void)const{return m_sTxt.c_str();}
	_CUtf8ToAnsi(const std::string& sTxt)
	{
		m_sTxt=_CUnicodeDecoding(_CUtf8Decoding(sTxt));
	}
};

class _CAnsiToUtf8{

private:

	std::string	m_sTxt;

public:

	operator std::string()const{return m_sTxt;}
	const char* c_str(void)const{return m_sTxt.c_str();}
	_CAnsiToUtf8(const std::string& sTxt)
	{
		m_sTxt=_CUtf8Encoding(_CUnicodeEncoding(sTxt));
	}
};


//Uniform Resource Locators (URL)
//http://www.ietf.org/rfc/rfc1738.txt

//Uniform Resource Identifier (URI): Generic Syntax
//http://www.ietf.org/rfc/rfc3986.txt

//2007.3.15 percent-encoding unsafe characters in url
class _CUrlEncoding : public std::vector<char>{

private:

	bool			m_bNullTerminated;

protected:

	void _init(const std::string& sUrl)
	{
		//2007.3.15 encoding unsafe characters like this: %##
		//SAMPLE http://del.icio.us/flimsy?url=http://www.qingbo.org/?p=230#comments&title=ABC
		//only work with segments of URL, rather than whole URLs

		//2007.3.16 some filenames may contain "_.-:", which should not be encoded;
		//2010.10.26 considering links to local files, ie "d:\\abc\\def.txt";

		//std::string sRes, sUnsafe="`<>\"#%{}|\\^~[]+ ?=&.,;:*$#@!-";
		std::string sRes, sUnsafe="`<>\"#%{}|\\^~[]+ ?=&,;*$#@!";

		std::string::const_iterator it;
		for(it=sUrl.begin(); it!=sUrl.end(); ++it){
			char ch=*it;
			std::string::size_type p=sUnsafe.find(ch);
			if(p!=std::string::npos || ch<0){
				char vBuf[8]; ::memset(vBuf, 0, sizeof(vBuf));
				int x=(unsigned char)ch;
				::sprintf(vBuf, "%%%02x", x);
				std::copy(vBuf, vBuf+strlen(vBuf), std::back_inserter(*this));
			}else{
				push_back(ch);
			}
		}
		if(m_bNullTerminated) push_back('\0');
	}

public:

	_CUrlEncoding(const std::string& sUrl, bool bNullTerminated=true)
		: m_bNullTerminated(bNullTerminated)
	{
		_init(sUrl);
	}

	operator std::string()const
	{
		if(m_bNullTerminated){
			const std::vector<char>& vBuf=*this;
			return &vBuf[0];
		}else{
			std::string tmp(begin(), end());
			return tmp;
		}
	}

};

//2007.3.13 perform url-decoding while parsing query-string within URL
//2007.3.15 percent-decoding unsafe characters in url
class _CUrlDecoding : public std::vector<char>{

private:

	bool			m_bNullTerminated;

protected:

	void _init(const std::string& sUrl)
	{
		//16:28 2005-3-12 decoding URL like this: %##
		std::string::const_iterator it;
		for(it=sUrl.begin(); it!=sUrl.end(); ){
			std::string::value_type ch=*it++;
			if(ch=='%'){
				std::string tmp;
				if(it!=sUrl.end()){
					ch=*it;
					if(ch=='u'){
						//2007.3.23 Chinese characters on URL encoded with (javascript::escape(url)) looks like this,
						//e.g. /dbnew?title=%u6C49%u5B57%u6807%u9898
						it++;

						//2008.6.22 Workaround for solaris CC/STL;
						//#if defined(__sun__)
						//std::string::difference_type nRight=0; std::distance(it, sUrl.end(), &nRight);
						//#else
						//std::string::difference_type nRight=std::distance(it, sUrl.end());
						//#endif

						if(std::distance(it, sUrl.end())>=4){
							//if(nRight>=4){
							tmp.assign(it, it+4);
							it+=4;
							int x; if(1!=::sscanf(tmp.c_str(), "%4X", &x)) x='?';
							wchar_t lpwStr[2]={static_cast<wchar_t>(x), L'\0'};
							tmp=_CUtf8Encoding(lpwStr);
							std::copy(tmp.begin(), tmp.end(), std::back_inserter(*this));
						}else{
							break;
						}
					}else{

						//2008.6.22 Workaround for solaris CC/STL;
						//#if defined(__sun__)
						//std::string::difference_type nRight=0; std::distance(it, sUrl.end(), &nRight);
						//#else
						//std::string::difference_type nRight=std::distance(it, sUrl.end());
						//#endif

						if(std::distance(it, sUrl.end())>=2){
							//if(nRight>=2){
							tmp.assign(it, it+2);
							it+=2;
							int x; if(1!=::sscanf(tmp.c_str(), "%2x", &x)) x='?';
							ch=static_cast<char>(x);
							push_back(ch);
						}else{
							break;
						}
					}
				}else{
					break;
				}
			}else if(ch=='+'){
				push_back(' ');
			}else{
				push_back(ch);
			}
		}
		if(m_bNullTerminated) push_back('\0');
	}

public:

	_CUrlDecoding(const std::string& sUrl, bool bNullTerminated=true)
		: m_bNullTerminated(bNullTerminated)
	{
		_init(sUrl);
	}

	operator std::string()const
	{
		if(m_bNullTerminated){
			const std::vector<char>& vBuf=*this;
			return &vBuf[0];
		}else{
			std::string tmp(begin(), end());
			return tmp;
		}
	}

};

//2007.6.10 xml encoding
class _CXmlEncoding{

private:

	std::string		m_sTxt;

	void _init(const std::string& txt, bool bRemoveCtrls)
	{
		// http://www.ugcs.caltech.edu/~toby/characters/
		// http://www.w3schools.com/tags/ref_entities.asp
		// http://en.wikipedia.org/wiki/List_of_XML_and_HTML_character_entity_references
		m_sTxt.reserve(txt.size()*2);
		const char* lpszTrack=txt.c_str(); unsigned char ch;
		while((ch=*lpszTrack++)){
			if(ch<' '){
				if(bRemoveCtrls){
					continue;
				}else{
					m_sTxt+=ch;
				}
			}
			else if(ch=='&') m_sTxt+="&amp;";
			else if(ch=='<') m_sTxt+="&lt;";
			else if(ch=='>') m_sTxt+="&gt;";
			else if(ch=='\"') m_sTxt+="&quot;";
			else if(ch=='\'') m_sTxt+="&apos;";
			else m_sTxt+=ch;
		}
	}

public:

	_CXmlEncoding(const std::string& txt, bool bRemoveCtrls){_init(txt, bRemoveCtrls);}

	operator std::string()const{return m_sTxt;}

};

class _CXmlDecoding{

private:

	std::string		m_sTxt;

protected:

	bool _match(const char* src, const char* tag, int n)
	{
		bool bMatch=false;
		if(n>0 && *src && *tag){
			while(*src && *tag && n>0){
				if(*src!=*tag) break;
				n--; src++; tag++;
			}
			bMatch=(n==0);
		}
		return bMatch;
	}

	void _init(const std::string& sSrc, const char* vTags[], int nTags)
	{
		m_sTxt.reserve(sSrc.size());
		std::string::size_type p1=0, p2=0, nLen=sSrc.size();
		while(p1<nLen){

			p2=sSrc.find('&', p1);
			if(p2!=std::string::npos){

				m_sTxt+=sSrc.substr(p1, p2-p1);

				p2++; //skip '&';
				p1=p2; //for next loop;

				const char* lpszSrc=sSrc.c_str()+p2;
				for(int i=0; i<nTags/2; ++i){
					const char* lpszTag=vTags[i*2]; lpszTag++; //skip '&';
					const char* lpszRes=vTags[i*2+1];
					int len=strlen(lpszTag);
					if(_match(lpszSrc, lpszTag, len)){
						//if(_CStdStringUtil(lpszSrc)._compare(lpszTag, len, false)==0){
						m_sTxt+=lpszRes;
						p1+=len; //ready for next loop;
						break;
					}
				}

			}else{
				m_sTxt+=sSrc.substr(p1);
				break;
			}
		}
	}

public:

	_CXmlDecoding(const std::string& txt)
	{
		//2008.7.13 here we only consider a sub set of the entities;
		//For a full list of entities, see:
		// http://www.w3pop.com/learn/view/doc/ref_entities/
		// http://en.wikipedia.org/wiki/List_of_XML_and_HTML_character_entity_references
		const char* vEntitiesXml[]={
			"&lt;", "<"
			, "&gt;", ">"
			, "&quot;", "\""
			, "&apos;", "\'"
			//, "&nbsp;", " " //2008.7.23 only for bad XML source;
			, "&amp;", "&" //must be put at the end of the array;
		};

		_init(txt, vEntitiesXml, sizeof(vEntitiesXml)/sizeof(vEntitiesXml[0]));
	}

	operator std::string()const{return m_sTxt;}

};

//2008.7.12 html encoding
class _CHtmlEncoding{

private:

	std::string		m_sTxt;

	void _init(const std::string& txt, bool bRemoveCtrls)
	{
		// 2008.7.12 Encode into HTML text;
		// the '&nbsp' is an entitiy for 'non-breaking space' character.
		// and it has a value of '&#160;' or '0xa0' in UCS2
		// During InnoKB, set html element text with the '&nbsp;', then get '0xa0' instead of '0x20';
		// therefore, this function converts '  ' to ' &nbsp;' for a wordaround;
		// TODO: consider converting '\r\n' to '<br>', and more entities;
		// http://en.wikipedia.org/wiki/List_of_XML_and_HTML_character_entity_references
		m_sTxt.reserve(txt.size()*3);
		const char* lpszTrack=txt.c_str(); unsigned char ch; int nSpace=0;
		while((ch=*lpszTrack++)){
			if(ch<' '){
				if(bRemoveCtrls){
					continue;
				}else{
					m_sTxt+=ch;
				}
			}else if(ch==' '){
				if(nSpace>0){
					m_sTxt+="&nbsp;";
					nSpace=0;
				}else{
					m_sTxt+=" ";
					nSpace++;
				}
				continue;
			}
			else if(ch=='&') m_sTxt+="&amp;";
			else if(ch=='<') m_sTxt+="&lt;";
			else if(ch=='>') m_sTxt+="&gt;";
			else if(ch=='\"') m_sTxt+="&quot;";
			else if(ch=='\'') m_sTxt+="&apos;";
			else m_sTxt+=ch;

			nSpace=0;
		}
	}

public:

	_CHtmlEncoding(const std::string& txt, bool bRemoveCtrls){_init(txt, bRemoveCtrls);}

	operator std::string()const{return m_sTxt;}

};

class _CHtmlDecoding : public _CXmlDecoding{

public:

	_CHtmlDecoding(const std::string& txt) : _CXmlDecoding("")
	{
		//2008.7.13 here we only consider a sub set of the entities;
		//For a full list of entities, see: http://www.w3pop.com/learn/view/doc/ref_entities/
		const char* vEntitiesHtml[]={
			"&lt;", "<"
			, "&gt;", ">"
			, "&quot;", "\""
			, "&apos;", "\'"
			, "&nbsp;", " "
			, "&circ;", "^"
			, "&tilde;", "~"
			, "&copy;", "(c)"
			//, ...
			, "&amp;", "&" //must be put at the end of the array;
		};

		_init(txt, vEntitiesHtml, sizeof(vEntitiesHtml)/sizeof(vEntitiesHtml[0]));
	}

};

#define		__ToAnsi(s)		_CUtf8ToAnsi(s)
#define		__ToUtf8(s)		_CAnsiToUtf8(s)

#endif
