#ifndef CCKIT_GL_FACTORY_H
#define CCKIT_GL_FACTORY_H

#include <unordered_set>
#include <forward_list>

namespace cckit
{
	template<typename ObjType>
	class GLfactory
	{
	private:
		typedef GLfactory<ObjType> this_type;
	public:
		typedef ObjType object_type;
	public:
		GLfactory();
		~GLfactory();

		static object_type* generate();
		static void destroy(const object_type* _pObj);
	private:
		std::unordered_set<object_type*> mObjs;
	
		static this_type* mpInstance;
		static std::forward_list<this_type*> mInstances;
	};
	template<typename ObjType>
	GLfactory<ObjType>* GLfactory<ObjType>::mpInstance = nullptr;
	template<typename ObjType>
	std::forward_list<GLfactory<ObjType>*> GLfactory<ObjType>::mInstances;

	template<typename ObjType>
	inline GLfactory<ObjType>::GLfactory() 
		: mObjs() {
		mInstances.push_front(mpInstance = this);
	}

	template<typename ObjType>
	inline GLfactory<ObjType>::~GLfactory() {
		for (auto pObj : mObjs)
			delete pObj;
	}

	template<typename ObjType>
	typename GLfactory<ObjType>::object_type* 
		GLfactory<ObjType>::generate() {
		object_type* pObj = new object_type;
		if (mpInstance) mpInstance->mObjs.insert(pObj);
		return pObj;
	}

	template<typename ObjType>
	void GLfactory<ObjType>::destroy(const object_type* _pObj) {
		for (auto pInstance : mInstances)
			if (pInstance->mObjs.erase(const_cast<object_type*>(_pObj)) != 0)
				break;
		delete _pObj;
	}
}

#endif // !CCKIT_GL_FACTORY_H