#ifndef __OGLBUFFER_H__
#define __OGLBUFFER_H__

#include <H3D/OGLUtil.h>
#include <H3D/OGLBufferLock.h>
#include <H3DUtil/Console.h>

#include <GL/glew.h>

	static const GLbitfield defaultMapFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
	static const GLbitfield defaultCreateFlags = defaultMapFlags | GL_DYNAMIC_STORAGE_BIT;

namespace H3D
{
	// --------------------------------------------------------------------------------------------------------------------
	enum BufferStorage {
		SystemMemory = 0,
		PersistentlyMappedBuffer
	};

	// --------------------------------------------------------------------------------------------------------------------
	template <typename Atom>
	class Buffer
	{
	public:
		Buffer(bool _cpuUpdates)
			: lockManager(_cpuUpdates), bufferContents(), name(0), target(), bufferStorage(SystemMemory)
		{ 
		}

		~Buffer() {
			Destroy();
		}

		bool Create(BufferStorage _storage, GLenum _target, GLuint _atomCount, 
			GLbitfield _createFlags = defaultCreateFlags, GLbitfield _mapFlags = defaultMapFlags) {
			//If we already had data, clean that up first
			if(bufferContents) {
				Destroy();        
			}

			//Sanity check :)
			if(bufferStorage == PersistentlyMappedBuffer) {
#ifndef GLEW_ARB_buffer_storage
				std::cout << "Creating a persistently mapped buffer failed because ARB_buffer_storage is not supported.";
				return false;
#endif
			}

			bufferStorage = _storage;
			target = _target;

			switch (bufferStorage) 
			{
			case SystemMemory: 
				{
					bufferContents = new Atom[_atomCount];                
					break;
				}

			case PersistentlyMappedBuffer: 
				{
					// This code currently doesn't care about the alignment of the returned memory. This could potentially
					// cause a crash, but since implementations are likely to return us memory that is at least aligned
					// on a 64-byte boundary we're okay with this for now. 
					// A robust implementation would ensure that the memory returned had enough slop that it could deal
					// with it's own alignment issues, at least. That's more work than I want to do right this second.

					glGenBuffers(1, &name);
					glBindBuffer(target, name);

					unsigned int bufferSize = sizeof(Atom) * _atomCount;

					glBufferStorage(target, bufferSize, nullptr, _createFlags);
					bufferContents = reinterpret_cast<Atom*>(glMapBufferRange(target, 0, bufferSize, _mapFlags));

					if(!bufferContents) 
					{
						std::cout << "glMapBufferRange failed, probable bug." << std::endl;
						return false;
					}
					break;
				}

			default: 
				{
					std::cout << ("Error, need to update Buffer::Create with logic for bufferStorage = %d", bufferStorage);
					break;
				}
			};

			return true;
		}

		// Called by dtor, must be non-virtual.
		void Destroy() 
		{
			switch (bufferStorage) 
			{
			case SystemMemory: 
				{
					delete[] bufferContents;
					bufferContents = 0;
					break;
				}

			case PersistentlyMappedBuffer: 
				{
					glBindBuffer(target, name);
					glUnmapBuffer(target);
					glDeleteBuffers(1, &name);

					bufferContents = 0;
					name = 0;

					break;
				}

			default: 
				{
					std::cout << ("Error, need to update Buffer::Create with logic for bufferStorage = %d", bufferStorage);
					break;
				}
			};
		}

		void WaitForLockedRange(size_t _lockBegin, size_t _lockLength) { lockManager.WaitForLockedRange(_lockBegin, _lockLength); }
		Atom* GetContents() { return bufferContents; }
		void LockRange(size_t _lockBegin, size_t _lockLength) { lockManager.LockRange(_lockBegin, _lockLength); }

		void BindBuffer() { glBindBuffer(target, name); }
		void BindBufferBase(GLuint _index) { glBindBufferBase(target, _index, name); }
		void BindBufferRange(GLuint _index, GLsizeiptr _head, GLsizeiptr _atomCount) 
		{ 
			glBindBufferRange(target, _index, name, _head * sizeof(Atom), _atomCount * sizeof(Atom)); 
		}

	private:
		OGLBufferLockManager lockManager;
		Atom* bufferContents;

		GLuint name;
		GLenum target;

		BufferStorage bufferStorage;
	};

	// --------------------------------------------------------------------------------------------------------------------
	template <typename Atom>
	class CircularBuffer
	{
	public:
		CircularBuffer(bool _cpuUpdates = true)
			: buffer(_cpuUpdates)
		{ 
		}

		bool Create(BufferStorage _storage, GLenum _target, GLuint _atomCount, GLbitfield _createFlags = defaultCreateFlags, GLbitfield _mapFlags = defaultMapFlags)
		{
			head = 0;
			sizeAtoms = _atomCount;

			return buffer.Create(_storage, _target, _atomCount, _createFlags, _mapFlags);
		}

		void Destroy() 
		{
			buffer.Destroy();
			sizeAtoms = 0;
			head = 0;
		}

		Atom* Reserve(GLsizeiptr _atomCount)
		{
			if(_atomCount >= sizeAtoms) 
			{
				std::cout << "Requested an update of size " << _atomCount << " for a buffer of size " << sizeAtoms << " atoms." << std::endl;
			}

			//GLsizeiptr lockStart = head;

			if(head + _atomCount >= sizeAtoms) 
			{
				// Need to wrap here.
				head = 0;
			}

			buffer.WaitForLockedRange(head, _atomCount);
			return &buffer.GetContents()[head];
		}

		void OnUsageComplete(GLsizeiptr _atomCount)
		{
			buffer.LockRange(head, _atomCount);
			head = (head + _atomCount) % sizeAtoms;
		}

		void BindBuffer() { buffer.BindBuffer(); }
		void BindBufferBase(GLuint _index) { buffer.BindBufferBase(_index); }
		void BindBufferRange(GLuint _index, GLsizeiptr _atomCount) { buffer.BindBufferRange(_index, head, _atomCount); }

		GLsizeiptr GetHead() const { return head; }
		void* GetHeadOffset() const { return (void*)(head * sizeof(Atom)); }
		GLsizeiptr GetSize() const { return sizeAtoms; }

	private:
		Buffer<Atom> buffer;

		GLsizeiptr head;
		GLsizeiptr sizeAtoms;
	};
}
#endif