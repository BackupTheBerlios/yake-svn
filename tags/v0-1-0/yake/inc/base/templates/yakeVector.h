/**------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright � 2004 The YAKE Team
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
namespace yake
{
namespace base
{
namespace templates
{

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

} // templates
} // base
} // yake


#endif // YAKE_BASE_TEMPLATES_VECTOR_H
