#pragma once

#include "Common.h"		// for DEATH_HELPER_EXPAND/DEATH_HELPER_PICK/DEATH_REMOVE_PARENS

#if !defined(DEATH_NO_ASSERT) && (!defined(DEATH_ASSERT) || !defined(DEATH_DEBUG_ASSERT) || !defined(DEATH_CONSTEXPR_ASSERT) || !defined(DEATH_DEBUG_CONSTEXPR_ASSERT) || !defined(DEATH_ASSERT_UNREACHABLE))
#	if defined(DEATH_STANDARD_ASSERT)
#		include <cassert>
#	elif defined(DEATH_TRACE)
#		include <cstdlib>
#	endif
#endif

// Event Tracing
#if defined(DEATH_TRACE)

// If DEATH_TRACE symbol is #defined with no value, supply internal function name
#	if DEATH_PASTE(DEATH_TRACE, 1) == 1 || DEATH_PASTE(DEATH_TRACE, 1) == 11
#		undef DEATH_TRACE
#		define DEATH_TRACE __WriteTrace
#	endif

enum class TraceLevel {
	Unknown,
	Debug,
	Info,
	Warning,
	Error,
	Assert,
	Fatal
};

/** @brief This functions needs to be provided by the target application to enable logging */
void DEATH_TRACE(TraceLevel level, const char* fmt, ...);

#	if defined(DEATH_TARGET_GCC) || defined(DEATH_TARGET_CLANG)
#		define __DEATH_CURRENT_FUNCTION __PRETTY_FUNCTION__
#	elif defined(DEATH_TARGET_MSVC)
#		define __DEATH_CURRENT_FUNCTION __FUNCTION__ "()"
#	else
#		define __DEATH_CURRENT_FUNCTION __func__
#	endif

#	if defined(DEATH_DEBUG)
#		define LOGD(fmt, ...) DEATH_TRACE(TraceLevel::Debug, "%s ‡ " fmt, __DEATH_CURRENT_FUNCTION, ##__VA_ARGS__)
#	else
#		define LOGD(fmt, ...) do {} while (false)
#	endif
#	define LOGI(fmt, ...) DEATH_TRACE(TraceLevel::Info, "%s ‡ " fmt, __DEATH_CURRENT_FUNCTION, ##__VA_ARGS__)
#	define LOGW(fmt, ...) DEATH_TRACE(TraceLevel::Warning, "%s ‡ " fmt, __DEATH_CURRENT_FUNCTION, ##__VA_ARGS__)
#	define LOGE(fmt, ...) DEATH_TRACE(TraceLevel::Error, "%s ‡ " fmt, __DEATH_CURRENT_FUNCTION, ##__VA_ARGS__)
#	define LOGF(fmt, ...) DEATH_TRACE(TraceLevel::Fatal, "%s ‡ " fmt, __DEATH_CURRENT_FUNCTION, ##__VA_ARGS__)
#else
#	define LOGD(fmt, ...) do {} while (false)
#	define LOGI(fmt, ...) do {} while (false)
#	define LOGW(fmt, ...) do {} while (false)
#	define LOGE(fmt, ...) do {} while (false)
#	define LOGF(fmt, ...) do {} while (false)
#endif

#if !defined(DEATH_ASSERT_BREAK)
#	if !defined(DEATH_DEBUG)
#		define DEATH_ASSERT_BREAK() do {} while (false)
#	elif defined(DEATH_TARGET_WINDOWS)
#		define DEATH_ASSERT_BREAK() __debugbreak()
#	else
#		if defined(__has_builtin)
#			if __has_builtin(__builtin_debugtrap)
#				define DEATH_ASSERT_BREAK() __builtin_debugtrap()
#			elif __has_builtin(__builtin_trap)
#				define DEATH_ASSERT_BREAK() __builtin_trap()
#			endif
#		endif
#		if !defined(DEATH_ASSERT_BREAK)
#			if defined(DEATH_TARGET_GCC) && (defined(__i386__) || defined(__x86_64__))
#				define DEATH_ASSERT_BREAK() __asm__ volatile("int3;nop")
#			elif defined(DEATH_TARGET_GCC) && defined(__thumb__)
#				define DEATH_ASSERT_BREAK() __asm__ volatile(".inst 0xde01")
#			elif defined(DEATH_TARGET_GCC) && defined(__arm__) && !defined(__thumb__)
#				define DEATH_ASSERT_BREAK() __asm__ volatile(".inst 0xe7f001f0")
#			else
#				define DEATH_ASSERT_BREAK() ::abort()
#			endif
#		endif
#	endif
#endif

// Assertions
#if !defined(DEATH_NO_ASSERT) && !defined(DEATH_STANDARD_ASSERT) && defined(DEATH_TRACE)
#	define __DEATH_ASSERT_BASE(fmt, ...) DEATH_TRACE(TraceLevel::Assert, "%s ‡ " fmt, __DEATH_CURRENT_FUNCTION, ##__VA_ARGS__)
#	define __DEATH_ASSERT_TRACE(...) DEATH_HELPER_EXPAND(__DEATH_ASSERT_BASE(__VA_ARGS__))
#endif

/** @brief Assertion macro */
#if !defined(DEATH_ASSERT)
#	if defined(DEATH_NO_ASSERT) || (!defined(DEATH_STANDARD_ASSERT) && !defined(DEATH_TRACE))
#		define DEATH_ASSERT(condition, message, returnValue) do {} while (false)
#	elif defined(DEATH_STANDARD_ASSERT)
#		define DEATH_ASSERT(condition, message, returnValue) assert(condition)
#	else
#		define DEATH_ASSERT(condition, message, returnValue)			\
			do {														\
				if DEATH_UNLIKELY(!(condition)) {						\
					__DEATH_ASSERT_TRACE(DEATH_REMOVE_PARENS(message));	\
					DEATH_ASSERT_BREAK();								\
					return returnValue;									\
				}														\
			} while(false)
#	endif
#endif

/** @brief Debug-only assertion macro */
#if !defined(DEATH_DEBUG_ASSERT)
#	if defined(DEATH_NO_ASSERT) || !defined(DEATH_DEBUG) || (!defined(DEATH_STANDARD_ASSERT) && !defined(DEATH_TRACE))
#		define DEATH_DEBUG_ASSERT(condition, ...) do {} while (false)
#	elif defined(DEATH_STANDARD_ASSERT)
#		define DEATH_DEBUG_ASSERT(condition, ...) assert(condition)
#	else
#		define __DEATH_DEBUG_ASSERT1(condition)							\
			do {														\
				if DEATH_UNLIKELY(!(condition)) {						\
					__DEATH_ASSERT_TRACE("Assertion (%s) failed at \"%s:%i\"", #condition, __FILE__, __LINE__);	\
					DEATH_ASSERT_BREAK();								\
				}														\
			} while(false)
#		define __DEATH_DEBUG_ASSERT3(condition, message, returnValue)	\
			do {														\
				if DEATH_UNLIKELY(!(condition)) {						\
					__DEATH_ASSERT_TRACE(DEATH_REMOVE_PARENS(message));	\
					DEATH_ASSERT_BREAK();								\
					return returnValue;									\
				}														\
			} while(false)
#		define DEATH_DEBUG_ASSERT(...) DEATH_HELPER_EXPAND(DEATH_HELPER_PICK(__VA_ARGS__, __DEATH_DEBUG_ASSERT3, __DEATH_DEBUG_ASSERT3, __DEATH_DEBUG_ASSERT3, __DEATH_DEBUG_ASSERT3, __DEATH_DEBUG_ASSERT3, __DEATH_DEBUG_ASSERT3, __DEATH_DEBUG_ASSERT1, __DEATH_DEBUG_ASSERT1, )(__VA_ARGS__))
#	endif
#endif

/** @brief Constexpr assertion macro */
#if !defined(DEATH_CONSTEXPR_ASSERT)
#	if defined(DEATH_NO_ASSERT) || (!defined(DEATH_STANDARD_ASSERT) && !defined(DEATH_TRACE))
#		define DEATH_CONSTEXPR_ASSERT(condition, message) static_cast<void>(0)
#	elif defined(DEATH_STANDARD_ASSERT)
#		define DEATH_CONSTEXPR_ASSERT(condition, message)				\
			static_cast<void>((condition) ? 0 : ([&]() {				\
				assert(!#condition);									\
			}(), 0))
#	else
#		define DEATH_CONSTEXPR_ASSERT(condition, message)				\
			static_cast<void>((condition) ? 0 : ([&]() {				\
				__DEATH_ASSERT_TRACE(DEATH_REMOVE_PARENS(message));		\
				DEATH_ASSERT_BREAK();									\
			}(), 0))
#	endif
#endif

/** @brief Debug-only constexpr assertion macro */
#if !defined(DEATH_DEBUG_CONSTEXPR_ASSERT)
#	if defined(DEATH_DEBUG)
#		define DEATH_DEBUG_CONSTEXPR_ASSERT(condition, message) DEATH_CONSTEXPR_ASSERT(condition, message)
#	else
#		define DEATH_DEBUG_CONSTEXPR_ASSERT(condition, message) static_cast<void>(0)
#	endif
#endif

/** @brief Assert that the code is unreachable */
#if !defined(DEATH_ASSERT_UNREACHABLE)
#	if defined(DEATH_NO_ASSERT) || (!defined(DEATH_STANDARD_ASSERT) && !defined(DEATH_TRACE))
#		if defined(DEATH_TARGET_GCC)
#			define DEATH_ASSERT_UNREACHABLE() __builtin_unreachable()
#		elif defined(DEATH_TARGET_MSVC)
#			define DEATH_ASSERT_UNREACHABLE() __assume(0)
#		else
#			define DEATH_ASSERT_UNREACHABLE() std::abort()
#		endif
#	elif defined(DEATH_STANDARD_ASSERT)
#		define DEATH_ASSERT_UNREACHABLE() assert(!"Unreachable code")
#	else
#		define DEATH_ASSERT_UNREACHABLE()								\
			do {														\
				__DEATH_ASSERT_TRACE("Reached unreachable code at \"%s:%i\"", __FILE__, __LINE__);	\
				DEATH_ASSERT_BREAK();									\
			} while (false)
#	endif
#endif
