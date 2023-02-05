#include "garbageCollector.h"
#include "../ErrorHandling/errorHandler.h"
#include "../Codegen/compiler.h"
#include "../Objects/objects.h"
#include <format>

//start size of heap in KB
#define HEAP_START_SIZE 1024


namespace memory {
	GarbageCollector gc = GarbageCollector();

	GarbageCollector::GarbageCollector() {
		heapSize = 0;
		heapSizeLimit = HEAP_START_SIZE*1024;

		shouldCollect = false;
	}

	void* GarbageCollector::alloc(uInt64 size) {
		std::scoped_lock<std::mutex> lk(allocMtx);
		heapSize += size;
		if (heapSize > heapSizeLimit) shouldCollect = true;
		byte* block = nullptr;
		try {
			block = new byte[size];
		}
		catch (const std::bad_alloc& e) {
			errorHandler::addSystemError(std::format("Failed allocation, tried to allocate {} bytes", size));
			throw errorHandler::SystemException();
		}
		objects.push_back(reinterpret_cast<object::Obj*>(block));
		return block;
	}

	void GarbageCollector::collect(runtime::VM* vm) {
		markRoots(vm);
		mark();
		sweep();
		if (heapSize > heapSizeLimit) heapSizeLimit << 1;
		shouldCollect = false;
	}

	void GarbageCollector::collect(compileCore::Compiler* compiler) {
		markRoots(compiler);
		mark();
		sweep();
		if (heapSize > heapSizeLimit) heapSizeLimit << 1;
		shouldCollect = false;
	}

	void GarbageCollector::mark() {
		//we use a stack to avoid going into a deep recursion(which might fail)
		while (!markStack.empty()) {
			object::Obj* ptr = markStack.back();
			markStack.pop_back();
			if (ptr->marked) continue;
			ptr->marked = true;
			ptr->trace();
		}
	}

	void GarbageCollector::markRoots(runtime::VM* vm) {

	}

	void GarbageCollector::markRoots(compileCore::Compiler* compiler) {
		compileCore::CurrentChunkInfo* c = compiler->current;
		while (c->enclosing) {
			c->func->marked = true;
			c->func->trace();
			c = c->enclosing;
		}
		c->func->marked = true;
		c->func->trace();
	}

	void GarbageCollector::sweep() {
		heapSize = 0;
		for (int i = objects.size() - 1; i >= 0; i--) {
			object::Obj* obj = objects[i];
			if (!obj->marked) {
				obj->~Obj();
				objects.erase(objects.begin() + i);
				continue;
			}
			heapSize += obj->getSize();
			obj->marked = false;
		}
	}

	void GarbageCollector::markObj(object::Obj* object) {
		markStack.push_back(object);
	}
}
