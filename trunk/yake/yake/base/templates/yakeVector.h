/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright © 2004 The YAKE Team
   For the latest information visit http://www.yake.org 
   ------------------------------------------------------------------------------------
   This program is free software; you can redistribute it and/or modify it under
   the terms of the GNU Lesser General Public License as published by the Free Software
   Foundation; either version 2 of the License, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License along with
   this program; if not, write to the Free Software Foundation, Inc., 59 Temple
   Place - Suite 330, Boston, MA 02111-1307, USA, or go to
   http://www.gnu.org/copyleft/lesser.txt.
   ------------------------------------------------------------------------------------
   If you are interested in another license model contact the Yake Team via
   E-Mail: team@yake.org.
   For more information see the LICENSE file in the root directory of the
   source code distribution.
   ------------------------------------------------------------------------------------
*/
#ifndef YAKE_BASE_TEMPLATES_VECTOR_H
#define YAKE_BASE_TEMPLATES_VECTOR_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASEPREREQUISITES_H
#include "../yakePrerequisites.h"
#endif

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {

template< class _Value, class _Alloc = std::allocator< _Value > >
class Vector : private std::vector< _Value >
{
// Types
private:
  typedef std::vector< _Value > _MyBase;

public:
  typedef typename _MyBase::value_type value_type;
  typedef typename _MyBase::iterator iterator;
  typedef typename _MyBase::const_iterator const_iterator;
  typedef typename _MyBase::size_type size_type;
  typedef typename _MyBase::difference_type difference_type;
  typedef typename _MyBase::reverse_iterator reverse_iterator;
  typedef typename _MyBase::const_reverse_iterator const_reverse_iterator;
  typedef typename _MyBase::pointer pointer;
  typedef typename _MyBase::const_pointer const_pointer;

  typedef typename _Alloc::reference reference;
  typedef typename _Alloc::const_reference const_reference;
        
// Class
public:
  explicit Vector( const _Alloc& rAlloc = _Alloc() ) : _MyBase( rAlloc ) {}
        
// Methods
public:
	 // Iterators
	 iterator begin() 
	 { return _MyBase::begin(); }

   const_iterator begin() const 
	 { return _MyBase::begin(); }

   iterator end() 
	 { return _MyBase::end(); }

   const_iterator end() const 
	 { return _MyBase::end(); }

   reverse_iterator rbegin() 
	 { return _MyBase::rbegin(); }

   const_reverse_iterator rbegin() const 
	 { return _MyBase::rbegin(); }

   reverse_iterator rend() 
	 { return _MyBase::rend(); }

   const_reverse_iterator rend() const 
	 { return _MyBase::rend(); }
        

   // Capacity
   size_type size() const 
	 { return _MyBase::size(); }

   size_type max_size() 
	 { return _MyBase::max_size(); }

	 bool empty() const 
	 { return _MyBase::empty(); }

	 void reserve(size_t n)
	 {
		 _MyBase::reserve(n);
	 }


   // Insert/Delete
	 void push_back( const _Value& Value )
	 { _MyBase::push_back( Value ); }	

	 void pop_back()
	 { _MyBase::pop_back(); }

 	 iterator insert( iterator Where, const _Value& Value )
	 { return _MyBase::insert( Where, Value ); }

	 void insert( iterator Where, size_type Count, const _Value& Value )
	 { _MyBase::insert( Where, Count, Value ); }

   void erase( iterator pos )
   { _MyBase::erase( pos ); }

   void erase(iterator first, iterator last)
   { _MyBase::erase(first, last); }
       
   void clear()
   { _MyBase::clear(); }

	 void resize( size_type Newsize )
	 {	_MyBase::resize( Newsize );	}

	 void resize( size_type Newsize, _Value Value )
	 {	_MyBase::resize( Newsize, Value ); }


	 // Assign
	 Vector& operator=( const Vector& rVector )
   {
     Vector( rVector ).swap( *this ); 
     return *this;
   }

	 void assign( size_type Count, const _Value& rValue )
	 {	_MyBase::assign( Count, rValue ); }

   void swap( Vector& rOther )
   {
       using std::swap;
       _MyBase::swap( rOther );
   }


	 // Elements
   reference operator[]( size_type Pos )
	 { return _MyBase::operator[]( Pos ); }

   const_reference operator[]( size_type Pos ) const
	 { return _MyBase::operator[]( Pos ); }

	 reference at( size_type Pos )
	 { return _MyBase::at( Pos ); }

	 const_reference at( size_type Pos ) const
	 { return _MyBase::at( Pos ); }

	 reference front()
	 { return _MyBase::front();	}

	 const_reference front() const
	 { return _MyBase::front();	}

	 reference back()
	 { return _MyBase::back(); }

	 const_reference back() const
	 { return _MyBase::back(); }


	 // Compare
   friend bool operator==( const Vector& lhs, const Vector& rhs )
   {
       const _MyBase& me = lhs;
       return me == rhs;
   }        

};

	/** The VectorIterator and ConstVectorIterator are based on the templates
		of the same name from OGRE (http://www.ogre3d.org).
	*/

	template < class T >
	class VectorIterator
	{
	private:
		typename T::iterator mCurrent;
		typename T::iterator mEnd;
		VectorIterator() {}
	public:
		VectorIterator(typename T::iterator start, typename T::iterator end) :
			mCurrent( start ), mEnd( end )
		{
		}
		VectorIterator(T& container) :
			mCurrent( container.begin() ), mEnd( container.end() )
		{
		}
		bool hasMoreElements()
		{
			return mCurrent != mEnd;
		}
		/** Returns the next element in the collection, and advances to the next. */
		typename T::value_type getNext(void)
		{
			return *mCurrent++;
		}
		/** Returns the next element in the collection, without advancing to the next. */
		typename T::value_type peekNext(void)
		{
			return *mCurrent;
		}
		/** Returns a pointer to the next element in the collection, without advancing to the next afterwards. */
		typename T::pointer peekNextPtr(void)
		{
			return &(*mCurrent);
		}
		/** Moves the iterator on one element. */
		void moveNext(void)
		{
			mCurrent++;
		}
	};
	template < class T >
	class ConstVectorIterator
	{
	private:
		typename T::const_iterator mCurrent;
		typename T::const_iterator mEnd;
		ConstVectorIterator() {}
	public:
		ConstVectorIterator(typename T::const_iterator start, typename T::const_iterator end) :
			mCurrent( start ), mEnd( end )
		{
		}
		ConstVectorIterator(const T& container) :
			mCurrent( container.begin() ), mEnd( container.end() )
		{
		}
		bool hasMoreElements()
		{
			return mCurrent != mEnd;
		}
		/** Returns the next element in the collection, and advances to the next. */
		typename T::value_type getNext(void)
		{
			return *mCurrent++;
		}
		/** Returns the next element in the collection, and advances to the next. */
		const typename T::value_type& getNextRef(void)
		{
			return *mCurrent++;
		}
		/** Returns the next element in the collection, without advancing to the next. */
		typename T::value_type peekNext(void)
		{
			return *mCurrent;
		}
		/** Returns a pointer to the next element in the collection, without advancing to the next afterwards. */
		typename T::const_pointer peekNextPtr(void)
		{
			return &(*mCurrent);
		}
		/** Moves the iterator on one element. */
		void moveNext(void)
		{
			mCurrent++;
		}
	};


	template< typename T >
	Vector< T > split( const T& where, const T& separator )
	{
		Vector< T > result;

		typename T::size_type oldpos = 0, pos = 0;
		while( ( pos = where.find( separator, pos ) ) != T::npos )
		{
			result.push_back( where.substr( oldpos, pos - oldpos ) );

			pos += separator.size();
			oldpos = pos;
		}

		result.push_back( where.substr( oldpos, pos - oldpos ) );

		return result;
	}

} // yake


#endif // YAKE_BASE_TEMPLATES_VECTOR_H
