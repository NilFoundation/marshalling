//---------------------------------------------------------------------------//
// Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
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

#ifndef MARSHALLING_DESERIALIZE_HPP
#define MARSHALLING_DESERIALIZE_HPP

#include <nil/marshalling/marshalling_state.hpp>
#include <nil/marshalling/accumulators/marshalling.hpp>
#include <nil/marshalling/accumulators/parameters/buffer_length.hpp>
#include <nil/marshalling/accumulators/parameters/expected_status.hpp>
#include <nil/marshalling/detail/type_traits.hpp>

namespace nil {
    namespace marshalling {

        /*!
         * @defgroup marshalling Marshalling
         *
         * @brief Marshalling between two or more defined types
         *
         * @defgroup marshalling_algorithms Algorithms
         * @ingroup marshalling
         * @brief Algorithms are meant to provide marshalling interface similar to STL algorithms' one.
         */

        /*
         * Marshalling with both input and output types, which are marshalling types, not a std
         * iterator of elements with a marshalling type
         */

        /*!
         * @brief
         *
         * @ingroup marshalling_algorithms
         *
         * @tparam InputType
         * @tparam OutputType
         *
         * @param input_field
         * @param expectedStatus
         *
         * @return
         */
        template<typename InputType, typename OutputType>
        typename std::enable_if<marshalling::detail::is_marshalling_field<InputType>::value
                                    && marshalling::detail::is_marshalling_field<OutputType>::value,
                                OutputType>::type
            marshal(InputType input_field, status_type expectedStatus = status_type::success) {
            typedef accumulator_set<OutputType> accumulator_set_type;
            typedef typename boost::mpl::front<typename accumulator_set_type::features_type>::type accumulator_type;

            accumulator_set_type acc = accumulator_set_type(OutputType());

            acc(input_field, accumulators::buffer_length = 1, accumulators::expected_status = expectedStatus);

            return boost::accumulators::extract_result<accumulator_type>(acc);
        }

        /*!
         * @brief
         *
         * @ingroup marshalling_algorithms
         *
         * @tparam InputType
         * @tparam OutputType
         * @tparam OutputTypeAccumulator
         *
         * @param input_field
         * @param acc
         * @param expectedStatus
         *
         * @return
         */
        template<typename InputType, typename OutputType, typename OutputTypeAccumulator = accumulator_set<OutputType>>
        typename std::enable_if<boost::accumulators::detail::is_accumulator_set<OutputTypeAccumulator>::value
                                    && marshalling::detail::is_marshalling_field<InputType>::value
                                    && marshalling::detail::is_marshalling_field<OutputType>::value,
                                OutputType>::type
            marshal(InputType input_field, OutputTypeAccumulator &acc,
                    status_type expectedStatus = status_type::success) {

            typedef OutputTypeAccumulator accumulator_set_type;
            typedef typename boost::mpl::front<typename accumulator_set_type::features_type>::type accumulator_type;

            acc(input_field, accumulators::buffer_length = 1, accumulators::expected_status = expectedStatus);

            OutputType result = boost::accumulators::extract_result<accumulator_type>(acc);

            return result;
        }

        /*************************
         * Marshalling with input type, which is not a marshalling type and output type, which
         * is a marshalling type, not a std iterator of elements with a marshalling type
         * ***********************************/

        /*!
         * @brief
         *
         * @ingroup marshalling_algorithms
         *
         * @tparam OutputType
         * @tparam InputIterator
         *
         * @param first
         * @param last
         * @param expectedStatus
         *
         * @return
         */
        template<typename OutputType, typename InputIterator>
        typename std::enable_if<marshalling::detail::is_iterator<InputIterator>::value
                                    && marshalling::detail::is_marshalling_field<OutputType>::value,
                                OutputType>::type
            marshal(InputIterator first, InputIterator last, status_type expectedStatus = status_type::success) {
            typedef accumulator_set<OutputType> accumulator_set_type;
            typedef typename boost::mpl::front<typename accumulator_set_type::features_type>::type accumulator_type;

            accumulator_set_type acc = accumulator_set_type(OutputType());

            acc(first, accumulators::buffer_length = std::distance(first, last),
                accumulators::expected_status = expectedStatus);

            return boost::accumulators::extract_result<accumulator_type>(acc);
        }

        /*!
         * @brief
         *
         * @ingroup marshalling_algorithms
         *
         * @tparam OutputType
         * @tparam InputIterator
         * @tparam OutputTypeAccumulator
         *
         * @param first
         * @param last
         * @param acc
         * @param expectedStatus
         *
         * @return
         */
        template<typename OutputType, typename InputIterator,
                 typename OutputTypeAccumulator = accumulator_set<OutputType>>
        typename std::enable_if<boost::accumulators::detail::is_accumulator_set<OutputTypeAccumulator>::value
                                    && marshalling::detail::is_iterator<InputIterator>::value
                                    && marshalling::detail::is_marshalling_field<OutputType>::value,
                                OutputType>::type
            marshal(InputIterator first, InputIterator last, OutputTypeAccumulator &acc,
                    status_type expectedStatus = status_type::success) {

            typedef OutputTypeAccumulator accumulator_set_type;
            typedef typename boost::mpl::front<typename accumulator_set_type::features_type>::type accumulator_type;

            acc(first, accumulators::buffer_length = std::distance(first, last),
                accumulators::expected_status = expectedStatus);

            OutputType result = boost::accumulators::extract_result<accumulator_type>(acc);

            return result;
        }

        /*!
         * @brief
         *
         * @ingroup marshalling_algorithms
         *
         * @tparam OutputType
         * @tparam SinglePassRange
         *
         * @param rng
         * @param expectedStatus
         *
         * @return
         */
        template<typename OutputType, typename SinglePassRange>
        typename std::enable_if<marshalling::detail::is_range<SinglePassRange>::value
                                    && marshalling::detail::is_marshalling_field<OutputType>::value,
                                OutputType>::type
            marshal(const SinglePassRange &rng, status_type expectedStatus = status_type::success) {

            typedef accumulator_set<OutputType> accumulator_set_type;
            typedef typename boost::mpl::front<typename accumulator_set_type::features_type>::type accumulator_type;

            accumulator_set_type acc = accumulator_set_type(OutputType());

            acc(rng.begin(), accumulators::buffer_length = rng.size(), accumulators::expected_status = expectedStatus);

            return boost::accumulators::extract_result<accumulator_type>(acc);
        }

        /*!
         * @brief
         *
         * @ingroup marshalling_algorithms
         *
         * @tparam OutputType
         * @tparam SinglePassRange
         * @tparam OutputTypeAccumulator
         *
         * @param rng
         * @param acc
         * @param expectedStatus
         *
         * @return
         */
        template<typename OutputType, typename SinglePassRange,
                 typename OutputTypeAccumulator = accumulator_set<OutputType>>
        typename std::enable_if<boost::accumulators::detail::is_accumulator_set<OutputTypeAccumulator>::value
                                    && marshalling::detail::is_range<SinglePassRange>::value
                                    && marshalling::detail::is_marshalling_field<OutputType>::value,
                                OutputType>::type &
            marshal(const SinglePassRange &rng, OutputTypeAccumulator &acc,
                    status_type expectedStatus = status_type::success) {
            typedef OutputTypeAccumulator accumulator_set_type;
            typedef typename boost::mpl::front<typename accumulator_set_type::features_type>::type accumulator_type;

            acc(rng.begin(), accumulators::buffer_length = rng.size(), accumulators::expected_status = expectedStatus);

            return boost::accumulators::extract_result<accumulator_type>(acc);
        }

        /*************************
         * Marshalling with input type, which is a marshalling type and output type, which is a
         * std iterators of elements with a marshalling type ***********************************/

        /*!
         * @brief
         *
         * @ingroup marshalling_algorithms
         *
         * @tparam InputType
         * @tparam OutputIterator
         *
         * @param input_field
         * @param out
         * @param expectedStatus
         *
         * @return
         */
        template<typename InputType, typename OutputIterator>
        typename std::enable_if<marshalling::detail::is_marshalling_field<InputType>::value
                                    && marshalling::detail::is_iterator<OutputIterator>::value,
                                OutputIterator>::type
            marshal(InputType input_field, OutputIterator out, status_type expectedStatus = status_type::success) {

            using type_to_process = typename std::iterator_traits<OutputIterator>::value_type;

            // hardcoded to be little-endian by default. If the user wants to process a container
            // in other order (for example, in reverse or by skipping some first elements), he should
            // define type of the container using array_list and process it not by iterator, but by
            // processing it as marshaling type.
            using marhsalling_array_type
                = types::array_list<marshalling::field_type<nil::marshalling::option::little_endian>, type_to_process>;

            typedef accumulator_set<marhsalling_array_type> accumulator_set_type;
            typedef typename boost::mpl::front<typename accumulator_set_type::features_type>::type accumulator_type;

            accumulator_set_type acc = accumulator_set_type(marhsalling_array_type());

            acc(input_field, accumulators::buffer_length = 1, accumulators::expected_status = expectedStatus);

            marhsalling_array_type array_list_element = boost::accumulators::extract_result<accumulator_type>(acc);

            std::copy(array_list_element.value().begin(), array_list_element.value().end(), out);

            return out;
        }

        /*!
         * @brief
         *
         * @ingroup marshalling_algorithms
         *
         * @tparam InputType
         * @tparam OutputIterator
         * @tparam OutputTypeAccumulator
         *
         * @param input_field
         * @param out
         * @param acc
         * @param expectedStatus
         *
         * @return
         */
        template<typename InputType, typename OutputIterator, typename OutputTypeAccumulator>
        typename std::enable_if<boost::accumulators::detail::is_accumulator_set<OutputTypeAccumulator>::value
                                    && marshalling::detail::is_marshalling_field<InputType>::value
                                    && marshalling::detail::is_iterator<OutputIterator>::value,
                                OutputIterator>::type
            marshal(InputType input_field, OutputIterator out, OutputTypeAccumulator &acc,
                    status_type expectedStatus = status_type::success) {

            using type_to_process = typename std::iterator_traits<OutputIterator>::value_type;

            // hardcoded to be little-endian by default. If the user wants to process a container
            // in other order (for example, in reverse or by skipping some first elements), he should
            // define type of the container using array_list and process it not by iterator, but by
            // processing it as marshaling type.
            using marhsalling_array_type
                = types::array_list<marshalling::field_type<nil::marshalling::option::little_endian>, type_to_process>;
            using nil_marshalling_array_internal_sequential_container_type =
                typename marhsalling_array_type::value_type;

            typedef OutputTypeAccumulator accumulator_set_type;
            typedef typename boost::mpl::front<typename accumulator_set_type::features_type>::type accumulator_type;

            acc(input_field, accumulators::buffer_length = 1, accumulators::expected_status = expectedStatus);

            marhsalling_array_type array_list_element = boost::accumulators::extract_result<accumulator_type>(acc);

            std::copy(array_list_element.value().begin(), array_list_element.value().end(), out);

            return out;
        }

        /*
         * Marshalling with both input and output type, which are std iterators of elements
         * with a marshalling type
         */

        /*!
         * @brief
         *
         * @ingroup marshalling_algorithms
         *
         * @tparam InputIterator
         * @tparam OutputIterator
         *
         * @param first
         * @param last
         * @param out
         * @param expectedStatus
         *
         * @return
         */
        template<typename InputIterator, typename OutputIterator>
        typename std::enable_if<marshalling::detail::is_iterator<InputIterator>::value
                                    && marshalling::detail::is_iterator<OutputIterator>::value,
                                OutputIterator>::type
            marshal(InputIterator first, InputIterator last, OutputIterator out,
                    status_type expectedStatus = status_type::success) {

            using type_to_process = typename std::iterator_traits<OutputIterator>::value_type;

            // hardcoded to be little-endian by default. If the user wants to process a container
            // in other order (for example, in reverse or by skipping some first elements), he should
            // define type of the container using array_list and process it not by iterator, but by
            // processing it as marshaling type.
            using marhsalling_array_type
                = types::array_list<marshalling::field_type<nil::marshalling::option::little_endian>, type_to_process>;

            typedef accumulator_set<marhsalling_array_type> accumulator_set_type;
            typedef typename boost::mpl::front<typename accumulator_set_type::features_type>::type accumulator_type;

            accumulator_set_type acc = accumulator_set_type(marhsalling_array_type());

            acc(first, accumulators::buffer_length = std::distance(first, last),
                accumulators::expected_status = expectedStatus);

            marhsalling_array_type array_list_element = boost::accumulators::extract_result<accumulator_type>(acc);

            std::copy(array_list_element.value().begin(), array_list_element.value().end(), out);

            return out;
        }

        /*!
         * @brief
         *
         * @ingroup marshalling_algorithms
         *
         * @tparam InputIterator
         * @tparam OutputIterator
         * @tparam OutputTypeAccumulator
         *
         * @param first
         * @param last
         * @param out
         * @param acc
         * @param expectedStatus
         *
         * @return
         */
        template<typename InputIterator, typename OutputIterator, typename OutputTypeAccumulator>
        typename std::enable_if<boost::accumulators::detail::is_accumulator_set<OutputTypeAccumulator>::value
                                    && marshalling::detail::is_iterator<InputIterator>::value
                                    && marshalling::detail::is_iterator<OutputIterator>::value,
                                OutputIterator>::type
            marshal(InputIterator first, InputIterator last, OutputIterator out, OutputTypeAccumulator &acc,
                    status_type expectedStatus = status_type::success) {

            using type_to_process = typename std::iterator_traits<OutputIterator>::value_type;

            // hardcoded to be little-endian by default. If the user wants to process a container
            // in other order (for example, in reverse or by skipping some first elements), he should
            // define type of the container using array_list and process it not by iterator, but by
            // processing it as marshaling type.
            using marhsalling_array_type
                = types::array_list<marshalling::field_type<nil::marshalling::option::little_endian>, type_to_process>;

            typedef OutputTypeAccumulator accumulator_set_type;
            typedef typename boost::mpl::front<typename accumulator_set_type::features_type>::type accumulator_type;

            acc(first, accumulators::buffer_length = std::distance(first, last),
                accumulators::expected_status = expectedStatus);

            marhsalling_array_type array_list_element = boost::accumulators::extract_result<accumulator_type>(acc);

            std::copy(array_list_element.value().begin(), array_list_element.value().end(), out);

            return out;
        }

        /*!
         * @brief
         *
         * @ingroup marshalling_algorithms
         *
         * @tparam SinglePassRange
         * @tparam OutputIterator
         * @tparam OutputTypeAccumulator
         *
         * @param rng
         * @param expectedStatus
         *
         * @return
         */
        template<typename SinglePassRange, typename OutputIterator>
        typename std::enable_if<marshalling::detail::is_range<SinglePassRange>::value
                                    && marshalling::detail::is_iterator<OutputIterator>::value,
                                OutputIterator>::type
            marshal(const SinglePassRange &rng, OutputIterator out, status_type expectedStatus = status_type::success) {

            using type_to_process = typename std::iterator_traits<OutputIterator>::value_type;

            // hardcoded to be little-endian by default. If the user wants to process a container
            // in other order (for example, in reverse or by skipping some first elements), he should
            // define type of the container using array_list and process it not by iterator, but by
            // processing it as marshaling type.
            using marhsalling_array_type
                = types::array_list<marshalling::field_type<nil::marshalling::option::little_endian>, type_to_process>;

            typedef accumulator_set<marhsalling_array_type> accumulator_set_type;
            typedef typename boost::mpl::front<typename accumulator_set_type::features_type>::type accumulator_type;

            accumulator_set_type acc = accumulator_set_type(marhsalling_array_type());

            acc(rng.begin(), accumulators::buffer_length = rng.size(), accumulators::expected_status = expectedStatus);

            marhsalling_array_type array_list_element = boost::accumulators::extract_result<accumulator_type>(acc);

            std::copy(array_list_element.value().begin(), array_list_element.value().end(), out);

            return out;
        }

        /*!
         * @brief
         *
         * @ingroup marshalling_algorithms
         *
         * @tparam SinglePassRange
         * @tparam OutputIterator
         * @tparam OutputTypeAccumulator
         *
         * @param rng
         * @param acc
         * @param expectedStatus
         *
         * @return
         */
        template<typename SinglePassRange, typename OutputIterator, typename OutputTypeAccumulator>
        typename std::enable_if<boost::accumulators::detail::is_accumulator_set<OutputTypeAccumulator>::value
                                    && marshalling::detail::is_range<SinglePassRange>::value
                                    && marshalling::detail::is_iterator<OutputIterator>::value,
                                OutputIterator>::type
            marshal(const SinglePassRange &rng, OutputIterator out, OutputTypeAccumulator &acc,
                    status_type expectedStatus = status_type::success) {

            using type_to_process = typename std::iterator_traits<OutputIterator>::value_type;

            // hardcoded to be little-endian by default. If the user wants to process a container
            // in other order (for example, in reverse or by skipping some first elements), he should
            // define type of the container using array_list and process it not by iterator, but by
            // processing it as marshaling type.
            using marhsalling_array_type
                = types::array_list<marshalling::field_type<nil::marshalling::option::little_endian>, type_to_process>;

            typedef OutputTypeAccumulator accumulator_set_type;
            typedef typename boost::mpl::front<typename accumulator_set_type::features_type>::type accumulator_type;

            acc(rng.begin(), accumulators::buffer_length = rng.size(), accumulators::expected_status = expectedStatus);

            marhsalling_array_type array_list_element = boost::accumulators::extract_result<accumulator_type>(acc);

            std::copy(array_list_element.value().begin(), array_list_element.value().end(), out);

            return out;
        }
    }    // namespace marshalling
}    // namespace nil

#endif    // MARSHALLING_DESERIALIZE_HPP
