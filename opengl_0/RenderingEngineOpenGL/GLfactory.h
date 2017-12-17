#ifndef CCKIT_GL_FACTORY_H
#define CCKIT_GL_FACTORY_H

#include <unordered_set>

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
	private:
		std::unordered_set<object_type*> mObjs;
	
		static this_type* pInstance;
	};
	template<typename ObjType>
	GLfactory<ObjType>* GLfactory<ObjType>::pInstance = nullptr;

	template<typename ObjType>
	inline GLfactory<ObjType>::GLfactory() 
		: mObjs() {
		pInstance = this;
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
		if (pInstance) pInstance->mObjs.insert(pObj);
		return pObj;
	}
}

#endif // !CCKIT_GL_FACTORY_H