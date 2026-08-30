#pragma once

#include <Core/Core.h>
#include <memory>

namespace Craft
{
	// 커스텀 타입 시스템을 제공하는 최상위 클래스(단일 상속만 지원)
	class CRAFT_API CraftObject
	{
	public:
		virtual ~CraftObject() = default;

		virtual size_t GetType() const = 0;	// 현재 객체 타입의 ID반환
		virtual bool Is(size_t id) const	// 전달된 다른 타입 ID와 현재 객체(부모포함)의 타입 비교
		{
			return false;	// 최상위 객체인 CraftObject까지 올라온건 타입이 다르다는 의미
		}

		template<typename T>
		bool IsTypeOf() const		// 타입 질문 함수
		{
			return Is(T::TypeId());	// 이 함수를 사용하기위해선 T타입이 static TypeId 함수를 가져야함
		}
	};

	template<typename T, typename U>
	std::shared_ptr<T> Cast(const std::shared_ptr<U>& object)	// 형변환 함수 - 스마트 포인터 간의 변환
	{
		if (!object)
			return nullptr;

		if (object->Is(T::TypeId()))	// object가 T타입인지 확인
			return std::static_pointer_cast<T>(object);	// static_pointer_cast는 런타임 타입검증x. 상대적으로 가벼움

		return nullptr;
	}
}

// 타입 시스템 사용하는 클래스에 배치할 매크로
#define TYPE_DECLARATIONS(Type, ParentType)							\
	using super = ParentType;										\
protected:															\
	static size_t TypeIdClass()										\
	{																\
		static int runTimeTypeId = 0;								\
		return reinterpret_cast<size_t>(&runTimeTypeId);			\
	}																\
public:																\
	static size_t TypeId()											\
	{																\
		return Type::TypeIdClass();									\
	}																\
	virtual size_t GetType() const override							\
	{																\
		return Type::TypeIdClass();									\
	}																\
	virtual bool Is(size_t id) const override						\
	{																\
		return (id == TypeIdClass()) ? true : ParentType::Is(id);	\
	}