//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004-2014, SenseGraphics AB
//
//    This file is part of H3D API.
//
//    H3D API is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    H3D API is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with H3D API; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//    A commercial license is also available. Please contact us at 
//    www.sensegraphics.com for more information.
//
//
/// \file RenderCommandBuffer.h
/// \brief Header file for RenderCommandBuffer, data structure that self-sorts and appends all render commands into one long buffer
///
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __RENDERCOMMANDBUFFER_H__
#define __RENDERCOMMANDBUFFER_H__

#include <H3D/RenderCommands.h>
#include <H3D/OGLBuffer.h>
#include <H3D/RenderMetaData.h>
#include <vector>

namespace H3D
{
	class RenderCommandBuffer
	{
	public:
		RenderCommandBuffer(unsigned int numCommands = 0);
		~RenderCommandBuffer();

		//Clear entire buffer, deleting every command
		void Clear();

		void InsertNewCommand(RenderCommand* command);

		//And this is how we render the entire scene...
		void Render();

		CircularBuffer<DrawCommand>& GetBuffer() {
			return commandBuffer;
		}

	private:
		std::vector<RenderCommand*> commandQueue;
		CircularBuffer<DrawCommand> commandBuffer;
	};
}
#endif