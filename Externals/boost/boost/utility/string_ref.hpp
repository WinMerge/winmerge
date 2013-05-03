/* 
   Copyright (c) Marshall Clow 2012-2012.

   Distributed under the Boost Software License, Version 1.0. (See accompanying
   file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    For more information, see http://www.boost.org
    
    Based on the StringRef implementation in LLVM (http://llvm.org) and
    N3422 by Jeffrey Yasskin
        http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3442.html

*/

#ifndef BOOST_STRING_REF_HPP
#define BOOST_STRING_REF_HPP

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>

#include <stdexcept>
#include <algorithm>
#include <functional>
#include <string>

namespace boost {
    
    namespace detail {
    //  A helper functor because sometimes we don't have lambdas
        template <typename charT, typename traits>
        class string_ref_traits_eq {
        public:
            string_ref_traits_eq ( charT ch ) : ch_(ch) {}
            bool operator () ( charT val ) const { return traits::eq ( ch_, val ); }
            charT ch_;
            };
        }
    
    template<typename charT, typename traits> class basic_string_ref;
    typedef basic_string_ref<char,     std::char_traits<char> >        string_ref;
    typedef basic_string_ref<wchar_t,  std::char_traits<wchar_t> >    wstring_ref;

#ifndef BOOST_NO_CXX11_CHAR16_T
    typedef basic_string_ref<char16_t, std::char_traits<char16_t> > u16string_ref;
#endif

#ifndef BOOST_NO_CXX11_CHAR32_T
    typedef basic_string_ref<char32_t, std::char_traits<char32_t> > u32string_ref;
#endif
    
    template<typename charT, typename traits>
    class basic_string_ref {
    public:
        // types
        typedef charT value_type;
        typedef const charT* pointer;
        typedef const charT& reference;
        typedef const charT& const_reference;
        typedef pointer const_iterator; // impl-defined
        typedef const_iterator iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef const_reverse_iterator reverse_iterator;
        typedef std::size_t size_type;
        typedef ptrdiff_t difference_type;
        static BOOST_CONSTEXPR_OR_CONST size_type npos = size_type(-1);
        
        // construct/copy
        BOOST_CONSTEXPR basic_string_ref ()
            : ptr_(NULL), len_(0) {}

        BOOST_CONSTEXPR basic_string_ref (const basic_string_ref &rhs)
            : ptr_(rhs.ptr_), len_(rhs.len_) {}

        basic_string_ref& operator=(const basic_string_ref &rhs) {
            ptr_ = rhs.ptr_;
            len_ = rhs.len_;
            return *this;
            }
            
        basic_string_ref(const charT* str)
            : ptr_(str), len_(traits::length(str)) {}

        template<typename Allocator>
        basic_string_ref(const std::basic_string<charT, traits, Allocator>& str)
            : ptr_(str.data()), len_(str.length()) {}

        BOOST_CONSTEXPR basic_string_ref(const charT* str, size_type len)
            : ptr_(str), len_(len) {}

#ifndef BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS
        template<typename Allocator>
        explicit operator std::basic_string<charT, traits, Allocator>() const {
            return std::basic_string<charT, traits, Allocator> ( ptr_, len_ );
            }
#endif
        
        // iterators
        BOOST_CONSTEXPR const_iterator   begin() const { return ptr_; }
        BOOST_CONSTEXPR const_iterator  cbegin() const { return ptr_; }
        BOOST_CONSTEXPR const_iterator     end() const { return ptr_ + len_; }
        BOOST_CONSTEXPR const_iterator    cend() const { return ptr_ + len_; }
                const_reverse_iterator  rbegin() const { return const_reverse_iterator (end()); }
                const_reverse_iterator crbegin() const { return const_reverse_iterator (end()); }
                const_reverse_iterator    rend() const { return const_reverse_iterator (begin()); }
                const_reverse_iterator   crend() const { return const_reverse_iterator (begin()); }
        
        // capacity
        BOOST_CONSTEXPR size_type size()     const { return len_; }
        BOOST_CONSTEXPR size_type length()   const { return len_; }
        BOOST_CONSTEXPR size_type max_size() const { return len_; }
        BOOST_CONSTEXPR bool empty()         const { return len_ == 0; }
        
        // element access
        BOOST_CONSTEXPR const charT& operator[](size_type pos) const { return ptr_[pos]; }

        const charT& at(size_t pos) const {
            if ( pos >= len_ )
                throw std::out_of_range ( "boost::string_ref::at" );
            return ptr_[pos];
            }
            
        BOOST_CONSTEXPR const charT& front() const { return ptr_[0]; }
        BOOST_CONSTEXPR const charT& back()  const { return ptr_[len_-1]; }
        BOOST_CONSTEXPR const charT* data()  const { return ptr_; }
        
        // modifiers
        void clear() { len_ = 0; }
        void remove_prefix(size_type n) {
            if ( n > len_ )
                n = len_;
            ptr_ += n;
            len_ -= n;
            }
            
        void remove_suffix(size_type n) {
            if ( n > len_ )
                n = len_;
            len_ -= n;
            }
            
        
        // basic_string_ref string operations
        BOOST_CONSTEXPR
        basic_string_ref substr(size_type pos, size_type n=npos) const {
#if BOOST_WORKAROUND(BOOST_MSVC, <= 1600)
            // Looks like msvc 8 and 9 have a codegen bug when one branch of
            // a conditional operator is a throw expression. -EAN 2012/12/04
            if ( pos > size()) throw std::out_of_range ( "string_ref::substr" );
            if ( n == npos || pos + n > size()) n = size () - pos;
            return basic_string_ref ( data() + pos, n );
#else
            return pos > size() ? throw std::out_of_range ( "string_ref::substr" ) :
                basic_string_ref ( data() + pos, n == npos || pos + n > size() ? size() - pos : n );
#endif
            }
        
        int compare(basic_string_ref x) const {
            int cmp = traits::compare ( ptr_, x.ptr_, (std::min)(len_, x.len_));
            return cmp != 0 ? cmp : ( len_ == x.len_ ? 0 : len_ < x.len_ ? -1 : 1 );
            }
        
        bool starts_with(charT c) const { return !empty() && traits::eq ( c, front()); }
        bool starts_with(basic_string_ref x) const {
            return len_ >= x.len_ && traits::compare ( ptr_, x.ptr_, x.len_ ) == 0;
            }
        
        bool ends_with(charT c) const { return !empty() && traits::eq ( c, back()); }
        bool ends_with(basic_string_ref x) const {
            return len_ >= x.len_ && traits::compare ( ptr_ + len_ - x.len_, x.ptr_, x.len_ ) == 0;
            }

        size_type find(basic_string_ref s) const {
            const_iterator iter = std::search ( this->cbegin (), this->cend (), 
                                                s.cbegin (), s.cend (), traits::eq );
            return iter = this->cend () ? npos : std::distance ( this->cbegin (), iter );
            }
        
        size_type find(charT c) const {
            const_iterator iter = std::find_if ( this->cbegin (), this->cend (), 
                                    detail::string_ref_traits_eq<charT, traits> ( c ));
            return iter == this->cend () ? npos : std::distance ( this->cbegin (), iter );
            }
                        
        size_type rfind(basic_string_ref s) const {
            const_reverse_iterator iter = std::search ( this->crbegin (), this->crend (), 
                                                s.crbegin (), s.crend (), traits::eq );
            return iter == this->crend () ? npos : reverse_distance ( this->crbegin (), iter );
            }

        size_type rfind(charT c) const {
            const_reverse_iterator iter = std::find_if ( this->crbegin (), this->crend (), 
                                    detail::string_ref_traits_eq<charT, traits> ( c ));
            return iter == this->crend () ? npos : reverse_distance ( this->crbegin (), iter );
            }
        
        size_type find_first_of(charT c) const { return  find (c); }
        size_type find_last_of (charT c) const { return rfind (c); }
        
        size_type find_first_of(basic_string_ref s) const {
            const_iterator iter = std::find_first_of 
            	( this->cbegin (), this->cend (), s.cbegin (), s.cend (), traits::eq );
            return iter == this->cend () ? npos : std::distance ( this->cbegin (), iter );
            }
        
        size_type find_last_of(basic_string_ref s) const {
            const_reverse_iterator iter = std::find_first_of 
                ( this->crbegin (), this->crend (), s.cbegin (), s.cend (), traits::eq );
            return iter == this->crend () ? npos : reverse_distance ( this->crbegin (), iter);
            }
        
        size_type find_first_not_of(basic_string_ref s) const {
        	const_iterator iter = find_not_of ( this->cbegin (), this->cend (), s );
            return iter == this->cend () ? npos : std::distance ( this->cbegin (), iter );
            }
        
        size_type find_first_not_of(charT c) const {
            for ( const_iterator iter = this->cbegin (); iter != this->cend (); ++iter )
                if ( !traits::eq ( c, *iter ))
                    return std::distance ( this->cbegin (), iter );
            return npos;
            }
        
        size_type find_last_not_of(basic_string_ref s) const {
        	const_reverse_iterator iter = find_not_of ( this->crbegin (), this->crend (), s );
            return iter == this->crend () ? npos : reverse_distance ( this->crbegin (), iter );
            }
        
        size_type find_last_not_of(charT c) const {
            for ( const_reverse_iterator iter = this->crbegin (); iter != this->crend (); ++iter )
                if ( !traits::eq ( c, *iter ))
                    return reverse_distance ( this->crbegin (), iter );
            return npos;
            }

    private:
        template <typename r_iter>
        size_type reverse_distance ( r_iter first, r_iter last ) const {
            return len_ - 1 - std::distance ( first, last );
            }
        
        template <typename Iterator>
        Iterator find_not_of ( Iterator first, Iterator last, basic_string_ref s ) const {
			for ( ; first != last ; ++first )
				if ( 0 == traits::find ( s.ptr_, s.len_, *first ))
					return first;
			return last;
			}
		
       		
        
        const charT *ptr_;
        std::size_t len_;
        };
    
    // Comparison operators
    template<typename charT, typename traits>
    bool operator==(basic_string_ref<charT, traits> x, basic_string_ref<charT, traits> y) {
        if ( x.size () != y.size ()) return false;
        return x.compare(y) == 0;
        }
    
    template<typename charT, typename traits>
    bool operator!=(basic_string_ref<charT, traits> x, basic_string_ref<charT, traits> y) {
        if ( x.size () != y.size ()) return true;
        return x.compare(y) != 0;
        }

    template<typename charT, typename traits>
    bool operator<(basic_string_ref<charT, traits> x, basic_string_ref<charT, traits> y) {
        return x.compare(y) < 0;
        }

    template<typename charT, typename traits>
    bool operator>(basic_string_ref<charT, traits> x, basic_string_ref<charT, traits> y) {
        return x.compare(y) > 0;
        }

    template<typename charT, typename traits>
    bool operator<=(basic_string_ref<charT, traits> x, basic_string_ref<charT, traits> y) {
        return x.compare(y) <= 0;
        }

    template<typename charT, typename traits>
    bool operator>=(basic_string_ref<charT, traits> x, basic_string_ref<charT, traits> y) {
        return x.compare(y) >= 0;
        }

    
    // Inserter
    template<class charT, class traits>
    std::basic_ostream<charT, traits>&
    operator<<(std::basic_ostream<charT, traits>& os, const basic_string_ref<charT,traits>& str) {
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
        for ( typename basic_string_ref<charT, traits>::const_iterator iter = str.begin (); iter != str.end (); ++iter )
            os << *iter;
#else
        for ( charT x : str )
            os << x;
#endif
        return os;
        }
    
#if 0
    // numeric conversions
    //
    //  These are short-term implementations.
    //  In a production environment, I would rather avoid the copying.
    //
    int stoi (string_ref str, size_t* idx=0, int base=10) {
        return std::stoi ( std::string(str), idx, base );
        }
    
    long stol (string_ref str, size_t* idx=0, int base=10) {
        return std::stol ( std::string(str), idx, base );
        }
        
    unsigned long stoul (string_ref str, size_t* idx=0, int base=10) {
        return std::stoul ( std::string(str), idx, base );
        }
        
    long long stoll (string_ref str, size_t* idx=0, int base=10) {
        return std::stoll ( std::string(str), idx, base );
        }
    
    unsigned long long stoull (string_ref str, size_t* idx=0, int base=10) {
        return std::stoull ( std::string(str), idx, base );
        }
    
    float stof (string_ref str, size_t* idx=0) {
        return std::stof ( std::string(str), idx );
        }
        
    double stod (string_ref str, size_t* idx=0) {
        return std::stod ( std::string(str), idx );
        }
        
    long double stold (string_ref str, size_t* idx=0)  {
        return std::stold ( std::string(str), idx );
        }
    
    int  stoi (wstring_ref str, size_t* idx=0, int base=10) {
        return std::stoi ( std::wstring(str), idx, base );
        }
        
    long stol (wstring_ref str, size_t* idx=0, int base=10) {
        return std::stol ( std::wstring(str), idx, base );
        }
        
    unsigned long stoul (wstring_ref str, size_t* idx=0, int base=10) {
        return std::stoul ( std::wstring(str), idx, base );
        }
        
    long long stoll (wstring_ref str, size_t* idx=0, int base=10) {
        return std::stoll ( std::wstring(str), idx, base );
        }
        
    unsigned long long stoull (wstring_ref str, size_t* idx=0, int base=10) {
        return std::stoull ( std::wstring(str), idx, base );
        }
    
    float  stof (wstring_ref str, size_t* idx=0) {
        return std::stof ( std::wstring(str), idx );
        }
        
    double stod (wstring_ref str, size_t* idx=0) {
        return std::stod ( std::wstring(str), idx );
        }
        
    long double stold (wstring_ref str, size_t* idx=0) {
        return std::stold ( std::wstring(str), idx );
        }
#endif

}

#if 0
namespace std {
    // Hashing
    template<> struct hash<boost::string_ref>;
    template<> struct hash<boost::u16string_ref>;
    template<> struct hash<boost::u32string_ref>;
    template<> struct hash<boost::wstring_ref>;
}
#endif

#endif
