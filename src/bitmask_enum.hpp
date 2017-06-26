#pragma once

// https://www.justsoftwaresolutions.co.uk/cplusplus/using-enum-classes-as-bitfields.html

template<typename E>
struct enable_bitmask_operators{
	static constexpr bool enable=false;
};

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable,E>::type
operator|(E lhs,E rhs){
	typedef typename std::underlying_type<E>::type underlying;
	return static_cast<E>(
		static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable,E>::type
operator&(E lhs,E rhs){
	typedef typename std::underlying_type<E>::type underlying;
	return static_cast<E>(
		static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable,bool>::type
operator!=(E lhs,typename std::underlying_type<E>::type rhs){
	typedef typename std::underlying_type<E>::type underlying;
	return static_cast<bool>(
		static_cast<underlying>(lhs) != rhs);
}

#define bitmask_enum(E) \
template<> \
struct enable_bitmask_operators<E> \
{ \
	static constexpr bool enable=true; \
};
