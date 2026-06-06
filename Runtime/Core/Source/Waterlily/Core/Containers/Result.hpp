#pragma once

#include <functional>
#include <type_traits>
#include <utility>
#include <variant>

namespace Wl
{

    template<typename T>
    class Success;

    template<typename T>
    class Failure;

    template<typename SuccessT, typename FailureT>
    class Result;

    struct FailureType
    {
        explicit FailureType() = default;
    };

    namespace internal
    {

        static constexpr FailureType failure_internal{};

        template<typename T, typename U>
        constexpr bool operator==(const Success<T>& lhs, const Success<U>& rhs) noexcept
        {
            if constexpr (!std::is_same_v<T, U>)
            {
                return false;
            }
            else
            {
                return lhs.value() == rhs.Value();
            }
        }

        template<typename T, typename U>
        constexpr bool operator!=(const Success<T>& lhs, const Success<U>& rhs) noexcept
        {
            return !(lhs == rhs);
        }

        template<typename Exp, typename F>
        constexpr auto AndThen(Exp&& exp, F&& f) noexcept
        {
            using T = std::remove_cvref_t<decltype(exp.value())>;
            using E = std::remove_cvref_t<decltype(exp.error())>;

            auto invoke_f = [&]() -> decltype(auto)
            {
                if constexpr (!std::is_void_v<T>)
                {
                    return std::invoke(std::forward<F>(f), std::forward<Exp>(exp).value());
                }
                else
                {
                    return std::invoke(std::forward<F>(f));
                }
            };

            using R = decltype(invoke_f());

            return exp.has_value() ? invoke_f() : R(failure_internal, std::forward<Exp>(exp).error());
        }

        template<typename Exp, typename F>
        constexpr auto OrElse(Exp&& exp, F&& f) noexcept
        {
            using T = std::remove_cvref_t<decltype(exp.value())>;
            using G = std::invoke_result_t<F, decltype(std::forward<Exp>(exp).error())>;

            if (!exp.has_value())
            {
                return std::invoke(std::forward<F>(f), std::forward<Exp>(exp).error());
            }

            if constexpr (!std::is_void_v<T>)
            {
                return G(std::in_place, std::forward<Exp>(exp).value());
            }
            else
            {
                return G();
            }
        }

        template<typename Exp, typename F>
        constexpr auto Map(Exp&& exp, F&& f) noexcept
        {
            using T = std::remove_cvref_t<decltype(exp.value())>;
            using E = std::remove_cvref_t<decltype(exp.Error())>;

            auto invoke_f = [&]() -> decltype(auto)
            {
                if constexpr (!std::is_void_v<T>)
                {
                    return std::invoke(std::forward<F>(f), std::forward<Exp>(exp).value());
                }
                else
                {
                    return std::invoke(std::forward<F>(f));
                }
            };

            using U = std::remove_cv_t<decltype(invoke_f())>;

            if (!exp.has_value())
            {
                return Result<U, E>(failure_internal, std::forward<Exp>(exp).error());
            }

            if constexpr (!std::is_void_v<U>)
            {
                return Result<U, E>(std::in_place, invoke_f());
            }
            else
            {
                invoke_f();
                return Result<U, E>();
            }
        }

        template<typename Exp, typename F>
        constexpr auto MapError(Exp&& exp, F&& f) noexcept
        {
            using T = std::remove_cvref_t<decltype(exp.value())>;
            using G = std::remove_cv_t<std::invoke_result_t<F, decltype(std::forward<Exp>(exp).error())>>;

            if (!exp.has_value())
            {
                return Result<T, G>(failure_internal, std::invoke(std::forward<F>(f), std::forward<Exp>(exp).error()));
            }

            if constexpr (std::is_void_v<T>)
            {
                return Result<T, G>();
            }
            else
            {
                return Result<T, G>(std::in_place, std::forward<Exp>(exp).value());
            }
        }
    }// namespace internal

    template<typename T>
    class Success final
    {
    public:
        template<typename U = T>
            requires(!std::is_same_v<std::remove_cvref_t<U>, Success<T>>) &&
                    (!std::is_same_v<std::remove_cvref_t<U>, std::in_place_t>) && std::is_constructible_v<T, U>
        constexpr explicit Success(U&& val) noexcept
            : m_value(std::forward<U>(val))
        {
        }

        template<typename... Args>
        constexpr explicit Success(std::in_place_t, Args&&... args) noexcept
            : m_value(std::forward<Args>(args)...)
        {
        }

        template<typename U, typename... Args>
        constexpr explicit Success(std::in_place_t, std::initializer_list<U> il, Args&&... args) noexcept
            : m_value(il, std::forward<Args>(args)...)
        {
        }

        constexpr T& GetValue() & noexcept
        {
            return m_value;
        }

        constexpr const T& GetValue() const& noexcept
        {
            return m_value;
        }

        constexpr T&& GetValue() && noexcept
        {
            return std::move(GetValue());
        }

        constexpr const T&& GetValue() const&& noexcept
        {
            return std::move(GetValue());
        }

        constexpr void Swap(Success& other) noexcept
        {
            std::swap(GetValue(), other.GetValue());
        }

        friend constexpr void Swap(Success& x, Success& y) noexcept
        {
            x.Swap(y);
        }

    private:
        T m_value;
    };

    template<typename T>
        requires(std::is_void_v<T>)
    class Success<T> final
    {
    public:
        constexpr explicit Success() noexcept = default;
    };

    template<typename E>
    class Failure final
    {
    public:
        template<typename Err = E>
            requires(!std::is_same_v<std::remove_cvref_t<Err>, Failure<E>>) &&
                    (!std::is_same_v<std::remove_cvref_t<Err>, std::in_place_t>) && std::is_constructible_v<E, Err>
        constexpr explicit Failure(Err&& err) noexcept
            : m_error(std::forward<Err>(err))
        {
        }

        template<typename... Args>
        constexpr explicit Failure(std::in_place_t, Args&&... args) noexcept
            : m_error(std::forward<Args>(args)...)
        {
        }

        template<typename U, typename... Args>
        constexpr explicit Failure(std::in_place_t, std::initializer_list<U> il, Args&&... args) noexcept
            : m_error(il, std::forward<Args>(args)...)
        {
        }

        constexpr E& GetError() & noexcept
        {
            return m_error;
        }

        constexpr const E& GetError() const& noexcept
        {
            return m_error;
        }

        constexpr E&& GetError() && noexcept
        {
            return std::move(GetError());
        }

        constexpr const E&& GetError() const&& noexcept
        {
            return std::move(GetError());
        }

        constexpr void Swap(Failure& other) noexcept
        {
            std::swap(GetError(), other.GetError());
        }

        friend constexpr void Swap(Failure& x, Failure& y) noexcept
        {
            x.Swap(y);
        }

    private:
        E m_error;
    };

    template<typename T, typename E>
    class Result
    {
    public:
        static inline constexpr size_t s_ValIdx = 1;
        static inline constexpr size_t s_ErrIdx = 2;
        static inline constexpr std::in_place_index_t<1> s_ValTag{};
        static inline constexpr std::in_place_index_t<2> s_ErrTag{};

        template<typename U, typename G>
        friend class Result;

        constexpr static Result<T, E> Success(const T& value)
        {
            return Result<T, E>(s_ValTag, value);
        }

        constexpr static Result<T, E> Failure(const E& error)
        {
            return Result<T, E>(s_ErrTag, error);
        }

        constexpr Result() noexcept
            requires(std::default_initializable<T>)
            : m_data(s_ValTag)
        {
        }

        constexpr Result(const Result& rhs) noexcept
            : m_data(rhs.m_data)
        {
        }

        constexpr Result(Result&& rhs) noexcept
            : m_data(std::move(rhs.m_data))
        {
            rhs.SetIsMovedFrom();
        }

        template<typename U, typename G>
        constexpr explicit Result(const Result<U, G>& rhs) noexcept
        {
            if (rhs.HasValue())
            {
                EmplaceValue(rhs.value());
            }
            else
            {
                EmplaceError(rhs.error());
            }
        }

        template<typename U, typename G>
        constexpr explicit Result(Result<U, G>&& rhs) noexcept
        {
            if (rhs.HasValue())
            {
                EmplaceValue(std::move(rhs.value()));
            }
            else
            {
                EmplaceError(std::move(rhs.GetError()));
            }
            rhs.SetIsMovedFrom();
        }

        template<typename... Args>
        constexpr explicit Result(decltype(s_ValTag), Args&&... args) noexcept
            : m_data(s_ValTag, std::forward<Args>(args)...)
        {
        }

        template<typename U, typename... Args>
        constexpr explicit Result(decltype(s_ValTag), std::initializer_list<U> il, Args&&... args) noexcept
            : m_data(s_ValTag, il, std::forward<Args>(args)...)
        {
        }

        template<typename... Args>
        constexpr explicit Result(decltype(s_ErrTag), Args&&... args) noexcept
            : m_data(s_ErrTag, std::forward<Args>(args)...)
        {
        }

        template<typename U, typename... Args>
        constexpr explicit Result(decltype(s_ErrTag), std::initializer_list<U> il, Args&&... args) noexcept
            : m_data(s_ErrTag, il, std::forward<Args>(args)...)
        {
        }

        constexpr Result& operator=(const Result& rhs) noexcept
        {
            m_data = rhs.m_data;
            return *this;
        }

        constexpr Result& operator=(Result&& rhs) noexcept
        {
            m_data = std::move(rhs.m_data);
            rhs.SetIsMovedFrom();
            return *this;
        }

        template<typename... Args>
        constexpr T& EmplaceValue(Args&&... args) noexcept
        {
            return m_data.template emplace<s_ValIdx>(std::forward<Args>(args)...);
        }

        template<typename U, typename... Args>
        constexpr T& EmplaceValue(std::initializer_list<U> il, Args&&... args) noexcept
        {
            return m_data.template emplace<s_ValIdx>(il, std::forward<Args>(args)...);
        }

        template<typename... Args>
        constexpr E& EmplaceError(Args&&... args) noexcept
        {
            return m_data.template emplace<s_ErrIdx>(std::forward<Args>(args)...);
        }

        template<typename U, typename... Args>
        constexpr E& EmplaceError(std::initializer_list<U> il, Args&&... args) noexcept
        {
            return m_data.template emplace<s_ErrIdx>(il, std::forward<Args>(args)...);
        }

        void Swap(Result& rhs) noexcept
        {
            m_data.swap(rhs.m_data);
        }

        constexpr bool HasValue() const noexcept
        {
            return m_data.index() == s_ValIdx;
        }

        constexpr T& GetValue() noexcept
        {
            return std::get<s_ValIdx>(m_data);
        }

        constexpr const T& GetValue() const noexcept
        {
            return std::get<s_ValIdx>(m_data);
        }

        constexpr E& GetError() noexcept
        {
            return std::get<s_ErrIdx>(m_data);
        }

        constexpr const E& GetError() const noexcept
        {
            return std::get<s_ErrIdx>(m_data);
        }

        template<typename F>
            requires(std::copy_constructible<E>)
        constexpr auto AndThen(F&& f) & noexcept
        {
            return internal::AndThen(*this, std::forward<F>(f));
        }

        template<typename F>
            requires(std::copy_constructible<E>)
        constexpr auto AndThen(F&& f) const& noexcept
        {
            return internal::AndThen(*this, std::forward<F>(f));
        }

        template<typename F>
            requires(std::move_constructible<E>)
        constexpr auto AndThen(F&& f) && noexcept
        {
            return internal::AndThen(std::move(*this), std::forward<F>(f));
        }

        template<typename F>
            requires(std::move_constructible<E>)
        constexpr auto AndThen(F&& f) const&& noexcept
        {
            return internal::AndThen(std::move(*this), std::forward<F>(f));
        }

        template<typename F>
        constexpr auto OrElse(F&& f) & noexcept
        {
            return internal::OrElse(*this, std::forward<F>(f));
        }

        template<typename F>
        constexpr auto OrElse(F&& f) const& noexcept
        {
            return internal::OrElse(*this, std::forward<F>(f));
        }

        template<typename F>
        constexpr auto OrElse(F&& f) && noexcept
        {
            return internal::OrElse(std::move(*this), std::forward<F>(f));
        }

        template<typename F>
        constexpr auto OrElse(F&& f) const&& noexcept
        {
            return internal::OrElse(std::move(*this), std::forward<F>(f));
        }

        template<typename F>
            requires(std::copy_constructible<E>)
        constexpr auto Map(F&& f) & noexcept
        {
            return internal::Map(*this, std::forward<F>(f));
        }

        template<typename F>
            requires(std::copy_constructible<E>)
        constexpr auto Map(F&& f) const& noexcept
        {
            return internal::Map(*this, std::forward<F>(f));
        }

        template<typename F>
            requires(std::move_constructible<E>)
        constexpr auto Map(F&& f) && noexcept
        {
            return internal::Map(std::move(*this), std::forward<F>(f));
        }

        template<typename F>
            requires(std::move_constructible<E>)
        constexpr auto Map(F&& f) const&& noexcept
        {
            return internal::Map(std::move(*this), std::forward<F>(f));
        }

        template<typename F>
        constexpr auto MapError(F&& f) & noexcept
        {
            return internal::MapError(*this, std::forward<F>(f));
        }

        template<typename F>
        constexpr auto MapError(F&& f) const& noexcept
        {
            return internal::MapError(*this, std::forward<F>(f));
        }

        template<typename F>
        constexpr auto MapError(F&& f) && noexcept
        {
            return internal::MapError(std::move(*this), std::forward<F>(f));
        }

        template<typename F>
        constexpr auto MapError(F&& f) const&& noexcept
        {
            return internal::MapError(std::move(*this), std::forward<F>(f));
        }

    private:
        static constexpr size_t kNulIdx = 0;
        static_assert(kNulIdx != s_ValIdx);
        static_assert(kNulIdx != s_ErrIdx);

        constexpr bool IsMovedFrom() const noexcept
        {
            return m_data.index() == kNulIdx;
        }

        constexpr void SetIsMovedFrom() noexcept
        {
            m_data.template emplace<kNulIdx>();
        }

        std::variant<std::monostate, T, E> m_data;
    };

}// namespace Wl