#ifndef YAKE_PROP_RTTI_CLASS_H
#define YAKE_PROP_RTTI_CLASS_H

#include "prerequisites.h"
#include <sstream>

namespace yake {

	typedef std::vector<PropDef> PropDefList;
	typedef std::map<std::string,PropDef> PropDefMap;
	struct RttiClass;
	struct RttiClassTemplate;
	struct RttiClassMgr;
	typedef std::set<const RttiClass*> RttiClassConstPtrSet;
	struct RttiClass
	{
	public:
		// "Class c'tor"
		RttiClass(const std::string& name) : name_(name) {}
		RttiClass(const std::string& name, const std::string& parent0) : 
		name_(name) 
		{
			addParent( parent0 );
		}
		RttiClass(const std::string& name, const std::string& parent0, const std::string& parent1) : 
		name_(name) 
		{
			addParent( parent0 );
			addParent( parent1 );
		}
		// "ClassTemplate c'tor"
		RttiClass(const std::string& name, const RttiClass& orig) :
		name_(name),
			props_(orig.props_),
			parents_(orig.parents_),
			parentNames_(orig.parentNames_),
			parentPtrs_(orig.parentPtrs_)
		{
		}
		std::string name() const
		{ return name_; }

		typedef RttiClassConstPtrSet ParentClassList;
		const ParentClassList& getParents() const
		{ return parentPtrs_;  }

		typedef std::deque<std::string> StringList;
		const StringList& getParentNames() const
		{
			return parentNames_;
		}

		void addParent(const std::string& clsName)
		{
			assert( !clsName.empty() );
			assert( parents_.find(clsName) == parents_.end() );
			parents_[ clsName ] = 0;
			parentNames_.push_back( clsName );
		}

		const PropDefMap& getPropDefs() const
		{ return props_; }
		void add(const PropDef&);

		//void setName_(const std::string& name) { name_ = name; }
	private:
		friend struct RttiClassMgr;
		void setParent(const std::string& clsName, RttiClass* cls)
		{
			assert( cls );
			assert( !clsName.empty() );
			assert( parents_.find(clsName) != parents_.end() );
			parents_[ clsName ] = cls;
			parentPtrs_.insert(cls);
		}
	private:
		std::string						name_;
		PropDefMap						props_;

		typedef std::map<std::string,RttiClass*> ParentClassMap;
		ParentClassMap					parents_;
		StringList						parentNames_;
		ParentClassList				parentPtrs_;
	};
	inline std::ostream& operator << (std::ostream& out, const RttiClass& cls)
	{
		out << "RttiClass(name='" << cls.name() << "')\n";
		for (RttiClass::StringList::const_iterator itClsName = cls.getParentNames().begin();  itClsName != cls.getParentNames().end(); ++itClsName)
			out << "   inherits from '" << *itClsName << "' (name)\n";
		for (RttiClassConstPtrSet::const_iterator itCls = cls.getParents().begin(); itCls != cls.getParents().end(); ++itCls)
			out << "   inherits from '" << (*itCls)->name() << "' (ptr)\n";
		for (PropDefMap::const_iterator it = cls.getPropDefs().begin(); it != cls.getPropDefs().end(); ++it)
		{
			out << "   property '" << it->first.c_str() << "' = " << it->second.type() << "\n";
		}
		return out;
	}

	struct RttiException : public std::exception
	{
		RttiException(const std::string& msg, const std::string& loc = "", const char* file = 0, const int line = 0)
		{
			std::stringstream out;
			out << "RttiException:\n";
			out << "  Reason: " << msg << "\n";
			if (!loc.empty())
				out << "  Location: " << loc << "\n";
			if (file)
				out << "  File: " << file << "@" << line << "\n";
			msg_ = out.str();
		}
		virtual ~RttiException() throw()
		{
		    // empty implementation. Required to avoid "looser throw specifier error".
		}
		const char* what() const throw ()
		{ return msg_.c_str(); }
	private:
		std::string		msg_;
	};
#define THROW_RTTI(MSG) \
	throw RttiException(MSG,"",__FILE__,__LINE__)
#define THROW_RTTI_LOC(MSG,LOC) \
	throw RttiException(MSG,LOC,__FILE__,__LINE__)

#if YAKE_COMPILER == COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable: 4290) // C4290: C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#endif
	struct RttiClassMgr
	{
		// insert into existing hierarchy
		void insert(RttiClass*) throw(RttiException);
		void erase(RttiClass*) throw(RttiException);
		RttiClass* get(const std::string&) const throw() ;
	private:
		typedef std::map<std::string,RttiClass*> StringClassPtrMap;
		StringClassPtrMap	classes_;
	};
#if YAKE_COMPILER == COMPILER_MSVC
#pragma warning(pop)
#endif
	inline RttiClass* RttiClassMgr::get(const std::string& clsName) const throw ()
	{
		StringClassPtrMap::const_iterator it = classes_.find(clsName);
		return (it == classes_.end()) ? 0 : it->second;
	}
#if YAKE_COMPILER == COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable: 4290) // C4290: C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#endif
	inline void RttiClassMgr::insert(RttiClass* cls) throw(yake::RttiException)
	{
		assert( cls );
		if (!cls)
			THROW_RTTI_LOC("Null class pointer.","RttiClassMgr::insert");
		const std::string clsName = cls->name();
		if (clsName.empty())
			THROW_RTTI_LOC("Empty class name.","RttiClassMgr::insert");
		StringClassPtrMap::const_iterator it = classes_.find(clsName);
		if (it != classes_.end())
			THROW_RTTI_LOC("Entry for this class name already exists!","RttiClassMgr::insert");

		// find parents and other referenced objects
		const RttiClass::StringList& parentNames = cls->getParentNames();
		if (!parentNames.empty())
		{
			for (RttiClass::StringList::const_iterator itName = parentNames.begin(); itName != parentNames.end(); ++itName)
			{
				RttiClass* parentCls = get(*itName);
				if (parentCls)
					cls->setParent(*itName,parentCls);
				else
					THROW_RTTI_LOC("Parent class not registered!","RttiClassMgr::insert");
			}
		}

		// finally insert
		classes_.insert( std::make_pair(clsName,cls) );

		// post-process
	}
#if YAKE_COMPILER == COMPILER_MSVC
#pragma warning(pop)
#endif
	inline void RttiClass::add(const PropDef & propdef)
	{
		props_[ propdef.name() ] = propdef;
	}

	#define DECL_CLASS_RTTI(CLASS,DESC) \
	public: \
		static std::string clsName()  \
		{ return std::string( #CLASS ); } \
		static std::string clsDescription()  \
		{ return std::string( DESC ); } \
		static ::yake::RttiClass* cls() \
		{ return &ms_cls; } \
		virtual ::yake::RttiClass* isA() const \
		{ return &ms_cls; } \
	private: \
		static ::yake::RttiClass ms_cls; \
	public:

	#define IMPL_CLASS_RTTI(CLASS) \
		::yake::RttiClass CLASS::ms_cls( CLASS::clsName() );

	#define IMPL_CLASS_RTTI_1(CLASS,PARENT1) \
		::yake::RttiClass CLASS::ms_cls( CLASS::clsName(), PARENT1 );

} // namespace yake

#endif
