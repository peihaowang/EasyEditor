#ifndef CPP_UTILITIES_H
#define CPP_UTILITIES_H

#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>

#define __EACH(it, v) for(it=(v).begin(); it!=(v).end(); ++it)
#define __EACH_R(it, v) for(it=(v).rbegin(); it!=(v).rend(); ++it)

template<class T> class _CDeferCppObjDeletion
{

private:

	T *		m_pObj;

public:

	_CDeferCppObjDeletion(T* p):m_pObj(p){return;}
	virtual ~_CDeferCppObjDeletion(void){if(m_pObj) delete m_pObj;}

};

template<class T> class _CDeferCppObjDeletionList : public std::vector<const T*>{

public:

	virtual ~_CDeferCppObjDeletionList(void){_delete_all();}
	_CDeferCppObjDeletionList(void) : std::vector<const T*>()
	{
		return;
	}

	typedef	typename std::vector<const T*>::iterator	iterator;
	typedef	typename std::vector<const T*>::const_iterator	const_iterator;

	void _delete_all(void)
	{
		const_iterator it;
		for(it=this->begin(); it!=this->end(); ++it){
			const T* p=*it;
			if(p) delete p;
		}
		this->clear();
	}

	_CDeferCppObjDeletionList& operator<<(T* pObj)
	{
		if(pObj) this->push_back(pObj);
		return *this;
	}
};

template<class T> class _CTempValueChange{

private:

	T&	m_valRef;
	T	m_valOld;

public:

	virtual ~_CTempValueChange(void){m_valRef=m_valOld;}
	_CTempValueChange(T& t, const T& newval)
		: m_valRef(t)
		, m_valOld(t)
	{
		m_valRef=newval;
	}

};

template<typename T1, typename T2> class _CBiMap
{

private:

	bool					m_bMulti;
	std::map<T1, T2>			m_mKeyVal;
	std::map<T2, T1>			m_mValKey;

protected:

	void _assert_size(void)const
	{
		if(!m_bMulti){
			//__ASSERT(m_mKeyVal.size() == m_mValKey.size());
		}
	}

public:

	_CBiMap(bool bMulti=false) : m_bMulti(bMulti){return;}

	bool empty(void)const{return size()==0;}
	int size(void)const{_assert_size(); return m_mKeyVal.size();}

	void _insert(const T1& t1, const T2& t2)
	{
		m_mKeyVal[t1]=t2;
		m_mValKey[t2]=t1;
		_assert_size();
	}

	void _insert(const T2& t2, const T1& t1){_insert(t1, t2);}

	bool _erase(const T1& t1)
	{
		bool bDel=false;
		typename std::map<T1, T2>::const_iterator it=m_mKeyVal.find(t1);
		if(it!=m_mKeyVal.end()){
			const T1& t1=it->first;
			const T2& t2=it->second;
			m_mValKey.erase(t2);
			m_mKeyVal.erase(t1);
			bDel=true;
		}
		_assert_size();
		return bDel;
	}

	bool _erase(const T2& t2)
	{
		bool bDel=false;
		typename std::map<T2, T1>::const_iterator it=m_mValKey.find(t2);
		if(it!=m_mValKey.end()){
			const T1& t1=it->second;
			const T2& t2=it->first;
			m_mKeyVal.erase(t1);
			m_mValKey.erase(t2);
			bDel=true;
		}
		_assert_size();
		return bDel;
	}

	template<typename _CPred> void _each_key(_CPred& xAct)const
	{
		typename std::map<T1, T2>::const_iterator it;
		for(it=m_mKeyVal.begin(); it!=m_mKeyVal.end(); ++it){
			xAct(it->first, it->second);
		}
	}

	template<typename _CPred> void _each_val(_CPred& xAct)const
	{
		typename std::map<T2, T1>::const_iterator it;
		for(it=m_mValKey.begin(); it!=m_mValKey.end(); ++it){
			xAct(it->first, it->second);
		}
	}

	T2 operator()(const T1& t1, const T2& xDef)const
	{
		T2 t=xDef;
		typename std::map<T1, T2>::const_iterator it=m_mKeyVal.find(t1);
		if(it!=m_mKeyVal.end()){
			t=it->second;
		}
		return t;
	}

	T1 operator()(const T2& t2, const T1& xDef)const
	{
		T1 t=xDef;
		typename std::map<T2, T1>::const_iterator it=m_mValKey.find(t2);
		if(it!=m_mValKey.end()){
			t=it->second;
		}
		return t;
	}

	bool _exists(const T1& t1)const{return m_mKeyVal.find(t1)!=m_mKeyVal.end();}
	bool _exists(const T2& t2)const{return m_mValKey.find(t2)!=m_mValKey.end();}

	T1 _last(const T1& xDef)const
	{
		T1 t=xDef;
		if(!m_mKeyVal.empty()){
			typename std::map<T1, T2>::const_reverse_iterator it=m_mKeyVal.rbegin();
			t=it->first;
		}
		return t;
	}

	T2 _last(const T2& xDef)const
	{
		T2 t=xDef;
		if(!m_mValKey.empty()){
			typename std::map<T2, T1>::const_reverse_iterator it=m_mValKey.rbegin();
			t=it->first;
		}
		return t;
	}

};

//2007.6.18 This sorted-vector allows duplicates of elements, while std::set not;
//std::multiset does, but it is not commonly supported by c++ compilers;
template<typename T> class _CSortedVector : protected std::vector<T>{

private:

	bool					m_bUnique;

public:

	//2007.7.19 G++ requires 'typename', or it gives warning: 'implicit typename is deprecated'
	//http://www.slac.stanford.edu/BFROOT/www/Computing/Programming/GCCWarnings.html
	//http://gcc.gnu.org/ml/gcc-help/2003-01/msg00246.html
	typedef	typename std::vector<T>::size_type			size_type;
	typedef	typename std::vector<T>::const_reference		const_reference;

	typedef	typename std::vector<T>::iterator			iterator;
	typedef	typename std::vector<T>::const_iterator			const_iterator;

	typedef	typename std::vector<T>::reverse_iterator		reverse_iterator;
	typedef	typename std::vector<T>::const_reverse_iterator		const_reverse_iterator;

	//2009.5.16 add 'protected' for base class;
	bool empty()const{return ((const std::vector<T>*)this)->empty();}
	void clear()const{((std::vector<T>*)this)->clear();}
	size_type size()const{return ((const std::vector<T>*)this)->size();}
	size_type capacity()const{return ((const std::vector<T>*)this)->capacity();}
	void reserve(size_type n){((std::vector<T>*)this)->reserve(n);}
	void resize(size_type n, T x = T()){((std::vector<T>*)this)->resize(n, x);}

	const_reference operator[](size_type pos)const{return ((const std::vector<T>&)*this)[pos];}
	const_reference at(size_type pos)const{return ((std::vector<T>*)this)->at(pos);}

	iterator end(){return ((std::vector<T>*)this)->end();}
	const_iterator end()const{return ((const std::vector<T>*)this)->end();}

	iterator begin(){return ((std::vector<T>*)this)->begin();}
	const_iterator begin()const{return ((const std::vector<T>*)this)->begin();}

	const_reverse_iterator rbegin()const{return ((const std::vector<T>*)this)->rbegin();}
	reverse_iterator rbegin(){return ((std::vector<T>*)this)->rbegin();}

	const_reverse_iterator rend()const{return ((const std::vector<T>*)this)->rend();}
	reverse_iterator rend(){return ((std::vector<T>*)this)->rend();}

public:

	//2007.8.21 GCC/FreeBSD6 requires 'this->' for base-members here;
	void _sort(void){std::sort(this->begin(), this->end());}

	//2008.4.29 NOTE: iterator -> T* may throw exceptions; AVOID please!!
	iterator _find(const T& d)
	{
		iterator itRes=this->end(), it=std::lower_bound(this->begin(), this->end(), d);
		if(it!=this->end() && (*it)==d ) itRes=it;
		return itRes;
	}

	const_iterator _find(const T& d)const
	{
		const_iterator itRes=this->end(), it=std::lower_bound(this->begin(), this->end(), d);
		if(it!=this->end() && (*it)==d ) itRes=it;
		return itRes;
	}

	bool _exists(const T& d)const
	{
		bool bExists=false;
		iterator it=std::lower_bound(this->begin(), this->end(), d);
		if(it!=this->end()){
			if(d==*it){
				bExists=true;
			}
		}
		return bExists;
	}

	bool _insert(const T& d)
	{
		bool bSucc=true;
		iterator it=std::lower_bound(this->begin(), this->end(), d);
		if(it!=this->end()){
			if( d==*it && m_bUnique ){
				*it=d; //overwrite;
				bSucc=false; //already exists!
			}else{
				this->insert(it, d);
			}
		}else{
			this->push_back(d);
		}
		return bSucc;
	}

	int _erase(const T& d)
	{
		//2008.4.28 this function requires to add the compile option: '/Zm200';
		//or MSVC6 gives: 'fatal error C1076: compiler limit : internal heap limit reached';
		//http://flipcode.spaces.live.com/blog/cns!8E578E7901A88369!587.entry

		int n=this->size();

		#if 0

		//2007.5.27 this code works slower;
		erase(std::remove(this->begin(), this->end(), d), this->end());

		#elif 1

		//right for sorted vector;
		std::pair<iterator, iterator> p=std::equal_range(this->begin(), this->end(), d);
		//if(p.first!=p.second)
		this->erase(p.first, p.second);

		#elif 0

		//2008.4.27 this code may work faster;
		iterator it1=_find(d);
		if(it1!=this->end()){
			if(m_bUnique){
				this->erase(it1);
			}else{
				iterator it2=std::upper_bound(this->begin(), this->end(), d);
				if(it1!=it2) this->erase(it1, it2);
			}
		}

		#else

		//2008.4.28 this code may work faster;
		iterator it=_find(d);
		if(it!=this->end()){
			this->erase(std::remove(it, this->end(), d), this->end());
		}

		#endif

		return n-this->size();
	}

	template<typename _CPredIf> int _erase_if(_CPredIf& xPred)
	{
		int n=this->size();
		erase(std::remove_if(this->begin, this->end(), xPred), this->end());
		return n-this->size();
	}

public:

	_CSortedVector(bool bUnique) : m_bUnique(bUnique)
	{
		return;
	}

	_CSortedVector(const std::vector<T>& d, bool bUnique) : m_bUnique(bUnique)
	{
		//2008.10.7 Sorting implicitly, care for the sort of the vector 'd';
		*this=d;
	}

	_CSortedVector& operator=(const std::vector<T>& d){return _assign(d);}
	_CSortedVector& _assign(const std::vector<T>& d)
	{
		this->clear();
		typename std::vector<T>::const_iterator it;
		for(it=d.begin(); it!=d.end(); ++it){
			_insert(*it);
		}
		return *this;
	}

	bool _unique(void)const{return m_bUnique;}

};

//2009.11.6 This class works like std::map<>, except it keeps elements in original order added, without sorting;
template<typename T1, typename T2> class _CPairVector : public std::vector< std::pair<T1, T2> >{

public:

	using std::vector< std::pair<T1, T2> >::begin;
	using std::vector< std::pair<T1, T2> >::end;
	using std::vector< std::pair<T1, T2> >::size;

	typedef	typename std::vector< std::pair<T1, T2> >::iterator		iterator;
	typedef	typename std::vector< std::pair<T1, T2> >::const_iterator	const_iterator;

	iterator _find(const T2& d2)
	{
		iterator it=this->end();
		for(it=begin(); it!=end(); ++it){
			if(it->second==d2){
				break;
			}
		}
		return it;
	}

	const_iterator _find(const T2& d2)const
	{
		const_iterator it=end();
		for(it=begin(); it!=end(); ++it){
			if(it->second==d2){
				break;
			}
		}
		return it;
	}

	iterator _find(const T1& d1)
	{
		iterator it=end();
		for(it=begin(); it!=end(); ++it){
			if(it->first==d1){
				break;
			}
		}
		return it;
	}

	const_iterator _find(const T1& d1)const
	{
		const_iterator it=end();
		for(it=begin(); it!=end(); ++it){
			if(it->first==d1){
				break;
			}
		}
		return it;
	}

public:

	_CPairVector(void){return;}

	T1& operator[](const T2& d2)
	{
		iterator it=_find(d2);
		if(it!=end()){
			return it->first;
		}else{
			this->push_back(std::pair<T1, T2>());
			it=begin()+(size()-1);
			it->second=d2;
			return it->first;
		}
	}

	T2& operator[](const T1& d1)
	{
		iterator it=_find(d1);
		if(it!=end()){
			return it->second;
		}else{
			this->push_back(std::pair<T1, T2>());
			it=begin()+(size()-1);
			it->first=d1;
			return it->second;
		}
	}

	T1 operator()(const T2& d2, const T1& xDef)const
	{
		const_iterator it;
		for(it=begin(); it!=end(); ++it){
			if(it->second==d2){
				return it->first;
			}
		}
		return xDef;
	}

	T2 operator()(const T1& d1, const T2& xDef)const
	{
		const_iterator it;
		for(it=begin(); it!=end(); ++it){
			if(it->first==d1){
				return it->second;
			}
		}
		return xDef;
	}

};

template<typename T> struct _CZeroStruct : public T{

public:

	_CZeroStruct<T>(void){::memset(this, 0, sizeof(*this));}

};

//2009.8.24 These 'predicate' containers are usually passed into the 'enumerate/list' function as a container;
template<typename T> class _CPredVector : public std::vector<T>{

public:

	virtual void operator()(const T& t){this->push_back(t);}

};

template<typename T> class _CPredSet : public std::set<T>{

public:

	virtual void operator()(const T& t){this->insert(t);}

};

#endif // CPP_UTILITIES_H
