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
*/
#ifndef YAKE_NX_MATERIAL_H
#define YAKE_NX_MATERIAL_H

namespace yake {
namespace physics {

	class MaterialNx : public IMaterial
	{
	public:
		MaterialNx( const NxMaterialIndex index );
		MaterialNx();
		~MaterialNx();

		virtual void setRollingFriction(const real friction);
		virtual void setRestitution(const real restitution);
		virtual void setStaticFriction(const real friction);
		virtual void setStaticFrictionV(const real friction);
		virtual void setStaticFrictionVEnabled(bool enabled);

		//-- helpers
		void _createFromDesc( const IMaterial::Desc & rDesc );
		inline NxMaterialIndex _getNxMatIndex() const
		{ return mNxMatIndex; }
	private:
		NxMaterialIndex	mNxMatIndex;
	};

}
}
#endif