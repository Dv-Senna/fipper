#pragma once

#include <array>
#include <string_view>
#include <tuple>

#include "fp/utils/generated/macroVarToSeq.hpp"


#define FP_EMPTY
#define FP_NULL(...)
#define FP_IDENTITY(...) __VA_ARGS__
#define FP_RPARENTHESIS() )
#define FP_COMMA(...) ,
#define FP_PARENTHESIS_REMOVER(...) FP_IDENTITY __VA_ARGS__
#define FP_STATE(x, ...) __VA_ARGS__
#define FP_STRINGIFY(...) FP_STRINGIFY_(__VA_ARGS__)
#define FP_STRINGIFY_(...) #__VA_ARGS__
#define FP_CONCAT(...) FP_CONCAT_(__VA_ARGS__)
#define FP_CONCAT_(a, b) a##b

/*
 * Convert a sequence to an improved guide
 * A sequence is defined as following :
 *     (val1)(val2)(val3)
 * An improved guide is :
 *     )val1) )val2) )val3) FP_IMPROVED_GUIDE_END_MARK)
 *
 * It allows better iteration capabilities by allowing context passing
 * This improvement to guide is inspired by https://github.com/HolyBlackCat/macro_sequence_for
 * */
#define FP_SEQ_TO_IMPROVED_GUIDE(seq) FP_SEQ_TO_IMPROVED_GUIDE_END(FP_SEQ_TO_IMPROVED_GUIDE_A seq) FP_IMPROVED_GUIDE_END_MARK)
#define FP_SEQ_TO_IMPROVED_GUIDE_A(...) FP_RPARENTHESIS FP_EMPTY() __VA_ARGS__ FP_RPARENTHESIS FP_EMPTY() FP_SEQ_TO_IMPROVED_GUIDE_B
#define FP_SEQ_TO_IMPROVED_GUIDE_B(...) FP_RPARENTHESIS FP_EMPTY() __VA_ARGS__ FP_RPARENTHESIS FP_EMPTY() FP_SEQ_TO_IMPROVED_GUIDE_A
#define FP_SEQ_TO_IMPROVED_GUIDE_A_END
#define FP_SEQ_TO_IMPROVED_GUIDE_B_END
#define FP_SEQ_TO_IMPROVED_GUIDE_END(...) FP_SEQ_TO_IMPROVED_GUIDE_END_(__VA_ARGS__)
#define FP_SEQ_TO_IMPROVED_GUIDE_END_(...) __VA_ARGS__##_END


/*
 * @brief Iterate over a sequence
 * @param body(ctx, step_ctx, value) The body of the loop
 * @param step(ctx, step_ctx) The step statement
 * @param final(ctx, step_ctx) A statement to execute at the end, with the final step_ctx
 * @param separator(ctx, step_ctx) A value that is inserted between each instance of body, but not before and after
 * @param ctx A constant value passed to the macros
 * @param start The start value of step_ctx
 * @param seq The sequence to iterate upon
 *
 * This macro is equivalent to the following pseudo-code :
 * ```
 *    expr := 
 *    step_ctx := start
 *    for (element in seq)
 *        expr = expr body(ctx, step_ctx, element)
 *        if element is not last
 *            expr = expr separator(ctx, step_ctx)
 *        step_ctx = step(ctx, step_ctx)
 *	  expr = expr final(ctx, step_ctx)
 *	  return expr
 * ```
 *
 * This implementation is inspired by https://github.com/HolyBlackCat/macro_sequence_for
 * */
#define FP_SEQ_FOREACH(body, step, final, separator, ctx, start, seq) FP_IMPROVED_GUIDE_FOREACH(body, step, final, separator, ctx, start, FP_SEQ_TO_IMPROVED_GUIDE(seq))


#define FP_IMPROVED_GUIDE_FOREACH(body, step, final, separator, ctx, start, ...) FP_IMPROVED_GUIDE_FOREACH_FIRST(body, step, final, separator, ctx, start, __VA_ARGS__

#define FP_IMPROVED_GUIDE_FOREACH_FIRST(body, step, final, separator, ctx, step_ctx, ...) FP_IMPROVED_GUIDE_FOREACH_FIRST_##__VA_ARGS__(body, step, final, separator, ctx, step_ctx)
#define FP_IMPROVED_GUIDE_FOREACH_FIRST_(body, step, final, separator, ctx, step_ctx, ...) FP_IMPROVED_GUIDE_FOREACH_FIRST_B(body, step, final, separator, ctx, step_ctx,
#define FP_IMPROVED_GUIDE_FOREACH_FIRST_FP_IMPROVED_GUIDE_END_MARK(body, step, final, separator, ctx, step_ctx, ...) final(ctx, step_ctx)
#define FP_IMPROVED_GUIDE_FOREACH_FIRST_B(body, step, final, separator, ctx, step_ctx, ...) body(ctx, step_ctx, __VA_ARGS__)\
	FP_IMPROVED_GUIDE_FOREACH_A(body, step, final, separator, ctx, step(ctx, step_ctx), step_ctx,

#define FP_IMPROVED_GUIDE_FOREACH_A(body, step, final, separator, ctx, step_ctx, old_step_ctx, ...)\
	FP_IMPROVED_GUIDE_FOREACH_A_##__VA_ARGS__(body, step, final, separator, ctx, step_ctx, old_step)
#define FP_IMPROVED_GUIDE_FOREACH_A_(body, step, final, separator, ctx, step_ctx, old_step_ctx) FP_IMPROVED_GUIDE_FOREACH_B(body, step, final, separator, ctx, step_ctx, old_step_ctx,
#define FP_IMPROVED_GUIDE_FOREACH_A_FP_IMPROVED_GUIDE_END_MARK(body, step, final, separator, ctx, step_ctx, old_step_ctx) final(ctx, step_ctx)
#define FP_IMPROVED_GUIDE_FOREACH_B(body, step, final, separator, ctx, step_ctx, old_step_ctx, ...) separator(ctx, old_step_ctx)\
	body(ctx, step_ctx, __VA_ARGS__)\
	FP_IMPROVED_GUIDE_FOREACH_A(body, step, final, separator, ctx, step(ctx, step_ctx), step_ctx,


#define FP_SEQ_FOREACH_BODY(body, ctx, seq) FP_SEQ_FOREACH(body, FP_NULL, FP_NULL, FP_NULL, ctx,, seq)
#define FP_SEQ_FOREACH_BODY_WITH_SEP(body, separator, ctx, seq) FP_SEQ_FOREACH(body, FP_NULL, FP_NULL, separator, ctx,, seq)
#define FP_SEQ_FOREACH_STEP(step, final, ctx, start, seq) FP_SEQ_FOREACH(FP_NULL, step, final, FP_NULL, ctx, start, seq)
#define FP_SEQ_FOREACH_STEP_WITH_SEP(step, final, separator, ctx, start, seq) FP_SEQ_FOREACH(FP_NULL, step, final, separator, ctx, start, seq)


/*
 * @brief Create a structure with reflection data, so it can auto-generate json translation and serialization
 * @param name The name of the struct
 * @... The members of the struct
 *
 * Example:
 *     To create the following struct :
 *     ```cpp
 *     struct Foo {
 *         int a;
 *         char b;
 *     };
 *     ```
 *     Use the following instantiation :
 *     ```cpp
 *     FP_REFLECTED_STRUCT(Foo,
 *         ((int) a)
 *         ((char) b)
 *     );
 *     ```
 * 
 * @warning This only support C-like structure
 * */
#define FP_REFLECTED_STRUCT(name, ...) FP_REFLECTED_STRUCT_SEQ(name, FP_VAR_TO_SEQ(__VA_ARGS__))

#define FP_REFLECTED_STRUCT_SEQ(name, members) struct name {\
	FP_SEQ_FOREACH_BODY(FP_REFLECTED_STRUCT_CREATE_MEMBER,, members)\
\
	constexpr static bool HAS_REFLECTION {true};\
	constexpr static std::size_t MEMBERS_COUNT {FP_SEQ_FOREACH_STEP(FP_REFLECTED_STRUCT_COUNT_MEMBERS_STEP, FP_STATE,, 0, members)};\
	constexpr static std::tuple<FP_SEQ_FOREACH_BODY_WITH_SEP(FP_REFLECTED_STRUCT_MEMBER_POINTER_TYPE, FP_COMMA, name, members)> MEMBERS_PTRS {\
		FP_SEQ_FOREACH_BODY_WITH_SEP(FP_REFLECTED_STRUCT_MEMBER_POINTER, FP_COMMA, name, members)\
	};\
	constexpr static std::array<std::string_view, MEMBERS_COUNT> MEMBERS_NAMES {\
		FP_SEQ_FOREACH_BODY_WITH_SEP(FP_REFLECTED_STRUCT_MEMBER_NAME, FP_COMMA,, members)\
	};\
	using MembersTypes = std::tuple<FP_SEQ_FOREACH_BODY_WITH_SEP(FP_REFLECTED_STRUCT_MEMBER_TYPE, FP_COMMA,, members)>;\
}

#define FP_REFLECTED_STRUCT_EXTRACT_TYPE(expr) FP_PARENTHESIS_REMOVER(FP_REFLECTED_STRUCT_EXTRACT_TYPE_AT_0_INTERFACE(FP_REFLECTED_STRUCT_EXTRACT_TYPE_COMMA_INSERTER expr))
#define FP_REFLECTED_STRUCT_EXTRACT_TYPE_COMMA_INSERTER(...) (__VA_ARGS__),
#define FP_REFLECTED_STRUCT_EXTRACT_TYPE_AT_0_INTERFACE(...) FP_REFLECTED_STRUCT_EXTRACT_TYPE_AT_0(__VA_ARGS__)
#define FP_REFLECTED_STRUCT_EXTRACT_TYPE_AT_0(x, ...) x

#define FP_REFLECTED_STRUCT_EXTRACT_VAR(expr) FP_NULL expr

#define FP_REFLECTED_STRUCT_COUNT_MEMBERS_STEP(ctx, step_ctx) step_ctx+1
#define FP_REFLECTED_STRUCT_MEMBER_POINTER_TYPE(ctx, step_ctx, ...) FP_REFLECTED_STRUCT_EXTRACT_TYPE(__VA_ARGS__) ctx::*
#define FP_REFLECTED_STRUCT_MEMBER_POINTER(ctx, step_ctx, ...) &ctx::FP_REFLECTED_STRUCT_EXTRACT_VAR(__VA_ARGS__)
#define FP_REFLECTED_STRUCT_MEMBER_NAME(ctx, step_ctx, ...) FP_STRINGIFY(FP_REFLECTED_STRUCT_EXTRACT_VAR(__VA_ARGS__))
#define FP_REFLECTED_STRUCT_MEMBER_TYPE(ctx, step_ctx, ...) FP_REFLECTED_STRUCT_EXTRACT_TYPE(__VA_ARGS__)
#define FP_REFLECTED_STRUCT_CREATE_MEMBER(ctx, step_ctx, ...) FP_IDENTITY __VA_ARGS__;






