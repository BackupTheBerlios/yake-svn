/**------------------------------------------------------------------------------------
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
#ifndef YAKE_DATA_DOM_SERIALIZER_H
#define YAKE_DATA_DOM_SERIALIZER_H

namespace yake {
	using namespace base::templates;
	using namespace base;
namespace data {
namespace dom {

#define varGet ttl::var::get //FIXME: move to yake::templates::var ...

	// parser for (hierarchically) organized data.

	class INode;
	typedef YAKE_DATA_API templates::Vector< SharedPtr<INode> > NodeList;
	class YAKE_DATA_API INode
	{
	public:
		// Variants... the evil :)
		enum ValueTypeId {
			VTID_UINT8		= 0,
			VTID_INT8		= 1,
			VTID_UINT16		= 2,
			VTID_INT16		= 3,
			VTID_UINT32		= 4,
			VTID_INT32		= 5,
			VTID_REAL		= 6,
			VTID_STRING		= 7
		};
		typedef Variant<
						uint8,
						int8,
						uint16,
						int16,
						uint32,
						int32,
						real,
						String> ValueType;
		typedef templates::AssocVector< String, ValueType > AttributeMap;

		// nodes
		virtual void addNode( SharedPtr<INode> pNode ) = 0;
		virtual SharedPtr<INode> getNodeByName( const String & name ) const = 0;
		virtual SharedPtr<INode> getNodeById( const String & id ) const = 0;
		virtual const NodeList& getNodes() const = 0;

		//virtual NodeList::Iterator getFirstNode() const = 0;
		//virtual NodeList::Iterator getNodeIterator() const = 0;

		// node values
		virtual ValueType getValue( const String & name ) const = 0;
		virtual void setValue( const String & name, const ValueType & value ) = 0;

		// attributes
		virtual const AttributeMap& getAttributes() const = 0;
		virtual ValueType getAttributeValue( const String & name ) const = 0;
		virtual void setAttributeValue( const String & name, const ValueType & value ) = 0;

		template< typename T >
		T getAttributeValueAs( const String & name ) const
		{
			ValueType val = getAttributeValue(name);
			return varGet<T>( val );
		}

		template<>
		real getAttributeValueAs( const String & name ) const
		{
			ValueType val = getAttributeValue(name);
			return varGet<real>( val );
		}
	};


	/** Generic DOM serializer interface.
		It can be used to parse (almost) any kind of (hierarchically or flatly)
		organised data.
		Concrete implementations may want to read data from sources like XML,
		RDBMS, INI style files etc.
		It allows for two ways of parsing:
		1) Using Signals ("callbacks")
			Signals will be fired whenever a new node or attribute is encountered
			during parsing. Clients can subscribe to these signals.
			NB No guarantees on consistency can be made! If, for example, data
				is referenced that has not yet been read then you obviously cannot access it.
		2) Querying / Interrogating
			After a call to parse() succeeded clients can query the data by
			getting the document node (getDocumentNode()) and iterating over nodes,
			child nodes (and their attributes) as needed.
			NB Consistency is bound on the consistency of the source data.
				But, for example, references between nodes can always be resolved (as
				long as the source data was intact, of course).
			NB The document node is not part of the source/target document.
		\todo make it a factory!
	*/
	class YAKE_DATA_API ISerializer
	{
	public:
		virtual String getName() const = 0;
		virtual Version getVersion() const = 0;

		/** Parse a data chunk. Several calls to parser() can be made in order to
			fully parse a document. This may prove useful when streaming data, for example.
			The caller has to make sure the given data chunk contains valid data and can
			be read by the serializer!
			\todo make it virtual void parse( const DataChunk & datachunk ) = 0;
		*/
		virtual void parse( const String & file, bool bFireSignals ) = 0;

		/** Serializes the document and all the data stored in the DOM tree into the
			target DataChunk object.
		*/
		//\todo:virtual void store( DataChunk & datachunk ) = 0;

		/** Prepare for a new load/parse run. */
		virtual void reset() = 0;

		/** Returns a pointer to the document node. The data parsed from the input can be found
			in its child nodes.
		*/
		virtual SharedPtr<INode> getDocumentNode() const = 0;

	protected:
		typedef Signal1<void(SharedPtr<INode>)>				NewNodeSignal;
		NewNodeSignal									mNewNodeSignal;
		typedef Signal1<void(const String&)>	NewAttributeSignal;
		NewAttributeSignal							mNewAttributeSignal;

	public:
		void subscribeToNewNodeSignal( const NewNodeSignal::slot_type & slot );
		void subscribeToNewAttributeSignal( const NewAttributeSignal::slot_type & slot );

	};

} // dom
} // data
} // yake


#endif