//---------------------------------------------------------------------------//
// Copyright (c) 2017-2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2020-2021 Nikita Kaskov <nbering@nil.foundation>
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------//

#ifndef MARSHALLING_BUNDLE_HPP
#define MARSHALLING_BUNDLE_HPP

#include <nil/detail/type_traits.hpp>

#include <nil/marshalling/status_type.hpp>
#include <nil/marshalling/options.hpp>
#include <nil/marshalling/types/bundle/basic_type.hpp>
#include <nil/marshalling/types/detail/adapt_basic_field.hpp>

#include <nil/marshalling/types/tag.hpp>

namespace nil {
    namespace marshalling {
        namespace types {

            /// @brief Bundles multiple fields into a single field.
            /// @details The class wraps nicely multiple fields and provides
            ///     expected single field API functions, such as length(), read(), write(),
            ///     valid(). It may be useful when a collection (nil::marshalling::types::array_list) of
            ///     complex fields is required.
            ///
            ///     Refer to @ref sec_field_tutorial_bundle for tutorial and usage examples.
            /// @tparam TFieldBase Base class for this field, expected to be a variant of
            ///     nil::marshalling::field_type.
            /// @tparam TMembers All wrapped fields bundled together in
            ///     <a href="http://en.cppreference.com/w/cpp/utility/tuple">std::tuple</a>.
            /// @tparam TOptions Zero or more options that modify/refine default behaviour
            ///     of the field.@n
            ///     Supported options are:
            ///     @li @ref nil::marshalling::option::default_value_initializer - All wrapped fields may
            ///         specify their independent default value initializers. It is
            ///         also possible to provide initializer for the bundle field which
            ///         will set appropriate values to the fields based on some
            ///         internal logic.
            ///     @li @ref nil::marshalling::option::contents_validator - All wrapped fields may specify
            ///         their independent validators. The bundle field considered to
            ///         be valid if all the wrapped fields are valid. This option though,
            ///         provides an ability to add extra validation logic that can
            ///         observe value of more than one wrapped fields. For example,
            ///         protocol specifies that if one specific field has value X, than
            ///         other field is NOT allowed to have value Y.
            ///     @li @ref nil::marshalling::option::contents_refresher - The default refreshing
            ///         behaviour is to call the @b refresh() member function of every
            ///         member field. This option provides an ability to set a custom
            ///         "refreshing" logic.
            ///     @li @ref nil::marshalling::option::custom_value_reader - It may be required to implement
            ///         custom reading functionality instead of default behaviour of
            ///         invoking read() member function of every member field. It is possible
            ///         to provide cusom reader functionality using nil::marshalling::option::custom_value_reader
            ///         option.
            ///     @li @ref nil::marshalling::option::has_custom_read
            ///     @li @ref nil::marshalling::option::has_custom_refresh
            ///     @li @ref nil::marshalling::option::empty_serialization
            ///     @li @ref nil::marshalling::option::version_storage
            /// @extends nil::marshalling::field_type
            /// @headerfile nil/marshalling/types/bundle.hpp
            /// @see @ref MARSHALLING_FIELD_MEMBERS_ACCESS()
            /// @see @ref MARSHALLING_FIELD_MEMBERS_ACCESS_NOTEMPLATE()
            template<typename TFieldBase, typename TMembers, typename... TOptions>
            class bundle : private detail::adapt_basic_field_type<detail::basic_bundle<TFieldBase, TMembers>, TOptions...> {
                using base_impl_type = detail::adapt_basic_field_type<detail::basic_bundle<TFieldBase, TMembers>, TOptions...>;
                static_assert(nil::detail::is_tuple<TMembers>::value,
                              "TMembers is expected to be a tuple of std::tuple<...>");

                static_assert(1U <= std::tuple_size<TMembers>::value,
                              "Number of members is expected to be at least 1.");

            public:
                /// @brief endian_type used for serialization.
                using typename base_impl_type::endian_type;

                /// @brief Version type
                using typename base_impl_type::version_type;

                /// @brief All the options provided to this class bundled into struct.
                using parsed_options_type = detail::options_parser<TOptions...>;

                /// @brief Tag indicating type of the field
                using tag = tag::bundle;

                /// @brief Value type.
                /// @details Same as TMemebers template argument, i.e. it is std::tuple
                ///     of all the wrapped fields.
                using typename base_impl_type::value_type;

                /// @brief Default constructor
                /// @details Invokes default constructor of every wrapped field
                bundle() = default;

                /// @brief Constructor
                explicit bundle(const value_type &val) : base_impl_type(val) {
                }

                /// @brief Constructor
                explicit bundle(value_type &&val) : base_impl_type(std::move(val)) {
                }

                /// @brief Get access to the stored tuple of fields.
                using base_impl_type::value;

                /// @brief Get length required to serialise bundled fields.
                /// @details Summarises all the results returned by the call to length() for
                ///     every field in the bundle.
                /// @return Number of bytes it will take to serialise the field value.
                using base_impl_type::length;

                /// @brief Get length required to serialise specified bundled member fields.
                /// @details Summarises all the results returned by the call to length() for
                ///     every specified field in the bundle.
                /// @tparam TFromIdx Index of the field (included) from which the counting must start.
                /// @return Number of bytes it will take to serialise the specified member fields.
                /// @pre TFromIdx < std::tuple_size<value_type>::value
                using base_impl_type::length_from;

                /// @brief Get length required to serialise specified bundled member fields.
                /// @details Summarises all the results returned by the call to length() for
                ///     every specified field in the bundle.
                /// @tparam TUntilIdx Index of the field (not included) until which the counting must be performed.
                /// @return Number of bytes it will take to serialise the specified member fields.
                /// @pre TUntilIdx <= std::tuple_size<value_type>::value
                using base_impl_type::length_until;

                /// @brief Get length required to serialise specified bundled member fields.
                /// @details Summarises all the results returned by the call to length() for
                ///     every specified field in the bundle.
                /// @tparam TFromIdx Index of the field (included) from which the counting must start.
                /// @tparam TUntilIdx Index of the field (not included) until which the counting must be performed.
                /// @return Number of bytes it will take to serialise the specified member fields.
                /// @pre TUntilIdx <= std::tuple_size<value_type>::value
                /// @pre TFromIdx < TUntilIdx
                using base_impl_type::length_from_until;

                /// @brief Get minimal length that is required to serialise all bundled fields.
                /// @return Minimal number of bytes required serialise the field value.
                using base_impl_type::min_length;

                /// @brief Get minimal length that is required to serialise specified bundled fields.
                /// @tparam TFromIdx Index of the field (included) from which the counting must start.
                /// @return Minimal number of bytes required to serialise the specified member fields.
                /// @pre TFromIdx < std::tuple_size<value_type>::value
                using base_impl_type::min_length_from;

                /// @brief Get minimal length that is required to serialise specified bundled fields.
                /// @tparam TUntilIdx Index of the field (not included) until which the counting must be performed.
                /// @return Minimal number of bytes required to serialise the specified member fields.
                /// @pre TUntilIdx <= std::tuple_size<value_type>::value
                using base_impl_type::min_length_until;

                /// @brief Get minimal length that is required to serialise specified bundled fields.
                /// @tparam TFromIdx Index of the field (included) from which the counting must start.
                /// @tparam TUntilIdx Index of the field (not included) until which the counting must be performed.
                /// @return Minimal number of bytes required to serialise the specified member fields.
                /// @pre TUntilIdx <= std::tuple_size<value_type>::value
                /// @pre TFromIdx < TUntilIdx
                using base_impl_type::min_length_from_until;

                /// @brief Get maximal length that is required to serialise all bundled fields.
                /// @return Maximal number of bytes required serialise the field value.
                using base_impl_type::max_length;

                /// @brief Get maximal length that is required to serialise specified bundled fields.
                /// @tparam TFromIdx Index of the field (included) from which the counting must start.
                /// @return Minimal number of bytes required to serialise the specified member fields.
                /// @pre TFromIdx < std::tuple_size<value_type>::value
                using base_impl_type::max_length_from;

                /// @brief Get maximal length that is required to serialise specified bundled fields.
                /// @tparam TUntilIdx Index of the field (not included) until which the counting must be performed.
                /// @return Minimal number of bytes required to serialise the specified member fields.
                /// @pre TUntilIdx <= std::tuple_size<value_type>::value
                using base_impl_type::max_length_until;

                /// @brief Get maximal length that is required to serialise specified bundled fields.
                /// @tparam TFromIdx Index of the field (included) from which the counting must start.
                /// @tparam TUntilIdx Index of the field (not included) until which the counting must be performed.
                /// @return Minimal number of bytes required to serialise the specified member fields.
                /// @pre TUntilIdx <= std::tuple_size<value_type>::value
                /// @pre TFromIdx < TUntilIdx
                using base_impl_type::max_length_from_until;

                /// @brief Read field value from input data sequence
                /// @details Invokes read() member function over every bundled field.
                /// @param[in, out] iter Iterator to read the data.
                /// @param[in] size Number of bytes available for reading.
                /// @return Status of read operation.
                /// @post Iterator is advanced.
                using base_impl_type::read;

                /// @brief Read selected number of member fields (from specified index).
                /// @details Similar to @ref read(), but invokes @b read() member function
                ///     of only selected member fields.
                /// @tparam TFromIdx Index of the member field (included) from which the
                ///     "read" operation starts
                /// @param[in, out] iter Iterator to read the data.
                /// @param[in] size Number of bytes available for reading.
                /// @return Status of read operation.
                /// @pre TFromIdx < std::tuple_size<value_type>
                /// @post Iterator is advanced.
                using base_impl_type::read_from;

                /// @brief Read selected number of member fields (until specified index).
                /// @details Similar to @ref read(), but invokes @b read() member function
                ///     of only selected member fields.
                /// @tparam TUntilIdx Index of the member field (NOT included) until which the
                ///     "read" operation continues.
                /// @param[in, out] iter Iterator to read the data.
                /// @param[in] size Number of bytes available for reading.
                /// @return Status of read operation.
                /// @pre TUntilIdx <= std::tuple_size<value_type>
                /// @post Iterator is advanced.
                using base_impl_type::read_until;

                /// @brief Read selected number of member fields (between specified indices).
                /// @details Similar to @ref read(), but invokes @b read() member function
                ///     of only selected member fields.
                /// @tparam TFromIdx Index of the member field (included) from which the
                ///     "read" operation starts
                /// @tparam TUntilIdx Index of the member field (NOT included) until which the
                ///     "read" operation continues.
                /// @param[in, out] iter Iterator to read the data.
                /// @param[in] size Number of bytes available for reading.
                /// @return Status of read operation.
                /// @pre TUntilIdx <= std::tuple_size<value_type>
                /// @pre TFromIdx < TUntilIdx
                /// @post Iterator is advanced.
                using base_impl_type::read_from_until;

                /// @brief Read field value from input data sequence without error check and status report.
                /// @details Similar to @ref read(), but doesn't perform any correctness
                ///     checks and doesn't report any failures.
                /// @param[in, out] iter Iterator to read the data.
                /// @post Iterator is advanced.
                using base_impl_type::read_no_status;

                /// @brief Read selected member fields from input data sequence
                ///     without error check and status report.
                /// @details Similar to @ref read_no_status(), but invokes @b read_no_status()
                ///     of only selected member fields.
                /// @tparam TFromIdx Index of the member field (included) from which the
                ///     "read" operation starts
                /// @param[in, out] iter Iterator to read the data.
                /// @pre TFromIdx < std::tuple_size<value_type>
                /// @post Iterator is advanced.
                using base_impl_type::read_from_no_status;

                /// @brief Read selected member fields from input data sequence
                ///     without error check and status report.
                /// @details Similar to @ref read_no_status(), but invokes @b read_no_status()
                ///     of only selected member fields.
                /// @tparam TUntilIdx Index of the member field (NOT included) until which the
                ///     "read" operation continues.
                /// @param[in, out] iter Iterator to read the data.
                /// @pre TUntilIdx <= std::tuple_size<value_type>
                /// @post Iterator is advanced.
                using base_impl_type::read_until_no_status;

                /// @brief Read selected member fields from input data sequence
                ///     without error check and status report.
                /// @details Similar to @ref read_no_status(), but invokes @b read_no_status()
                ///     of only selected member fields.
                /// @tparam TFromIdx Index of the member field (included) from which the
                ///     "read" operation starts
                /// @tparam TUntilIdx Index of the member field (NOT included) until which the
                ///     "read" operation continues.
                /// @param[in, out] iter Iterator to read the data.
                /// @pre TUntilIdx <= std::tuple_size<value_type>
                /// @pre TFromIdx < TUntilIdx
                /// @post Iterator is advanced.
                using base_impl_type::read_from_until_no_status;

                /// @brief Write current field value to output data sequence
                /// @details Invokes write() member function over every bundled field.
                /// @param[in, out] iter Iterator to write the data.
                /// @param[in] size Maximal number of bytes that can be written.
                /// @return Status of write operation.
                /// @post Iterator is advanced.
                using base_impl_type::write;

                /// @brief Write selected member fields to output data sequence.
                /// @details Similar to @ref write(), but invokes @b write member
                ///     function of only selected member fields.
                /// @tparam TFromIdx Index of the member field (included) from which the
                ///     "write" operation starts.
                /// @param[in, out] iter Iterator to write the data.
                /// @param[in] size Maximal number of bytes that can be written.
                /// @return Status of write operation.
                /// @pre TFromIdx < std::tuple_size<value_type>
                /// @post Iterator is advanced.
                using base_impl_type::write_from;

                /// @brief Write selected member fields to output data sequence.
                /// @details Similar to @ref write(), but invokes @b write member
                ///     function of only selected member fields.
                /// @tparam TUntilIdx Index of the member field (NOT included) until which the
                ///     "write" operation continues.
                /// @param[in, out] iter Iterator to write the data.
                /// @param[in] size Maximal number of bytes that can be written.
                /// @return Status of write operation.
                /// @pre TUntilIdx <= std::tuple_size<value_type>
                /// @post Iterator is advanced.
                using base_impl_type::write_until;

                /// @brief Write selected member fields to output data sequence.
                /// @details Similar to @ref write(), but invokes @b write member
                ///     function of only selected member fields.
                /// @tparam TFromIdx Index of the member field (included) from which the
                ///     "write" operation starts.
                /// @tparam TUntilIdx Index of the member field (NOT included) until which the
                ///     "write" operation continues.
                /// @param[in, out] iter Iterator to write the data.
                /// @param[in] size Maximal number of bytes that can be written.
                /// @return Status of write operation.
                /// @pre TUntilIdx <= std::tuple_size<value_type>
                /// @pre TFromIdx < TUntilIdx
                /// @post Iterator is advanced.
                using base_impl_type::write_from_until;

                /// @brief Write current field value to output data sequence  without error check and status report.
                /// @details Similar to @ref write(), but doesn't perform any correctness
                ///     checks and doesn't report any failures.
                /// @param[in, out] iter Iterator to write the data.
                /// @post Iterator is advanced.
                using base_impl_type::write_no_status;

                /// @brief Write selected member fields to output data sequence
                ///     without error check and status report.
                /// @details Similar to @ref write_no_status(), but invokes @b write_no_status()
                ///     of only selected member fields.
                /// @tparam TFromIdx Index of the member field (included) from which the
                ///     "write" operation starts
                /// @param[in, out] iter Iterator to read the data.
                /// @pre TFromIdx < std::tuple_size<value_type>
                /// @post Iterator is advanced.
                using base_impl_type::write_from_no_status;

                /// @brief Write selected member fields to output data sequence
                ///     without error check and status report.
                /// @details Similar to @ref write_no_status(), but invokes @b write_no_status()
                ///     of only selected member fields.
                /// @tparam TUntilIdx Index of the member field (NOT included) until which the
                ///     "write" operation continues.
                /// @param[in, out] iter Iterator to read the data.
                /// @pre TUntilIdx <= std::tuple_size<value_type>
                /// @post Iterator is advanced.
                using base_impl_type::write_until_no_status;

                /// @brief Write selected member fields to output data sequence
                ///     without error check and status report.
                /// @details Similar to @ref write_no_status(), but invokes @b write_no_status()
                ///     of only selected member fields.
                /// @tparam TFromIdx Index of the member field (included) from which the
                ///     "write" operation starts
                /// @tparam TUntilIdx Index of the member field (NOT included) until which the
                ///     "write" operation continues.
                /// @param[in, out] iter Iterator to read the data.
                /// @pre TUntilIdx <= std::tuple_size<value_type>
                /// @post Iterator is advanced.
                using base_impl_type::write_from_until_no_status;

                /// @brief Check validity of all the bundled fields.
                using base_impl_type::valid;

                /// @brief Refresh the field's contents
                /// @details Calls refresh() member function on every member field, will
                ///     return @b true if any of the calls returns @b true.
                using base_impl_type::refresh;

                /// @brief Compile time check if this class is version dependent
                static constexpr bool is_version_dependent() {
                    return parsed_options_type::has_custom_version_update || base_impl_type::is_version_dependent();
                }

                // /// @brief Get version of the field.
                // /// @details Exists only if @ref nil::marshalling::option::version_storage option has been provided.
                // version_type get_version() const {
                //     return base_impl_type::get_version();
                // }

                /// @brief Default implementation of version update.
                /// @return @b true in case the field contents have changed, @b false otherwise
                bool set_version(version_type version) {
                    return base_impl_type::set_version(version);
                }

            protected:
                using base_impl_type::read_data;
                using base_impl_type::write_data;

            private:
                static_assert(
                    !parsed_options_type::has_ser_offset,
                    "nil::marshalling::option::num_value_ser_offset option is not applicable to bundle field");
                static_assert(!parsed_options_type::has_fixed_length_limit,
                              "nil::marshalling::option::fixed_length option is not applicable to bundle field");
                static_assert(!parsed_options_type::has_fixed_bit_length_limit,
                              "nil::marshalling::option::fixed_bit_length option is not applicable to bundle field");
                static_assert(!parsed_options_type::has_var_length_limits,
                              "nil::marshalling::option::var_length option is not applicable to bundle field");
                static_assert(!parsed_options_type::has_sequence_elem_length_forcing,
                              "nil::marshalling::option::SequenceElemLengthForcingEnabled option is not applicable to "
                              "bundle field");
                static_assert(
                    !parsed_options_type::has_sequence_size_forcing,
                    "nil::marshalling::option::sequence_size_forcing_enabled option is not applicable to bundle field");
                static_assert(
                    !parsed_options_type::has_sequence_length_forcing,
                    "nil::marshalling::option::sequence_length_forcing_enabled option is not applicable to bundle field");
                static_assert(!parsed_options_type::has_sequence_fixed_size,
                              "nil::marshalling::option::sequence_fixed_size option is not applicable to bundle field");
                static_assert(!parsed_options_type::has_sequence_fixed_size_use_fixed_size_storage,
                              "nil::marshalling::option::SequenceFixedSizeUseFixedSizeStorage option is not applicable "
                              "to bundle field");
                static_assert(
                    !parsed_options_type::has_sequence_size_field_prefix,
                    "nil::marshalling::option::sequence_size_field_prefix option is not applicable to bundle field");
                static_assert(!parsed_options_type::has_sequence_ser_length_field_prefix,
                              "nil::marshalling::option::sequence_ser_length_field_prefix option is not applicable to "
                              "bundle field");
                static_assert(!parsed_options_type::has_sequence_elem_ser_length_field_prefix,
                              "nil::marshalling::option::sequence_elem_ser_length_field_prefix option is not "
                              "applicable to bundle field");
                static_assert(!parsed_options_type::has_sequence_elem_fixed_ser_length_field_prefix,
                              "nil::marshalling::option::SequenceElemSerLengthFixedFieldPrefix option is not "
                              "applicable to bundle field");
                static_assert(!parsed_options_type::has_sequence_trailing_field_suffix,
                              "nil::marshalling::option::sequence_trailing_field_suffix option is not applicable to "
                              "bundle field");
                static_assert(!parsed_options_type::has_sequence_termination_field_suffix,
                              "nil::marshalling::option::sequence_termination_field_suffix option is not applicable to "
                              "bundle field");
                static_assert(!parsed_options_type::has_fixed_size_storage,
                              "nil::marshalling::option::fixed_size_storage option is not applicable to bundle field");
                static_assert(!parsed_options_type::has_custom_storage_type,
                              "nil::marshalling::option::custom_storage_type option is not applicable to bundle field");
                static_assert(!parsed_options_type::has_scaling_ratio,
                              "nil::marshalling::option::scaling_ratio_type option is not applicable to bundle field");
                static_assert(!parsed_options_type::has_units,
                              "nil::marshalling::option::Units option is not applicable to bundle field");
                static_assert(!parsed_options_type::has_orig_data_view,
                              "nil::marshalling::option::orig_data_view option is not applicable to bundle field");
                static_assert(!parsed_options_type::has_multi_range_validation,
                              "nil::marshalling::option::valid_num_value_range (or similar) option is not applicable "
                              "to bundle field");
                static_assert(!parsed_options_type::has_versions_range,
                              "nil::marshalling::option::exists_between_versions (or similar) option is not applicable "
                              "to bundle field");
                static_assert(!parsed_options_type::has_invalid_by_default,
                              "nil::marshalling::option::invalid_by_default option is not applicable to bundle field");
            };

            /// @brief Equality comparison operator.
            /// @param[in] field1 First field.
            /// @param[in] field2 Second field.
            /// @return true in case fields are equal, false otherwise.
            /// @related bundle
            template<typename TFieldBase, typename TMembers, typename... TOptions>
            bool operator==(const bundle<TFieldBase, TMembers, TOptions...> &field1,
                            const bundle<TFieldBase, TMembers, TOptions...> &field2) {
                return field1.value() == field2.value();
            }

            /// @brief Non-equality comparison operator.
            /// @param[in] field1 First field.
            /// @param[in] field2 Second field.
            /// @return true in case fields are NOT equal, false otherwise.
            /// @related bundle
            template<typename TFieldBase, typename TMembers, typename... TOptions>
            bool operator!=(const bundle<TFieldBase, TMembers, TOptions...> &field1,
                            const bundle<TFieldBase, TMembers, TOptions...> &field2) {
                return field1.value() != field2.value();
            }

            /// @brief Equivalence comparison operator.
            /// @param[in] field1 First field.
            /// @param[in] field2 Second field.
            /// @related bundle
            template<typename TFieldBase, typename TMembers, typename... TOptions>
            bool operator<(const bundle<TFieldBase, TMembers, TOptions...> &field1,
                           const bundle<TFieldBase, TMembers, TOptions...> &field2) {
                return field1.value() < field2.value();
            }

            /// @brief Equivalence comparison operator.
            /// @param[in] field1 First field.
            /// @param[in] field2 Second field.
            /// @related bundle
            template<typename TFieldBase, typename TMembers, typename... TOptions>
            bool operator<=(const bundle<TFieldBase, TMembers, TOptions...> &field1,
                            const bundle<TFieldBase, TMembers, TOptions...> &field2) {
                return field1.value() <= field2.value();
            }

            /// @brief Equivalence comparison operator.
            /// @param[in] field1 First field.
            /// @param[in] field2 Second field.
            /// @related bundle
            template<typename TFieldBase, typename TMembers, typename... TOptions>
            bool operator>(const bundle<TFieldBase, TMembers, TOptions...> &field1,
                           const bundle<TFieldBase, TMembers, TOptions...> &field2) {
                return field1.value() > field2.value();
            }

            /// @brief Equivalence comparison operator.
            /// @param[in] field1 First field.
            /// @param[in] field2 Second field.
            /// @related bundle
            template<typename TFieldBase, typename TMembers, typename... TOptions>
            bool operator>=(const bundle<TFieldBase, TMembers, TOptions...> &field1,
                            const bundle<TFieldBase, TMembers, TOptions...> &field2) {
                return field1.value() >= field2.value();
            }

            /// @brief Upcast type of the field definition to its parent nil::marshalling::types::bundle type
            ///     in order to have access to its internal types.
            /// @related nil::marshalling::types::bundle
            template<typename TFieldBase, typename TMembers, typename... TOptions>
            inline bundle<TFieldBase, TMembers, TOptions...> &
                to_field_base(bundle<TFieldBase, TMembers, TOptions...> &field) {
                return field;
            }

            /// @brief Upcast type of the field definition to its parent nil::marshalling::types::bundle type
            ///     in order to have access to its internal types.
            /// @related nil::marshalling::types::bundle
            template<typename TFieldBase, typename TMembers, typename... TOptions>
            inline const bundle<TFieldBase, TMembers, TOptions...> &
                to_field_base(const bundle<TFieldBase, TMembers, TOptions...> &field) {
                return field;
            }

        }    // namespace types
    }        // namespace marshalling
}    // namespace nil
#endif    // MARSHALLING_BUNDLE_HPP
