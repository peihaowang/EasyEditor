#ifndef DLL_UTILITIES_H
#define DLL_UTILITIES_H

#if defined(Q_OS_MAC)

#include <dlfcn.h>

class QLibraryEx{

public:

	typedef		void *		FARPROC;

	class _CFunc{

	private:

		FARPROC   m_xProc;

	public:

		_CFunc(FARPROC xProc) : m_xProc(xProc){return;}

		operator bool()const{ return m_xProc!=NULL; }

		template <typename _TRes>
		_TRes operator()(_TRes def)
		{
			if(!bool(*this))return def;
			typedef _TRes (__STDCALL2* _TFarProc)(void);
			return ((_TFarProc)m_xProc)();
		}

		template <typename P1, typename _TRes>
		_TRes operator()(P1 p1, _TRes def)
		{
			if(!bool(*this))return def;
			typedef _TRes (__STDCALL2* _TFarProc)(P1 p1);
			return ((_TFarProc)m_xProc)(p1);
		}

		template <typename P1, typename P2, typename _TRes>
		_TRes operator()(P1 p1, P2 p2, _TRes def)
		{
			if(!bool(*this))return def;
			typedef _TRes (__STDCALL2* _TFarProc)(P1 p1, P2 p2);
			return ((_TFarProc)m_xProc)(p1, p2);
		}

		template <typename P1, typename P2, typename P3, typename _TRes>
		_TRes operator()(P1 p1, P2 p2, P3 p3, _TRes def)
		{
			if(!bool(*this))return def;
			typedef _TRes (__STDCALL2* _TFarProc)(P1 p1, P2 p2, P3 p3);
			return ((_TFarProc)m_xProc)(p1, p2, p3);
		}

		template <typename P1, typename P2, typename P3, typename P4, typename _TRes>
		_TRes operator()(P1 p1, P2 p2, P3 p3, P4 p4, _TRes def)
		{
			if(!bool(*this))return def;
			typedef _TRes (__STDCALL2* _TFarProc)(P1 p1, P2 p2, P3 p3, P4 p4);
			return ((_TFarProc)m_xProc)(p1, p2, p3, p4);
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename _TRes>
		_TRes operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, _TRes def)
		{
			if(!bool(*this))return def;
			typedef _TRes (__STDCALL2* _TFarProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5);
			return ((_TFarProc)m_xProc)(p1, p2, p3, p4, p5);
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename _TRes>
		_TRes operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, _TRes def)
		{
			if(!bool(*this))return def;
			typedef _TRes (__STDCALL2* _TFarProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6);
			return ((_TFarProc)m_xProc)(p1, p2, p3, p4, p5, p6);
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename _TRes>
		_TRes operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, _TRes def)
		{
			if(!bool(*this))return def;
			typedef _TRes (__STDCALL2* _TFarProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7);
			return ((_TFarProc)m_xProc)(p1, p2, p3, p4, p5, p6, p7);
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename _TRes>
		_TRes operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, _TRes def)
		{
			if(!bool(*this))return def;
			typedef _TRes (__STDCALL2* _TFarProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8);
			return ((_TFarProc)m_xProc)(p1, p2, p3, p4, p5, p6, p7, p8);
		}

	};

private:

	void *		m_hInst;
	bool		m_bFree;
	QString		m_sDyLib;

protected:

	void close(void)
	{
		if(m_hInst!=NULL){
			dlclose(m_hInst);
			m_hInst=NULL;
		}
	}

public:

	operator bool()const{return isLoaded();}
	_CFunc operator[](const char* lpszFuncName)
	{
		FARPROC p=resolve(lpszFuncName);
		return _CFunc(p);
	}

	virtual ~QLibraryEx(void){if(m_bFree) close();}
	QLibraryEx(const QString& sLib="") : m_hInst(NULL), m_bFree(true), m_sDyLib(sLib)
	{
		setFileName(sLib);
		load();
	}

	QString fileName()const{return m_sDyLib;}
	void setFileName(const QString& sFn){m_sDyLib=sFn+".dylib";}

	bool isLoaded(void)const{return m_hInst!=NULL;}
	bool load(void)
	{
		close();
		m_hInst=dlopen(m_sDyLib.toStdString().c_str(), RTLD_LAZY);
		return isLoaded();
	}

	FARPROC resolve(const char* lpszFuncName)
	{
		return (lpszFuncName && *lpszFuncName && isLoaded()) ? dlsym(m_hInst, lpszFuncName) : NULL;
	}

};

#else

class QLibraryEx : public QLibrary{

public:

	typedef		void *		FARPROC;

	class _CFunc{

	private:

		FARPROC   m_xProc;

	public:

		_CFunc(FARPROC xProc) : m_xProc(xProc){return;}

		operator bool()const{ return m_xProc!=NULL; }

		template <typename _TRes>
		_TRes operator()(_TRes def)
		{
			if(!bool(*this))return def;
			typedef _TRes (__STDCALL2* _TFarProc)(void);
			return ((_TFarProc)m_xProc)();
		}

		template <typename P1, typename _TRes>
		_TRes operator()(P1 p1, _TRes def)
		{
			if(!bool(*this))return def;
			typedef _TRes (__STDCALL2* _TFarProc)(P1 p1);
			return ((_TFarProc)m_xProc)(p1);
		}

		template <typename P1, typename P2, typename _TRes>
		_TRes operator()(P1 p1, P2 p2, _TRes def)
		{
			if(!bool(*this))return def;
			typedef _TRes (__STDCALL2* _TFarProc)(P1 p1, P2 p2);
			return ((_TFarProc)m_xProc)(p1, p2);
		}

		template <typename P1, typename P2, typename P3, typename _TRes>
		_TRes operator()(P1 p1, P2 p2, P3 p3, _TRes def)
		{
			if(!bool(*this))return def;
			typedef _TRes (__STDCALL2* _TFarProc)(P1 p1, P2 p2, P3 p3);
			return ((_TFarProc)m_xProc)(p1, p2, p3);
		}

		template <typename P1, typename P2, typename P3, typename P4, typename _TRes>
		_TRes operator()(P1 p1, P2 p2, P3 p3, P4 p4, _TRes def)
		{
			if(!bool(*this))return def;
			typedef _TRes (__STDCALL2* _TFarProc)(P1 p1, P2 p2, P3 p3, P4 p4);
			return ((_TFarProc)m_xProc)(p1, p2, p3, p4);
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename _TRes>
		_TRes operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, _TRes def)
		{
			if(!bool(*this))return def;
			typedef _TRes (__STDCALL2* _TFarProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5);
			return ((_TFarProc)m_xProc)(p1, p2, p3, p4, p5);
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename _TRes>
		_TRes operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, _TRes def)
		{
			if(!bool(*this))return def;
			typedef _TRes (__STDCALL2* _TFarProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6);
			return ((_TFarProc)m_xProc)(p1, p2, p3, p4, p5, p6);
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename _TRes>
		_TRes operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, _TRes def)
		{
			if(!bool(*this))return def;
			typedef _TRes (__STDCALL2* _TFarProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7);
			return ((_TFarProc)m_xProc)(p1, p2, p3, p4, p5, p6, p7);
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename _TRes>
		_TRes operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, _TRes def)
		{
			if(!bool(*this))return def;
			typedef _TRes (__STDCALL2* _TFarProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8);
			return ((_TFarProc)m_xProc)(p1, p2, p3, p4, p5, p6, p7, p8);
		}

	};

public:

	operator bool()const{return isLoaded();}
	_CFunc operator[](const char* lpszFuncName)
	{
		FARPROC p=(FARPROC*)QLibrary::resolve(lpszFuncName); //non-const member, it loads the dll/so if needed;
		return _CFunc(p);
	}

	QLibraryEx(const QString& sLib="") : QLibrary(sLib)
	{
		if(!sLib.isEmpty()){
			if(QLibrary::load()){
				qDebug() << "Dynamically loaded: " << fileName();
			}else{
				qDebug() << "Failed to load DLL: " << fileName();
			}
		}
	}

};

#endif

#endif // DLL_UTILITIES_H
